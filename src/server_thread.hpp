#ifndef SERVER_THREAD_H
#define SERVER_THREAD_H

#include "buffer.hpp"
#include "router.hpp"
#include "thread.hpp"
#include "socket.hpp"

namespace rele {

class server_process;


/*
Class: server_thread
A class which is responsible for relaying requests to <route>s with a <router>.
*/
class server_thread: rele::thread {

	friend class server_process;

	private:

		/*
			Variable: client_socket
			The socket this <server_thread> is serving.
		*/
		rele::net_socket *client_socket;

		/*
			Variable: has_connection
			True when this thread is connected, false if not.
		*/
		bool has_connection;

		/*
			Variable: alive
			??
		*/
		int alive;

		/*
			Variable: parent_process
			Pointer to the parent process object this thread was spawned from.
		*/
		server_process *parent_process;

		/*
			Variable: buffer
			Inbound data buffer.
		*/
		byte_buffer *buffer;

		/*
			Variable: m
			Mutex variable for processing data.
		*/
		std::mutex m;

		/*
			Variable: cv
			Condition variable for processing data.
		*/
		std::condition_variable cv;

		/*
			Function: unclaim
			Unclain the thread, returning it to the pool of the <parent_process>.
		*/
		void unclaim();

		/*
			Function: run
			Thread loop.

			Returns:
				Always null
		*/
		virtual void *run();

		/*
			Function: respond
			Respond to a <request>. This is called from the function
			<client_handler> when we've collected enough data about the request
			to be processed.

			Parameters:
				req - A pointer to the <request> object.
		*/
		void respond(request *req);

		char *read_request(char *at);
		char *scan_headers(char *at, std::map <std::string, std::string> &headers);

	public:

		/*
			Constructor: server_thread
			Initialize and set up the internals.
		*/
		server_thread();

		/*
			Destructor: ~server_thread
			Free up mem and close the socket.
		*/
		~server_thread();

		/*
			Function: process
			Start processing requests from the given socket.

			Parameters:
				new_socket - The socket to use.
		*/
		void process(rele::net_socket *new_socket);

		/*
			Function: client_handler
			Read and parse the HTTP request from the socket and process it.
		*/
		void client_handler();
};


void *_server_thread_process(void*);

}

#endif
