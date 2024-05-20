#include <iostream>
#include <string>
extern "C"
{
#include "uv.h"
}


#if 0
void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		// 如果读取错误或连接已关闭，释放内存并关闭客户端
		uv_close((uv_handle_t*)client, NULL);
		free(buf->base);
		return;
	}

	std::string message = "Hello, World!\n";
	uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t wrbuf = uv_buf_init((char*)message.c_str(), message.length());
	uv_write(req, client, &wrbuf, 1, [](uv_write_t* req, int status) {
		free(req);
		if (status < 0) {
			std::cerr << "Write error: " << uv_strerror(status) << std::endl;
		}
	});

	// 释放内存
	free(buf->base);
}

void on_new_connection(uv_stream_t* server, int status) {
	if (status < 0) {
		std::cerr << "New connection error: " << uv_strerror(status) << std::endl;
		return;
	}

	uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(uv_default_loop(), client);
	if (uv_accept(server, (uv_stream_t*)client) == 0) {
		uv_read_start((uv_stream_t*)client, on_alloc, on_read);
	}
	else {
		uv_close((uv_handle_t*)client, NULL);
	}
}

int main() 
{
	uv_tcp_t server;
	uv_tcp_init(uv_default_loop(), &server);

	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", 8080, &addr);

	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
	if (r) {
		std::cerr << "Listen error: " << uv_strerror(r) << std::endl;
		return 1;
	}

	std::cout << "Listening on port 8080..." << std::endl;
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	return 0;
}
#endif

void on_connect(uv_connect_t* req, int status) {
	if (status < 0) {
		std::cerr << "Connect error: " << uv_strerror(status) << std::endl;
		return;
	}

	// 连接成功，发送数据
	uv_stream_t* stream = req->handle;
	std::string message = "Hello, Server!\n";
	uv_write_t* write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t buf = uv_buf_init((char*)message.c_str(), message.length());
	uv_write(write_req, stream, &buf, 1, [](uv_write_t* req, int status) {
		free(req);
		if (status < 0) {
			std::cerr << "Write error: " << uv_strerror(status) << std::endl;
		}
	});
}

void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		// 如果读取错误或连接已关闭，释放内存并关闭客户端
		uv_close((uv_handle_t*)client, NULL);
		free(buf->base);
		return;
	}

	// 打印接收到的数据
	std::cout.write(buf->base, nread);

	// 释放内存
	free(buf->base);
}

int main() {
	uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(uv_default_loop(), socket);
	uv_udp_init
	struct sockaddr_in dest;
	uv_ip4_addr("127.0.0.1", 8080, &dest);

	uv_connect_t* connect_req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	uv_tcp_connect(connect_req, socket, (const struct sockaddr*)&dest, on_connect);

	// 开始读取数据
	uv_read_start((uv_stream_t*)socket, [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
		buf->base = (char*)malloc(suggested_size);
		buf->len = suggested_size;
	}, on_read);

	std::cout << "Connecting to server..." << std::endl;
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	// 清理资源
	free(socket);
	free(connect_req);

	return 0;
}