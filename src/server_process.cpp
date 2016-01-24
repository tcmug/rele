

#include "server_process.hpp"

#include <iostream>
#include <string>
#include <map>

#include "logger.hpp"
#include "socket.hpp"
#include "server_thread.hpp"

using namespace rele;

server_process::server_process(int port) {

	this->listen_size = 20;

	this->server_socket.listen("", port);

	this->thread_pool_size = 100;
	this->threads_active.reserve(this->thread_pool_size);
	this->threads_passive.reserve(this->thread_pool_size);

	rele::logger::get_instance()->log("Starting server...");
}

server_process::~server_process() {

}



void http_quick_response(rele::net_socket &sock, const char *code) {
	char str[2000];
	sock.read(str, 2000);
	sprintf(str, "HTTP/1.1 %s\r\nContent-Length: 0;\r\n\r\n", code);
	sock << str;
}




pthread_mutex_t m_thread_pool;
int thread_pool_top;
server_thread **thread_pool;


void server_process::start() {

	pthread_mutex_init(&m_thread_pool, NULL);

	for (int n = 0; n < this->thread_pool_size; n++) {
		this->create_thread();
	}

	printf("Running...\n");

	while (1) {

		// Accept the new connection
		rele::net_socket new_socket = this->server_socket.accept();

		if (new_socket.is_valid()) {
			server_thread *thread = this->claim_thread();
			if (thread) {
				thread->process(new_socket);
			} else {
				// Pool is full and we ran out of wait time
				http_quick_response(new_socket, "408 Request Timeout");
				new_socket.close();
			}
		}
	}

}



server_thread *server_process::create_thread() {
	server_thread *thread = new server_thread();
	thread->parent_process = this;
	this->threads_passive.push_back(thread);
	return thread;
}


server_thread *server_process::claim_thread() {
	pthread_mutex_lock(&m_thread_pool);
	server_thread *thread = 0;
	if (this->threads_passive.size() > 0) {
		thread = this->threads_passive.back();
		this->threads_passive.pop_back();
		this->threads_active.push_back(thread);
	}
	pthread_mutex_unlock(&m_thread_pool);
	//printf("%lu/%lu\n", this->threads_active.size(),  this->threads_passive.size());
	return thread;
}


void server_process::unclaim_thread(server_thread *thread) {
	pthread_mutex_lock(&m_thread_pool);
	this->threads_passive.push_back(thread);
	this->threads_active.erase(std::remove(this->threads_active.begin(), this->threads_active.end(), thread), this->threads_active.end());
	pthread_mutex_unlock(&m_thread_pool);
}
