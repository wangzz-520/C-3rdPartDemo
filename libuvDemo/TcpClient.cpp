#include "TcpClient.h"

TcpClient::TcpClient(std::string server_ip, uint16_t server_port)
{
	int ret = 0;
	uv_loop_init(&m_loop);

	this->m_serverIp = server_ip;
	this->m_serverPort = server_port;
	this->m_interval = 1000;//ms
	this->m_sendInterval = 10;//ms

	ret = uv_mutex_init(&this->m_mutex);
	if (ret)
	{
		std::cout << "uv_mutex_init error" << std::endl;
		return;
	}

	ret = uv_timer_init(&this->m_loop, &this->m_sendTimer);						// 发送定时器
	if (ret)
	{
		std::cout << "uv_timer_init error" << std::endl;
		return;
	}

	ret = uv_async_init(&this->m_loop, &this->m_sendAsync, sendAsync);
	if (ret)
	{
		std::cout << "uv_async_init error" << std::endl;
		return;
	}

	ret = uv_timer_init(&this->m_loop, &this->m_reconnectTimer);			// 重连定时器
	if (ret)
	{
		std::cout << "uv_timer_init error" << std::endl;
		return;
	}

	ret = uv_tcp_init(&this->m_loop, &this->m_socket);
	if (ret)
	{
		std::cout << "uv_tcp_init error" << std::endl;
		return;
	}

	ret = uv_tcp_nodelay(&this->m_socket, 1);
	if (ret)
	{
		std::cout << "uv_tcp_nodelay error" << std::endl;
		return;
	}

	printf("uv_tcp_nodelay\n");

	ret = uv_tcp_keepalive(&this->m_socket, 1, 60);
	if (ret)
	{
		std::cout << "uv_tcp_keepalive error" << std::endl;
		return;
	}

	printf("uv_tcp_keepalive\n");

	ret = uv_ip4_addr(server_ip.c_str(), server_port, &this->m_dest);
	if (ret)
	{
		std::cout << "uv_ip4_addr error" << std::endl;
		return;
	}

	ret = uv_tcp_connect(&this->m_connectReq, &this->m_socket, (const sockaddr*)&this->m_dest, connect);
	if (ret)
	{
		std::cout << "uv_tcp_connect error" << std::endl;
		return;
	}

	this->m_sendTimer.data = this;
	this->m_sendAsync.data = this;

	this->m_socket.data = this;
	this->m_connectReq.data = this;
	this->m_reconnectTimer.data = this;
}


void TcpClient::sendAsync(uv_async_t *handle)
{
	TcpClient *tmpClient = (TcpClient *)handle->data;

	uv_mutex_lock(&tmpClient->m_mutex);
	if (tmpClient->m_isConnect)
	{
		std::string message = "Hello, Server!\n";
		uv_write_t *write_reqs = &(tmpClient->m_writeReq);
		uv_stream_t* stream = tmpClient->m_connectReq.handle;
		uv_buf_t buf = uv_buf_init((char*)message.c_str(), message.length());

		uv_write(write_reqs, stream, &buf, 1, [](uv_write_t* req, int status) {
			free(req);
			if (status < 0) {
				std::cerr << "Write error: " << uv_strerror(status) << std::endl;
			}
		});
	}
	uv_mutex_unlock(&tmpClient->m_mutex);
}

void TcpClient::connect(uv_connect_t* handle, int status)
{
	TcpClient *tmpClient = (TcpClient *)handle->data;
	int32_t iret = 0;

	if (status)
	{
		tmpClient->m_isConnect = false;
		std::cerr << "Connect error: " << uv_strerror(status) << std::endl;

		tmpClient->startReconnect();
		uv_close((uv_handle_t *)&tmpClient->m_socket, close);
		return;
	}

	iret = uv_read_start(handle->handle, [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
		buf->base = (char*)malloc(suggested_size);
		buf->len = suggested_size;
	}, read);	// 正常连接，开始接收数据
	if (iret)
	{
		std::cout << "uv_read_start error" << std::endl;
	}
	else
	{
		tmpClient->m_isConnect = true;
		tmpClient->startSend();
	}

	if (tmpClient->m_isReconnect)
	{
		tmpClient->stopReconnect();
	}
}


void TcpClient::close(uv_handle_t* handle)
{
	TcpClient *client = (TcpClient*)handle->data;

	client->m_isClosed = true;
}

void TcpClient::read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{

}

void TcpClient::startReconnect()
{
	m_isReconnect = true;
	this->m_socket.data = this;
}

void TcpClient::stopReconnect()
{
	this->m_isConnect = false;
	this->m_socket.data = this;
	uv_timer_stop(&this->m_reconnectTimer);
}

void TcpClient::startSend()
{
	uv_timer_start(&this->m_sendTimer, sendTimer, this->m_sendInterval, 0);
}

void TcpClient::stopSend()
{

}

void TcpClient::sendTimer(uv_timer_t* handle)
{
	TcpClient *tmpClient = (TcpClient *)handle->data;

	uv_async_send(&tmpClient->m_sendAsync);
}
