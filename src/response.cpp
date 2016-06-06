

#include <iostream>

#include "response.hpp"

using namespace rele;

response::response() {
	this->protocol = "HTTP/1.1";
	this->status = "200";
}



void response::reset() {
	this->header.clear();
	this->status.clear();
}


// Set query string
//
//
void response::set_header(const std::string &start, const std::string &value) {
	this->header[start] = value;
}

void response::set_status(int code) {
	this->status = std::to_string(code);
}


const std::string &response::get_status() const {
	return this->status;
}

const std::string &response::get_header_string(const std::string &string, const std::string &default_value) const {
	auto i = this->header.find(string);
	if (i != this->header.end()) {
		return i->second;
	}
	return default_value;
}



int response::get_header_int(const std::string &string, int default_value) const {
	auto i = this->header.find(string);
	if (i != this->header.end()) {
		try {
			return std::stoi(i->second);
		} catch (std::exception &e) {
		}
	}
	return default_value;
}



response::operator std::string() {

	std::string headers_string = this->protocol + " " + this->status + "\r\n";

	auto i = this->header.begin();
	while (i != this->header.end()) {
		headers_string += i->first + ": " + i->second + "\r\n";
		i++;
	}

	return headers_string + "\r\n";
}



