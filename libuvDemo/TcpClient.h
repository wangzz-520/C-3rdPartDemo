#pragma once

#include <iostream>
#include <string>
#include "uv.h"

class TcpClient
{
public:
	TcpClient(std::string server_ip, uint16_t server_port);

public:
	static void sendAsync(uv_async_t *handle);
	static void connect(uv_connect_t* handle, int status);
	static void close(uv_handle_t* handle);
	static void read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void sendTimer(uv_timer_t* handle);

	void startReconnect();
	void stopReconnect();

	void startSend();
	void stopSend();

private:
	std::string m_serverIp;
	uint16_t m_serverPort = 0;

	uv_mutex_t m_mutex;
	uv_loop_t m_loop;

	uv_tcp_t m_socket;

	struct sockaddr_in m_dest;

	bool m_isConnect = false;
	bool m_isReconnect = true;
	bool m_isClosed = false;

	uv_timer_t m_sendTimer;
	uv_async_t m_sendAsync;

	uv_timer_t m_reconnectTimer;

	uv_connect_t m_connectReq;

	uv_write_t m_writeReq;

	int m_interval = 0;
	int m_sendInterval = 0;
};

