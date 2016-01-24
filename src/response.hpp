#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

namespace rele {

/*
   Class: response
   Class representing an HTTP response
*/
class response {

	private:

		/*
			Variable: header
			An std::map which stores the headers for the response.
		*/
		std::map <std::string, std::string> header;

		/*
			Variable: protocol
			The protocol in use.
		*/
		std::string protocol;

		/*
			Variable: status
			The HTTP status of the response.
		*/
		std::string status;

	public:

		/*
			Constructor: response
			Constructor to initialize the response.
		*/
		response();

		/*
			Function: reset
			Reset the response for reuse.
		*/
		void reset();

		/*
			Function: set_header
			Set the given header

			Parameters:
				header - The header to set
				value - The value to set it to
		*/
		void set_header(const std::string &start, const std::string &value);

		/*
			Function: set_status
			Set the response status

			Parameters:
				code - The HTTP code
		*/
		void set_status(int code);

		/*
			Function: get_status
			Set the response status

			Returns:
				HTTP status
		*/
		const std::string &get_status() const;

		/*
			Function: get_header_string
			Get the given header string

			Parameters:
				header - The header
				default_value - The default value

			Returns:
				The header if set, otherwise the given default value
		*/
		const std::string &get_header_string(const std::string &string, const std::string &default_value) const;

		/*
			Function: get_header_int
			Get the given header integer

			Parameters:
				header - The header
				default_value - The default value

			Returns:
				The header if set, otherwise the given default value
		*/
		int get_header_int(const std::string &string, int default_value) const;

		/*

		*/
		operator std::string();

};

}

#endif
