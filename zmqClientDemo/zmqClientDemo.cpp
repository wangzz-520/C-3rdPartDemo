#include <zmq.h>
#include <stdio.h>
#include <string.h>


//PUSH / PULL
int main(void) 
{
	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *puller = zmq_socket(context, ZMQ_PULL);

	// 连接套接字到服务器
	zmq_connect(puller, "tcp://localhost:5555");

	while (1) {
		// 接收消息
		char buffer[10] = {0};
		zmq_recv(puller, buffer, 10, 0);
		printf("Received message: [%s]\n", buffer);
	}

	// 清理资源
	zmq_close(puller);
	zmq_ctx_term(context);

	return 0;
}



//PUB/SUB
int main3()
{
	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *subscriber = zmq_socket(context, ZMQ_SUB);

	// 连接套接字到服务器
	zmq_connect(subscriber, "tcp://localhost:5555");

	// 订阅一个主题
	zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);

	while (1) {
		// 接收消息
		char buffer[10] = { 0 };
		zmq_recv(subscriber, buffer, 10, 0);
		printf("Received message: [%s]\n", buffer);
	}

	// 清理资源
	zmq_close(subscriber);
	zmq_ctx_term(context);

	return 0;
}


//REQ/REP
int main2()
{
	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *sender = zmq_socket(context, ZMQ_REQ);

	// 连接套接字到服务器
	zmq_connect(sender, "tcp://127.0.0.1:5555");

	// 发送请求
	char buffer[10] = {0};
	strcpy(buffer, "Hello");
	zmq_send(sender, buffer, 5, 0);

	// 接收应答
	zmq_recv(sender, buffer, 10, 0);
	printf("Received response: [%s]\n", buffer);

	// 清理资源
	zmq_close(sender);
	zmq_ctx_term(context);

	return 0;
}
