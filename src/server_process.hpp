#ifndef SERVER_HPP
#define SERVER_HPP

#include "socket.hpp"
#include "server_thread.hpp"

#include <vector>

namespace rele {

class server_thread;

/*
   Class: server_process
   A server_process is responsible for listening to incoming connections and
   delegating a <server_thread> to handle the request.
   a <route>.
*/
class server_process {


	private:

		pthread_mutex_t m_thread_pool;


		/*
			Variable: server_socket
			The socket which accepts incoming connections.
		*/
		rele::net_socket *server_socket;

		/*
			Variable: listen_size
			Maximum number of incoming connections in queue.
		*/
		int listen_size;

		/*
			Variable: thread_pool_size
			Amount of threads reserved for incoming requests.
		*/
		int thread_pool_size;

		/*
			Variable: threads_passive
			An std::vector of idle <server_thread>s.
		*/
		std::vector <server_thread*> threads_passive;

		/*
			Variable: threads_active
			An std::vector of active <server_thread>s processing a <request>.
		*/
		std::vector <server_thread*> threads_active;

		/*
			Function: create_thread
			Create a <server_thread>.

			Returns:
				A pointer to <server_thread>
		*/
		server_thread *create_thread();

		/*
			Function: wait_pause.
			Not in use
		*/
		void wait_pause();

	public:

		/*
			Constructor: server_prorcess
			Create and intialize the <server_process>.

			Parameters:
				port - Port to listen on to.
		*/
		server_process(int port, bool ssl);

		/*
			Desctructor: ~server_process
			Free up memory and deinitialize. Close sockets and threads.
		*/
		~server_process();

		/*
			Function: start
			Start listening, wait for incoming connections and pass them
			on for further processing.
		*/
		void start();

		/*
			Function: claim_thread
			Claim a thread from the pool.

			Returns:
				A pointer to a <server_thread> or NULL if there are no free
				threads in the pool.
		*/
		server_thread *claim_thread();

		/*
			Function: unclaim_thread
			Return a <server_thread> to the pool.

			Parameters:
				thread - The thread to return to the pool.
		*/
		void unclaim_thread(server_thread *thread);

};

}

#endif
