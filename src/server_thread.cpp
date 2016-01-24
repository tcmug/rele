
#include <iostream>
#include <string>
#include <map>

#include "server_process.hpp"
#include "server_thread.hpp"
#include "buffer.hpp"
#include "request.hpp"
#include "response.hpp"
#include "router.hpp"
#include "dynamic_source.hpp"

using namespace rele;


// Constant: STATE_REQUEST
// Request scanner is scanning the request.
#define STATE_REQUEST	(0)

// Constant: STATE_HEADERS
// Request scanner is scanning the headers.
#define STATE_HEADERS	(1)

// Constant: STATE_DATA
// Request scanner is scanning the data.
#define STATE_DATA		(2)

// Constant: STATE_DELIVER
// Request scanner is ready to deliver data.
#define STATE_DELIVER	(3)


#define HASH_START (5381)
#define MAX_PATH_LENGTH (2048)


/**
 * Create a new server thread object
 */
server_thread::server_thread(): thread() {

	this->has_connection = false;
	this->alive = 1;
	this->buffer = new byte_buffer();


}

/**
 * Free the server thread
 */
server_thread::~server_thread() {
	delete this->buffer;
}



/**
 * Main process thread
 */
void *server_thread::run() {

	while (this->alive > 0) {

		// Wait for a connection (invoken by server_thread_process)
		{
		std::unique_lock<std::mutex> lck(this->m);
		this->cv.wait(lck, [this] () { return this->has_connection; });
		}

		// Run the handler
		this->client_handler();
		//this->socket.invalidate();

		// Restore has_connection flag
		{
		std::unique_lock<std::mutex> lck(this->m);
		this->has_connection = false;
		}

		// Free the thread back to the pool
		this->unclaim();
	}

	return NULL;
}

/**
 * Invoker for the thread process
 */
void server_thread::process(const rele::net_socket &new_socket) {
	// Notify waiting thread for precessing
	this->client_socket = new_socket;

	// Signal thread in server_thread::run()
	std::lock_guard<std::mutex> lck(this->m);
    this->has_connection = true;
    this->cv.notify_one();

}



router *get_router_instance() {

	static router *r = 0;

	if (!r) {
		r = new router();
		r->add_route(new route());
	}

	r->load_dynamic_routes();

	return r;

}



void server_thread::client_handler() {

	if (!this->client_socket.is_valid()) {
		//
	}

	// Generic scan variables
	char *start = 0;
	char *at    = this->buffer->data;

	// Header vars
	char *colon = 0;
	char *value = 0;

	// Request vars
	char *path = 0;

	int bytes_done = 0;
	int state = STATE_REQUEST;
	int connection_close = 1;
	int content_length = 0;
	unsigned long header_key_hash;

	request req;

	while (this->buffer->read_socket(this->client_socket)) {

		char *end = &this->buffer->data[this->buffer->used];
		bytes_done = 0;

		// Loop through received data
		while (state == STATE_DELIVER || at < end) {

			//assert(at >= this->buffer->data);

			switch (state) {

				case STATE_REQUEST:
					// Scan METHOD PATH AND PROTOCOL:
					// method path proto
					switch (*at) {

						case '\r':
							// Ignore \r
						break;

						case '\n':

							*(at-1) = 0; // Add line end

							req.set_protocol(start);

							bytes_done = (at + 1) - this->buffer->data;
							state = STATE_HEADERS;
							start = 0;
							colon = 0;
							value = 0;

							header_key_hash = HASH_START;

							if (at - path > 100) {
								char str[2000];
								const char *data = "414 Request-URI Too Long";
								sprintf(str, "HTTP/1.1 %s\r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", "414 Request-URI Too Long", strlen(data), data);
								this->client_socket << str;
							}

							req.set_query(path);
						break;

						case ' ':
							*at = 0;
							if (req.get_method().empty()) {
								// Grab method.
								path = at + 1;
								req.set_method(start);
								start = 0;
							} else {
								start = at+1;
							}
						break;

						default:
							if (start == 0) {
								start = at;
							}
						break;
					}
				break;

				case STATE_HEADERS:
					// Scan headers:
					// Header: Some value
					switch (*at) {

						case '\r':
							// Ignore \r
						break;

						case ':':
							// Store colon position
							if (!colon && !value) {
								colon = at;
							}
						break;

						case '\n':
							// We hit end of line
							if (start) {

								// Line was completed
								*(at-1) = 0; // Add string end at the end of the line

								// since line ending is "\r\n", -1 means at the position of \r
								bytes_done = (at + 1) - this->buffer->data;

								req.set_header(start, value);

								header_key_hash = HASH_START;

							} else {

								// Second consequetive \n

								// Connection keep alive
								std::string str = req.get_header_string( "Connection", "" );
								if ( str == "keep-alive" ) {
									connection_close = 0;
								}

								// Get content type
								str = req.get_header_string( "Content-Type", "text/html" );
								content_length = req.get_header_int( "Content-Length", 0 );

								// Second consequetive \n end of headers.
								if (content_length == 0) {
									// There is no content - skip to deliver content
									state = STATE_DELIVER;
								} else {
									// Theres some content - continue to read it
									state = STATE_DATA;
								}

							}

							// Start scanning another header line (unless state has changed)
							start = 0;
							colon = 0;
							value = 0;
						break;

						default:
							// Treat any other characters as data
							if (start == 0) {
								start = at;
							}
							if (colon) {
								if (*at != ' ') {
									*colon = 0; // Add line end at colon to separate it properly
									colon = 0;
									value = at;
								}
							}
							// colon is 0 and value is 0 when scanning key from headers
							else if (!value) {
								// Calculate hash for the header key
								int c = *at;
								header_key_hash = ((header_key_hash << 5) + header_key_hash) + c;

							}
						break;
					}
				break;

				case STATE_DATA: // CONTENT
					{
					// Calculate bytes done
					content_length -= end - at;
					// *at = 0;
					// std::cout << at << std::endl;
					at = end - 1;
					bytes_done = this->buffer->used;

					if (content_length == 0) {
						state = STATE_DELIVER;
					}
				}
				break;

				case STATE_DELIVER: {

					this->respond(&req);

					start = 0;
					colon = 0;
					value = 0;
					path = 0;

					state = STATE_REQUEST;

					// Clean the whole request
					bytes_done = this->buffer->used;
					at = this->buffer->data + bytes_done - 1;

					req.reset();

				}
				break;

			}

			at++;

		} // Loop through received data

		if (bytes_done > 0) {
			this->buffer->shift(bytes_done);
			at -= bytes_done;
			if (start) {
				start -= bytes_done;
			}
		}

		if (connection_close == 1) {
			break;
		}

	}

	this->client_socket.close();
}



std::mutex log_mutex;

// Respond to request
//
//
void server_thread::respond(request *req) {

	response res;

	// Get a router instance and route.

	router *r = get_router_instance();
	const char *contents = r->route_request(req, &res);

	if (!contents) {
		contents = "404 Not Found";
		res.set_header("Content-Type", "text/html");
		res.set_header("Content-Length", std::to_string(strlen(contents)));
		res.set_status(404);
	}

	// Log this request.
	log_mutex.lock();
	std::cout << req->get_protocol() << " " << req->get_method() << " " << req->get_query() << " ::: " << res.get_status() << std::endl;
	log_mutex.unlock();

	// Push out the reply.
	this->client_socket << res;
	this->client_socket << contents;

}


// Unclaim thread
//
//
void server_thread::unclaim() {
	this->parent_process->unclaim_thread(this);
}


