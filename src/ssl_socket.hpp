#ifndef SSL_SOCKET_HPP
#define SSL_SOCKET_HPP

#include "socket.hpp"

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace rele {

class ssl_socket: public net_socket {

    private:

        SSL *ssl;
        static SSL_CTX *ssl_ctx;

    public:

        ssl_socket();

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

        bool connect(const std::string &hostname, int port);
        void ssl_init();
};

}

#endif
