#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <utility>
#include <vector>
#include <string>
#include <regex>

#include "response.hpp"
#include "request.hpp"

namespace rele {

class dynamic_source;

/*
	Class: route
	A route handles an incoming <request> and prepares a <response>.
*/
class route {

	public:

		dynamic_source *source;

	public:

		/*
			Constructor: route
			Initialize variables.
		*/
		route();

		/*
			Destructor: ~route
			Free up all reserved memory and close all files opened.
		*/
		virtual ~route();

		/*
			Function: get_pattern
			Get the regular expression pattern for this route.

			Returns:
				A regular expression in C string format.
		*/
		virtual const char *get_pattern();

		/*
			Function: process
			Process a request and prepare a response and return a pointer to
			the data.

			Parameters:
				r - A const pointer to the request object
				res - A pointer to the response object to fill

			Returns:
				A pointer to a C string containing the response data.
		*/
		virtual const char *process(const request *r, response *res);


		void lock_source();

		void unlock_source();

};

}

#endif
