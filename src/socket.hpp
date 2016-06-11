#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>

namespace rele {

/*
	Class: net_socket
	A socket wrapper.
*/
class net_socket {

	protected:

		/*
			Variable: s_socket
			C struct socket descriptor.
		*/
		struct s_socket;

		/*
			Variable: _socket
			The C socket
		*/
		s_socket *_socket;

	public:

		/*
			Constructor: net_socket
			Initialize the socket.
		*/
		net_socket();

		/*
			Constructor: net_socket
			Copy constructor.

			Parameters:
				obj - the socket to copy
		*/
   		net_socket(const net_socket &obj);

		/*
			Constructor: net_socket
			Copy constructor.
		*/
		~net_socket();

		/*
			Function: is_valid
			Check if the socket is valid and working.

			Returns:
				True if everything is fine and dandy, false if not.
		*/
		bool is_valid();

		/*
			Function: listen
			Prepare the socket for <accept>ing incoming connections.

			Parameters:
				addr - Address to bind the listener on
				port - Port number to listen on
		*/
		void listen(const std::string &addr, int port);

		/*
			Function: accept
			Wait for and accept a connection.

			Returns:
				A connected <net_socket>
		*/
		net_socket accept();

		/*
			Function: connect
			Try to make a connection with the port in the given address.

			Parameters:
				addr - The address
				port - The port

			Returns:
				True if connection was made, false if not.
		*/
		bool connect(const std::string &addr, int port);

		/*
			Function: operator =
			Copy operator

			Parameters:
				other - The socket to copy
		*/
		net_socket & operator = (const net_socket &other);

		/*
			Function: operator <<
			Write a string to the socket

			Parameters:
				data - The data in std::string format
		*/
		net_socket & operator << (const std::string &data);

		/*
			Function: write
			Write a string to the socket

			Parameters:
				data - The data in std::string format

			Returns:
				The number of bytes written
		*/
		int write(const std::string &data);

		/*
			Function: read
			Read at most the given amount of bytes from the socket to the given
			bfufer.

			Parameters:
				buffer - The buffer where to read the data to.
				len - Maximum amount of data to read.

			Returns:
				The number of bytes read.
		*/
		int read(char *buffer, int len);

		/*
			Function: close
			Close the socket
		*/
		void close();

		/*
			Function: set_reuse_addr
			Set the socket addr reuse on or off.

			Parameters:
				val - 1 for on, 0 for off.
		*/
		void set_reuse_addr(int val);

		/*
			Function: set_tcp_nodelay
			Set the sockets nagle algorithm on or off.

			Parameters:
				val - 1 for on, 0 for off.
		*/
		void set_tcp_nodelay(int val);


};

}; // Namespace

#endif
