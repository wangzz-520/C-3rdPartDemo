#include <zmq.h>
#include <stdio.h>
#include <string.h>

//PUSH / PULL
int main(void)
{
	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *pusher = zmq_socket(context, ZMQ_PUSH);

	// 绑定套接字到端口
	zmq_bind(pusher, "tcp://*:5555");

	while (1) {
		// 发送消息
		char buffer[10] = { 0 };
		sprintf(buffer, "Hello");
		zmq_send(pusher, buffer, strlen(buffer), 0);
		printf("Sent message: [%s]\n", buffer);
	}

	// 清理资源
	zmq_close(pusher);
	zmq_ctx_term(context);

	return 0;
}

//PUB/SUB
int main3(void)
{
	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *publisher = zmq_socket(context, ZMQ_PUB);

	// 绑定套接字到端口
	zmq_bind(publisher, "tcp://*:5555");

	while (1) {
		// 发送消息
		char buffer[10] = {0};
		sprintf(buffer, "Hello");
		zmq_send(publisher, buffer, strlen(buffer), 0);
		printf("Sent message: [%s]\n", buffer);
	}

	// 清理资源
	zmq_close(publisher);
	zmq_ctx_term(context);

	return 0;
}


//REQ/REP
int main2(void) 
{

	// 创建上下文
	void *context = zmq_ctx_new();

	// 创建套接字
	void *receiver = zmq_socket(context, ZMQ_REP);

	// 绑定套接字到端口
	zmq_bind(receiver, "tcp://127.0.0.1:5555");

	while (1) {
		// 接收消息
		char buffer[10] = { 0 };
		zmq_recv(receiver, buffer, 10, 0);
		printf("Received request: [%s]\n", buffer);

		// 发送应答
		zmq_send(receiver, "World", 5, 0);
	}

	// 清理资源
	zmq_close(receiver);
	zmq_ctx_term(context);

	return 0;
}