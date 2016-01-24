

#include "logger.hpp"

#include <iostream>

using namespace rele;


logger *logger::main_logger = NULL;


logger::logger() {
	this->log("Logging to stdout/stderr	");
}



logger *logger::get_instance() {
	if (logger::main_logger == NULL) {
		logger::main_logger = new logger();
	}
	return logger::main_logger;
}


void logger::log(const std::string &ln) {
	std::lock_guard<std::mutex> lock(mtx);
	std::cout << ln << std::endl;
}



void logger::log_error(const std::string &ln) {
	std::lock_guard<std::mutex> lock(mtx);
	std::cerr << "!! ERROR: " << ln << std::endl;
}
