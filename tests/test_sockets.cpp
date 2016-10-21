
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

}
