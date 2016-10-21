#ifndef SSL_SOCKET_HPP
#define SSL_SOCKET_HPP

#include "socket.hpp"

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace rele {

class ssl_socket: public net_socket {

    private:

        const SSL_METHOD *method;
        SSL *ssl;
        SSL_CTX *ssl_ctx;


        /*
            Function: ssl_init
            Initialize SSL for this socket.

            Returns:
                True if success or false if not
        */
        bool ssl_init();

    public:


        /*
            Constructor: net_socket
            Initialize the socket.
        */
        ssl_socket();


        /*
            Destructor: ssl_socket
            Clean up and destroy the SSL context.
        */
        ~ssl_socket();

        /*
            Function: write
            Write a string to the socket

            Parameters:
                data - The data in std::string format

            Returns:
                The number of bytes written
        */
        virtual int write(const std::string &data);

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
        virtual int read(char *buffer, int len);

        /*
            Function: listen
            Prepare the socket for <accept>ing incoming connections.

            Parameters:
                addr - Address to bind the listener on
                port - Port number to listen on
        */
        bool connect(const std::string &hostname, int port);
};

}

#endif
