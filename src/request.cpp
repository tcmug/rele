
#include <iostream>
#include <iomanip>

#include "request.hpp"

using namespace rele;



void request::reset() {
	this->header.clear();
	this->query.clear();
	this->parameters.clear();
	this->method.clear();
	this->protocol.clear();
}



void request::set_query(const std::string &str) {
	this->query = str;
}


const std::string &request::get_query() const {
	return this->query;
}



void request::set_parameters(const std::string &str) {
	this->parameters = str;
}


const std::string &request::get_parameters() const {
	return this->parameters;
}




void request::set_header(const std::string &header, const std::string &value) {
	this->header[header] = value;
}




void request::set_protocol(const std::string &proto) {
	this->protocol = proto;
}



void request::set_method(const std::string &meth) {
	this->method = meth;
}



const std::string &request::get_protocol() const {
	return this->protocol;
}


const std::string &request::get_method() const {
	return this->method;
}


const std::string &request::get_header_string(const std::string &string, const std::string &default_value) const {
	auto i = this->header.find(string);
	if (i != this->header.end()) {
		return i->second;
	}
	return default_value;
}



int request::get_header_int(const std::string &string, int default_value) const {
	auto i = this->header.find(string);
	if (i != this->header.end()) {
		try {
			return std::stoi(i->second);
		} catch (std::exception &e) {
		}
	}
	return default_value;
}



request::operator std::string() {

	std::string headers_string = this->method + " " + this->query + " " + this->protocol + "\r\n";

	auto i = this->header.begin();
	while (i != this->header.end()) {
		headers_string += i->first + ": " + i->second + "\r\n";
		i++;
	}

	return headers_string + "\r\n";
}

