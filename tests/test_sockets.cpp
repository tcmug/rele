
#include <iostream>
#include <string>

#include "../src/ssl_socket.hpp"


#define BSZ (1000)

int main(int argc, char *argv[]) {

    {
    rele::net_socket connection;
    char buffer[BSZ];
    std::string request = "GET http://google.com/ HTTP/1.0\r\n\r\n";

    if (connection.connect("google.com", 80)) {
        std::cout << "HTTP Connected." << std::endl;
        std::cout << "Write: <<<START>>>" << request << "<<<END>>>" << std::endl;
        connection.write(request);
        connection.read(buffer, BSZ);
        std::cout << "Read: <<<START>>>" << buffer << "<<<END>>>" << std::endl;
    }
    else {
        std::cout << "Failed." << std::endl;
    }
    }

    {
    rele::ssl_socket connection;
    char buffer[BSZ];
    std::string request = "GET http://google.com/ HTTP/1.0\r\n\r\n";

    if (connection.connect("google.com", 443)) {
        std::cout << "HTTPS Connected." << std::endl;
        std::cout << "Write: <<<START>>>" << request << "<<<END>>>" << std::endl;
        connection.write(request);
        connection.read(buffer, BSZ);
        std::cout << "Read: <<<START>>>" << buffer << "<<<END>>>" << std::endl;
    }
    else {
        std::cout << "Failed." << std::endl;
    }
    }



    std::cout << "SERVER TEST" << std::endl;
    {
    rele::ssl_socket server;
    char buffer[BSZ];
    std::string request = "GET http://google.com/ HTTP/1.0\r\n\r\n";
    try {
    std::cout << "Listen" << std::endl;
    server.listen("127.0.0.1", 18443);
    std::cout << "Server up" << std::endl;

    while (1) {
        rele::net_socket *client = server.accept();
        if (client->is_valid()) {
            std::cout << "Reading" << std::endl;
            try {
                client->read(buffer, BSZ);
                client->write("Hello world\r\n\r\n");
                std::cout << "Read: <<<START>>>" << buffer << "<<<END>>>" << std::endl;
            }
            catch (...) {
                std::cout << "blah" << std::endl;
            }
        }
        delete client;
    }

    }
    catch (...) {

        std::cout << "Exception" << std::endl;
    }

    }

}
