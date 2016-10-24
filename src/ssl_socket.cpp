
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <poll.h>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <iostream>

#include "ssl_socket.hpp"


using namespace rele;


#include <assert.h>


struct net_socket::s_socket {
  int socket;
  int port;
  socklen_t clilen;
  struct hostent *host;
  struct sockaddr_in address;
};

struct ssl_socket::s_ssl {
  const SSL_METHOD *method;
  SSL *ssl;
  SSL_CTX *ctx;
};



ssl_socket::ssl_socket(): net_socket() {
  _ssl = new s_ssl;
  _ssl->method = NULL;
  _ssl->ssl    = NULL;
  _ssl->ctx    = NULL;
}



ssl_socket::~ssl_socket() {

  assert(_ssl != 0);

  if (_ssl->ssl) {
    SSL_shutdown(_ssl->ssl);
    SSL_free(_ssl->ssl);
  }

  if (_ssl->ctx) {
    SSL_CTX_free(_ssl->ctx);
  }

  std::cout << "Destroyed" << std::endl;

  delete _ssl;

}



bool ssl_socket::connect(const std::string &hostname, int port) {
  this->ssl_client_init();
  if (net_socket::connect(hostname, port)) {
    if (SSL_set_fd(_ssl->ssl, _socket->socket) == 1) {
      if (SSL_connect(_ssl->ssl) == 1) {
        return true;
      }
    }
  }
  return false;
}


void ssl_socket::listen(const std::string &addr, int port) {
  net_socket::listen(addr, port);
  if (!ssl_server_init()) {
    std::cout << "init failed" << std::endl;
  }
}


net_socket *ssl_socket::accept() {

  ssl_socket *new_sock = new ssl_socket();

  socklen_t len;

  // Accept the new connection
  new_sock->_socket->socket = ::accept(
    this->_socket->socket,
    (sockaddr*)&new_sock->_socket->address,
    &len
  );

  int r = 666;
  new_sock->_ssl->ssl = SSL_new(_ssl->ctx);
  if ((r = SSL_set_fd(new_sock->_ssl->ssl, new_sock->_socket->socket)) == 1) {
    if ((r = SSL_accept(new_sock->_ssl->ssl)) == 1) {
      return new_sock;
    }
  }

  int err_SSL_get_error = SSL_get_error(_ssl->ssl, r);
  int err_ERR_get_error = ERR_get_error();

  std::cout << "[DEBUG] SSL_accept() : Failed with return "
            << r << std::endl;
  std::cout << "[DEBUG]     SSL_get_error() returned : "
            << err_SSL_get_error << std::endl;
  std::cout << "[DEBUG]     Error string : "
            << ERR_error_string( err_SSL_get_error, NULL )
            << std::endl;
  std::cout << "[DEBUG]     ERR_get_error() returned : "
            << err_ERR_get_error << std::endl;
  std::cout << "+--------------------------------------------------+"
            << std::endl;

  return new_sock;
}



bool ssl_socket::ssl_server_init() {

  assert(_ssl->method == NULL);

  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  _ssl->method = TLS_server_method();
  _ssl->ctx = SSL_CTX_new(_ssl->method);

  if (!_ssl->ctx) {
    return false;
  }

  if (SSL_CTX_use_certificate_file(_ssl->ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
//    ERR_print_errors_fp(stderr);
    return false;
  }
  if (SSL_CTX_use_PrivateKey_file(_ssl->ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
//    ERR_print_errors_fp(stderr);
    return false;
  }

  if (!SSL_CTX_check_private_key(_ssl->ctx)) {
    return false; //fprintf(stderr,"Private key does not match the certificate public key\n");
   // exit(5);
  }

  return true;
}



bool ssl_socket::ssl_client_init() {

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  //ERR_load_BIO_strings();
  //ERR_load_crypto_strings();

  if (SSL_library_init() < 0)
    return false;

  _ssl->method = SSLv23_client_method();

  if ((_ssl->ctx = SSL_CTX_new(_ssl->method)) == NULL)
    return false;

  SSL_CTX_set_options(_ssl->ctx, SSL_OP_NO_SSLv2);
  _ssl->ssl = SSL_new(_ssl->ctx);

  return true;

}


int ssl_socket::write(const std::string &data) {
  assert(_ssl != 0);
  return SSL_write(_ssl->ssl, data.c_str(), data.size());
}


int ssl_socket::read(char *buffer, int len) {
  assert(_ssl != 0);
  return SSL_read(_ssl->ssl, buffer, len);
}
