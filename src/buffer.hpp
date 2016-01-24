#ifndef LSD_BUFFER_H
#define LSD_BUFFER_H

#include "socket.hpp"

namespace rele {

/*
	Class: byte_buffer
	A simple buffer for storing data
*/
class byte_buffer {

	public:

		/*
			Constructor: byte_buffer
			Initialize the buffer

			Parameters:
				size - Size of the bytes in buffer
		*/
		byte_buffer(int size = 1000);

		/*
			Destructor: ~byte_buffer
			Delete and free the memory reserved by the buffer
		*/
		~byte_buffer();

		/*
			Function: read_socket
			Read data from the socket in to the buffer

			Parameters:
				sock - The socket to read from

			Returns:
				True if data was read, false if there was no data to read.
		*/
		bool read_socket(net_socket &sock);

		/*
			Function: shift
			Pop data from the beginning of the buffer

			Parameters:
				bytes - Bytes to shift
		*/
		int shift(int bytes);

		/*
			Variable: data
			Buffer data
		*/
		char *data;

		/*
			Variable: last_read
			Amount of bytes read during the last <read_socket> call
		*/
		int last_read;

		/*
			Variable: size
			Total size of the buffer
		*/
		int size;

		/*
			Variable: used
			Amount of bytes used currently
		*/
		int used;
};

}

#endif
