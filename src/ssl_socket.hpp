#ifndef SSL_SOCKET_HPP
#define SSL_SOCKET_HPP

#include "socket.hpp"

namespace rele {

class ssl_socket: public net_socket {

    protected:

        /*
            Variable: s_socket
            C struct socket descriptor.
        */
        struct s_ssl;

        /*
            Variable: _socket
            The C socket
        */
        s_ssl *_ssl;

        /*
            Function: ssl_init
            Initialize SSL for this socket.

            Returns:
                True if success or false if not
        */
        bool ssl_client_init();
        bool ssl_server_init();

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
        virtual ~ssl_socket();

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
        net_socket *accept();

        /*
            Function: listen
            Prepare the socket for <accept>ing incoming connections.

            Parameters:
                addr - Address to bind the listener on
                port - Port number to listen on
        */
        bool connect(const std::string &hostname, int port);

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
};

}

#endif
