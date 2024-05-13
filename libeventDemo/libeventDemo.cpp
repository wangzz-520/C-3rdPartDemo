#include <stdio.h>
#include <string.h>
#include <event.h>
#include <evhttp.h>

void request_handler(struct evhttp_request *req, void *arg) 
{
	struct evbuffer *buf;
	const char *uri = evhttp_request_get_uri(req);
	printf("Received a request: %s\n", uri);

	buf = evbuffer_new();
	evbuffer_add_printf(buf, "Hello, World! Your request uri is: %s", uri);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}


int main()
{
	struct event_base *base;
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Failed to create event base\n");
		return 1;
	}

	http = evhttp_new(base);
	if (!http) {
		fprintf(stderr, "Failed to create evhttp\n");
		return 1;
	}

	evhttp_set_gencb(http, request_handler, NULL);

	handle = evhttp_bind_socket_with_handle(http, "127.0.0.1", 8080);
	if (!handle) {
		fprintf(stderr, "Failed to bind socket\n");
		return 1;
	}
	
	printf("Listening on 8080\n");
	event_base_dispatch(base);

	evhttp_free(http);
	event_base_free(base);

	return 0;
}