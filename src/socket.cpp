
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <poll.h>

#include "socket.hpp"


using namespace rele;


#include <assert.h>

// Signal struct
//
//
struct net_socket::s_socket {
	int socket;
	int port;
	socklen_t clilen;
	struct sockaddr_in address;
};


net_socket::net_socket() {

	this->_socket = new s_socket;
	this->_socket->socket = -1;
	memset((char *)&this->_socket->address, 0, sizeof(this->_socket->address));

}



net_socket::net_socket(const net_socket &other) {
	this->_socket = new s_socket;
	printf("copy const\n");
	memcpy(this->_socket, other._socket, sizeof(net_socket::s_socket));
}




net_socket &net_socket::operator = (const net_socket &other) {
	this->_socket = new s_socket;
	memcpy(this->_socket, other._socket, sizeof(net_socket::s_socket));
	return *this;
}



net_socket::~net_socket() {

	delete this->_socket;

}



void net_socket::listen(const std::string &hostname, int port) {

	this->_socket->port = port;
	//this->_socket->hostname = hostname;

	this->_socket->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket->socket < 0) {
		throw;
	}

	this->_socket->address.sin_family = AF_INET;
	this->_socket->address.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_socket->address.sin_port = htons(this->_socket->port);

	if (bind(this->_socket->socket, (struct sockaddr *) &this->_socket->address, sizeof(this->_socket->address)) < 0) {
		throw;
	}

	struct timeval tv;

	tv.tv_sec = 1;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	if (setsockopt(this->_socket->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0) {
		throw;
	}

	//this->set_reuse_addr(1);
	this->set_tcp_nodelay(1);

	::listen(this->_socket->socket, 100);
}



void net_socket::set_reuse_addr(int val) {
	if (setsockopt(this->_socket->socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
		throw;
	}
}

void net_socket::set_tcp_nodelay(int val) {
	if (setsockopt(this->_socket->socket, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1) {
		throw;
	}
}


net_socket net_socket::accept() {

	net_socket new_sock;

	socklen_t len;

	// Accept the new connection
	new_sock._socket->socket = ::accept(
		this->_socket->socket,
		(sockaddr*)&new_sock._socket->address,
		&len
	);

	// struct timeval tv;

	// tv.tv_sec = 1;  /* 30 Secs Timeout */
	// tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	// if (setsockopt(new_sock._socket->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0) {
	// 	throw;
	// }

	return new_sock;
}



bool net_socket::is_valid() {
	return this->_socket->socket != -1;
}



bool net_socket::connect(const std::string &hostname, int port) {

	this->_socket->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket->socket < 0) {
		return false;
	}

	this->_socket->port = port;
	//this->_socket->hostname = hostname;

	return true;
}


void net_socket::close() {
	//::shutdown(this->_socket->socket, SHUT_RDWR);
	::close(this->_socket->socket);
	this->_socket->socket = -1;
}


net_socket & net_socket::operator << (const std::string &data) {
	assert(this->_socket->socket > 0);
	::write(this->_socket->socket, data.c_str(), data.size());
	return *this;
}


int net_socket::write(const std::string &data) {
	return ::write(this->_socket->socket, data.c_str(), data.size());
}


int net_socket::read(char *buffer, int len) {
	return ::read(_socket->socket, buffer, len);
}


