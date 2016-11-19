
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <poll.h>
#include <thread>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#include <iostream>

#include "ssl_socket.hpp"


using namespace rele;

bool ssl_socket::initialized = false;


// https://linux.die.net/man/3/crypto_set_locking_callback
// http://www.ecoscentric.com/ecospro/doc/html/ref/openssl-ecos-threads.html


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



#include <openssl/opensslconf.h>
#ifndef OPENSSL_THREADS
#error Need OpenSSL/LibreSSL with thread suppoert
#endif


#include <mutex>
#include <vector>
std::vector <std::mutex*> ssl_mutex;

extern "C" {

typedef unsigned long (*t_thread_id_callback)(void);
typedef void (*t_lock_callback)(int mode, int n, const char *file, int line);
typedef void (*t_threadid_callback)(CRYPTO_THREADID *);

static void ssl_callback_lock(int mode, int n, const char *file, int line) {
  assert(n >= 0 && n <= ssl_mutex.size());
  if (mode & CRYPTO_LOCK) {
    ssl_mutex[n]->lock();
  } else {
    ssl_mutex[n]->unlock();
  }
}

static void ssl_threadid_callback(CRYPTO_THREADID *id) {
  CRYPTO_THREADID_set_numeric(id, (std::hash<std::thread::id>()(std::this_thread::get_id())));
}

static unsigned long ssl_callback_thread_id(void) {
  return (std::hash<std::thread::id>()(std::this_thread::get_id()));
}
}



ssl_socket::ssl_socket(): net_socket() {
  _ssl = new s_ssl;
  _ssl->method = NULL;
  _ssl->ssl    = NULL;
  _ssl->ctx    = NULL;
  assert(_socket->socket == -1);
}



ssl_socket::~ssl_socket() {
  close();
  assert(_ssl != 0);
  delete _ssl;
}



bool ssl_socket::connect(const std::string &hostname, int port) {
  ssl_client_init();
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
  if (!ssl_server_init()) {
    std::cout << "init failed" << std::endl;
  }
  net_socket::listen(addr, port);
}


net_socket *ssl_socket::accept() {

  ssl_socket *new_sock = new ssl_socket();

  socklen_t len;

  // Accept the new connection
  new_sock->_socket->socket = ::accept(
    _socket->socket,
    (sockaddr*)&new_sock->_socket->address,
    &len
  );

  // If we cannot establish an SSL connection, simply close the socket.
  new_sock->_ssl->ssl = SSL_new(_ssl->ctx);
  int ret = SSL_set_fd(new_sock->_ssl->ssl, new_sock->_socket->socket);
  if (ret != 1) {
    ERR_print_errors_fp(stderr);
    std::cout << "SSL_set_fd failed. " << ret << std::endl;
    goto fail;
  }
  ret = SSL_accept(new_sock->_ssl->ssl);
  if (ret != 1) {
    ERR_print_errors_fp(stderr);
    std::cout << "SSL_accept failed. " << ret << std::endl;
    goto fail;
  }

  return new_sock;

  fail:
  new_sock->close();
}



bool ssl_socket::ssl_server_init() {

  assert(_ssl->method == NULL);

  if (!ssl_socket::initialized) {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    for (int i = 0; i < CRYPTO_num_locks() + 1; i++) {
      ssl_mutex.push_back(new std::mutex());
    }

    //CRYPTO_THREADID_set_callback((t_threadid_callback)&ssl_threadid_callback);
    CRYPTO_set_id_callback((t_thread_id_callback)&ssl_callback_thread_id);
    CRYPTO_set_locking_callback((t_lock_callback)&ssl_callback_lock);

    ssl_socket::initialized = true;
  }

//  _ssl->method = TLS_server_method();
  _ssl->method = SSLv23_server_method();
  _ssl->ctx = SSL_CTX_new(_ssl->method);

  if (!_ssl->ctx) {
    return false;
  }

  if (ciphers.length() > 0) {
    if (SSL_CTX_set_cipher_list(_ssl->ctx, ciphers.c_str()) <= 0) {
      ERR_print_errors_fp(stderr);
      return false;
    } else {
      std::cout << "Ciphers: " << ciphers << std::endl;
    }
  }
  // if (SSL_CTX_use_certificate_chain_file(_ssl->ctx, "server.pem") <= 0) {
  //   std::cout << "le fuu 1" << std::endl;
  //   ERR_print_errors_fp(stderr);
  //   return false;
  // }

  if (SSL_CTX_use_certificate_file(_ssl->ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return false;
  }

  if (SSL_CTX_use_PrivateKey_file(_ssl->ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return false;
  }

  if (!SSL_CTX_check_private_key(_ssl->ctx)) {
    return false; //fprintf(stderr,"Private key does not match the certificate public key\n");
   // exit(5);
  }

  return true;
}



bool ssl_socket::ssl_client_init() {

  if (!ssl_socket::initialized) {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ssl_socket::initialized = true;
  }

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


void ssl_socket::close() {
  if (_ssl->ssl) {
    SSL_shutdown(_ssl->ssl);
    SSL_shutdown(_ssl->ssl);
    SSL_free(_ssl->ssl);
    _ssl->ssl = 0;
    std::cout << "Freed SSL" << std::endl;
  }
  if (_ssl->ctx) {
    SSL_CTX_free(_ssl->ctx);
    _ssl->ctx = 0;
    std::cout << "Freed CTX" << std::endl;
  }
  if (_ssl->method) {
    // Free / Release ?
    _ssl->method = 0;
    std::cout << "Freed METHOD" << std::endl;
  }
  net_socket::close();
}



void ssl_socket::set_ciphers(const std::string &cip) {
  ciphers = cip;
}

std::string ssl_socket::get_ciphers() const {
  return ciphers;
}
