
#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include "server_process.hpp"
#include "server_thread.hpp"
#include "buffer.hpp"
#include "request.hpp"
#include "response.hpp"
#include "router.hpp"
#include "dynamic_source.hpp"

using namespace rele;


std::mutex log_mutex;

/**
 * Create a new server thread object
 */
server_thread::server_thread(): thread() {

	this->has_connection = false;
	this->alive = 1;
	this->buffer = new byte_buffer(1024); // 1MB

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


// Helper function for reading data to the buffer from the socket.
int buffer_read_socket(std::vector <char> &buffer, rele::net_socket &socket) {
	int size = buffer.size();
	buffer.resize(buffer.capacity());
	int read = socket.read(&buffer[0], buffer.capacity());
	if (read >= 0) {
		buffer.resize(size + read);
	}
	return read;
	//std::cout << read << " = " <<  buffer.size() << "/" << buffer.capacity() << std::endl;
}


// Helper function for shifting the buffer by the given amount.
int buffer_shift(std::vector <char> &buffer, int amount) {
	buffer.erase(buffer.begin(), buffer.begin() + amount);
	//std::cout << -amount << " = " <<  buffer.size() << "/" << buffer.capacity() << std::endl;
}

// Helper function for reading a single ASCII line from the buffer.
std::string buffer_read_line(std::vector <char> &buffer, rele::net_socket &client_socket) {

	// Will return or throw an exception
	while (true) {
		// Scan the end of the line as well as invalid characters.
		for (std::vector <char>::iterator i = buffer.begin(); i < buffer.end(); i++) {
			if (*i == '\n') {
				std::string line(buffer.begin(), i - 1);
				buffer_shift(buffer, (i - buffer.begin()) + 1);
				return line;
			}
			else if (*i <= 0) {
				throw "400 Bad Request";
			}
		}
		if (buffer_read_socket(buffer, client_socket) <= 0) {
			throw "400 Bad Request";
		}
	}
}


void server_thread::client_handler() {

	request req;

	try {

		std::vector <char> buffer;
		buffer.reserve(1024 * 1024);
		std::string line = buffer_read_line(buffer, client_socket);

		std::cout << line << std::endl;

		// Read request headers.

		std::istringstream iss(line);
		std::vector <std::string> split_line {
			std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{}
		};

		if (split_line.size() != 3) {
			throw "400 Bad Request";
		}

		req.set_method(split_line[0]);
		req.set_query(split_line[1]);
		req.set_protocol(split_line[2]);

		// Read headers.

		while (true) {

			std::string line = buffer_read_line(buffer, client_socket);

			// Headers part ends with an empty line.

			if (line.length() == 0) {
				break;
			}

			// Split the string in to key and value.

			std::string key;
			std::string value;
			int colon = line.find(":");
			key = line.substr(0, colon);
			if (colon != std::string::npos) {
			  value = line.substr(colon + 2);
			}

			req.set_header(key, value);
		}

		// Read request data.

		int content_length = req.get_header_int("Content-Length", 0);

		while (content_length > 0) {
			int size = buffer.size();
			content_length -= size;
			//std::cout << "remaining: " << content_length << std::endl;
			buffer_shift(buffer, size);
			if (content_length > 0 && buffer.empty()) {
				if (buffer_read_socket(buffer, client_socket) == 0) {
					throw "400 Bad Request";
				}
			}
		}

		this->respond(&req);

		buffer_shift(buffer, buffer.size());
		req.reset();

	}
	catch (const char *error) {
		char str[2000];
		sprintf(str, "HTTP/1.1 %s\r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", error, strlen(error), error);
		this->client_socket << str;
	}

	this->client_socket.close();

}




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
	std::cout << req->get_protocol() << " " << req->get_method() << " " << req->get_query() << " | " << req->get_parameters() << " ::: " << res.get_status() << std::endl;
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


