

#include "server_process.hpp"

#include <iostream>
#include <string>
#include <map>

#include "logger.hpp"
#include "socket.hpp"
#include "ssl_socket.hpp"
#include "server_thread.hpp"
#include "ini.hpp"

using namespace rele;

server_process::server_process(int port, bool ssl) {

	this->listen_size = 20;

	if (ssl) {
		this->server_socket = new ssl_socket();
		this->server_socket->listen("", port);
	}
	else {
		this->server_socket = new net_socket();
		this->server_socket->listen("", port);
	}

	this->thread_pool_size = 100;
	this->threads_active.reserve(this->thread_pool_size);
	this->threads_passive.reserve(this->thread_pool_size);

	rele::logger::get_instance()->info("Starting server...");
}




server_process::server_process(const ini &ini) {

	listen_size = ini.get_int("listen_size", 20);

	if (ini.get_int("ssl", 0) == 1) {
		server_socket = new ssl_socket();
		server_socket->listen("", ini.get_int("port", 8080));
		static_cast<ssl_socket*>(server_socket)->set_ciphers(ini.get_string("ciphers", ""));
	}
	else {
		server_socket = new net_socket();
		server_socket->listen("", ini.get_int("port", 8080));
	}

	thread_pool_size = ini.get_int("thread_pool", 40);
	threads_active.reserve(thread_pool_size);
	threads_passive.reserve(thread_pool_size);

	rele::logger::get_instance()->info("Starting server...");
}


server_process::~server_process() {
	delete this->server_socket;
}



void http_quick_response(rele::net_socket *sock, const char *code) {
	char str[2000];
	sock->read(str, 2000);
	sprintf(str, "HTTP/1.1 %s\r\nContent-Length: 0;\r\n\r\n", code);
	*sock << str;
}







void server_process::start() {

	pthread_mutex_init(&m_thread_pool, NULL);

	for (int n = 0; n < this->thread_pool_size; n++) {
		this->create_thread();
	}

	printf("Running...\n");

	while (1) {

		// Accept the new connection
		rele::net_socket *new_socket = this->server_socket->accept();

		if (new_socket->is_valid()) {
			server_thread *thread = this->claim_thread();
			if (thread) {
				thread->process(new_socket);
			} else {
				// Pool is full and we ran out of wait time
				http_quick_response(new_socket, "408 Request Timeout");
				delete new_socket;
			}
		} else {
			std::cout << "Bad socks " << this->threads_active.size() << std::endl;
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
