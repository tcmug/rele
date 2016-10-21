
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <poll.h>

#include "ssl_socket.hpp"


using namespace rele;


#include <assert.h>


struct net_socket::s_socket {
  int socket;
  int port;
  socklen_t clilen;
  struct sockaddr_in address;
};


#define RSA_SERVER_CERT     "server.crt"
#define RSA_SERVER_KEY      "server.key"
#define RSA_SERVER_CA_CERT  "server_ca.crt"
#define RSA_SERVER_CA_PATH  "sys$common:[syshlp.examples.ssl]"

const bool verify_client = 0;

#define ON (1)
#define OFF (0)


ssl_socket::ssl_socket() {
  net_socket::net_socket();

  method = NULL;
  ssl = NULL;
  ssl_ctx = NULL;

}



ssl_socket::~ssl_socket() {

  if (ssl) {
    SSL_free(ssl);
  }

  if (ssl_ctx) {
    SSL_CTX_free(ssl_ctx);
  }

}


bool ssl_socket::connect(const std::string &hostname, int port) {
  this->ssl_init();
  if (net_socket::connect(hostname, port)) {
    // set_tcp_nodelay(0);
    if (SSL_set_fd(ssl, _socket->socket) == 1) {
      if (SSL_connect(ssl) != 1) {
        return false;
      }
      return true;
    }
  }
  return false;
}



bool ssl_socket::ssl_init() {

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  /* ---------------------------------------------------------- *
   * initialize SSL library and register algorithms             *
   * ---------------------------------------------------------- */
  if (SSL_library_init() < 0)
    return false;

  /* ---------------------------------------------------------- *
   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
   * ---------------------------------------------------------- */
  method = SSLv23_client_method();

  /* ---------------------------------------------------------- *
   * Try to create a new SSL context                            *
   * ---------------------------------------------------------- */
  if ((ssl_ctx = SSL_CTX_new(method)) == NULL)
    return false;

  /* ---------------------------------------------------------- *
   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
   * ---------------------------------------------------------- */
  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);

  /* ---------------------------------------------------------- *
   * Create new SSL connection state object                     *
   * ---------------------------------------------------------- */
  ssl = SSL_new(ssl_ctx);

  return true;
}

int ssl_socket::write(const std::string &data) {
    return SSL_write(this->ssl, data.c_str(), data.size());
}


int ssl_socket::read(char *buffer, int len) {
    return SSL_read(this->ssl, buffer, len);
}
