#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <mutex>

namespace rele {

class logger {

	private:

		static logger *main_logger;
		std::mutex mtx;

	public:

		/*
			Constructor: logger
			Create and initialize logger
		*/
		logger();


		/*
			Function: get_instance
			Get a singleton of logger

		Parameters:
			- Logger instance

		Returns:
			Pointer to logger instance
		*/
		static logger *get_instance();

		/*
		Function: info
			Description

		Parameters:
			&ln - The log message
		*/
		void info(const std::string &ln);

		/*
		Function: error
			Description

		Parameters:
			&ln - The log message
		*/
		void error(const std::string &ln);

};

}

#endif
