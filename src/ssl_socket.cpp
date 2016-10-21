
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


SSL_CTX *ssl_socket::ssl_ctx = 0;

#define RSA_SERVER_CERT     "server.crt"
#define RSA_SERVER_KEY      "server.key"
#define RSA_SERVER_CA_CERT  "server_ca.crt"
#define RSA_SERVER_CA_PATH  "sys$common:[syshlp.examples.ssl]"

const bool verify_client = 0;

#define ON (1)
#define OFF (0)


ssl_socket::ssl_socket() {
  net_socket::net_socket();
  // if (ssl_socket::ssl_ctx == 0) {
  //   // Initialize.
  //   const SSL_METHOD *meth = SSLv23_method();
  //   ssl_socket::ssl_ctx = SSL_CTX_new(meth);

  //   /* Load the server certificate into the SSL_CTX structure */
  //   if (SSL_CTX_use_certificate_file(ssl_socket::ssl_ctx, RSA_SERVER_CERT, SSL_FILETYPE_PEM) <= 0) {
  //     ERR_print_errors_fp(stderr);
  //     exit(1);
  //   }

  //   /* Load the private-key corresponding to the server certificate */
  //   if (SSL_CTX_use_PrivateKey_file(ssl_socket::ssl_ctx, RSA_SERVER_KEY, SSL_FILETYPE_PEM) <= 0) {
  //     ERR_print_errors_fp(stderr);
  //     exit(1);
  //   }

  //   /* Check if the server certificate and private-key matches */
  //   if (!SSL_CTX_check_private_key(ssl_socket::ssl_ctx)) {
  //     fprintf(stderr,"Private key does not match the certificate public key\n");
  //     exit(1);
  //   }

  //   if(verify_client == ON) {
  //     /* Load the RSA CA certificate into the SSL_CTX structure */
  //     if (!SSL_CTX_load_verify_locations(ssl_socket::ssl_ctx, RSA_SERVER_CA_CERT, NULL)) {
  //       ERR_print_errors_fp(stderr);
  //       exit(1);
  //     }
  //   }
  //   /* Set to require peer (client) certificate verification */
  //   SSL_CTX_set_verify(ssl_socket::ssl_ctx, SSL_VERIFY_PEER,NULL);
  //   /* Set the verification depth to 1 */
  //   SSL_CTX_set_verify_depth(ssl_socket::ssl_ctx,1);
  // }

  // this->ssl = SSL_new(ssl_socket::ssl_ctx);

}


BIO              *certbio = NULL;
BIO               *outbio = NULL;
X509                *cert = NULL;
X509_NAME       *certname = NULL;
const SSL_METHOD *method;
int server = 0;
int ret, i;


bool ssl_socket::connect(const std::string &hostname, int port) {
  this->ssl_init();
  if (net_socket::connect(hostname, port)) {
    if (SSL_set_fd(ssl, _socket->socket) == 1) {

      if ( SSL_connect(ssl) != 1 )
          BIO_printf(outbio, "Error: Could not build a SSL session");
        else
          BIO_printf(outbio, "Successfully enabled SSL/TLS session");


      return true;
    }
  }
  return false;
}



void ssl_socket::ssl_init() {

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();


  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * initialize SSL library and register algorithms             *
   * ---------------------------------------------------------- */
  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  /* ---------------------------------------------------------- *
   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
   * ---------------------------------------------------------- */
  method = SSLv23_client_method();

  /* ---------------------------------------------------------- *
   * Try to create a new SSL context                            *
   * ---------------------------------------------------------- */
  if ( (ssl_ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  /* ---------------------------------------------------------- *
   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
   * ---------------------------------------------------------- */
  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);

  /* ---------------------------------------------------------- *
   * Create new SSL connection state object                     *
   * ---------------------------------------------------------- */
  ssl = SSL_new(ssl_ctx);


}

int ssl_socket::write(const std::string &data) {
    return SSL_write(this->ssl, data.c_str(), data.size());
}


int ssl_socket::read(char *buffer, int len) {
    return SSL_read(this->ssl, buffer, len);
}
