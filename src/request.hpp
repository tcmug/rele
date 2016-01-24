#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>

namespace rele {

/*
   Class: request
   Class representing an HTTP request
*/
class request {

	private:

		/*
			Variable: header
			std::map which stores all the header information.
		*/
		std::map <std::string, std::string> header;

		/*
			Variable: protocol
			The used protocol.
		*/
		std::string protocol;

		/*
			Variable: method
			The request method.
		*/

		std::string method;

		/*
			Variable: query
			The query string
		*/
		std::string query;

	public:

		/*
			Function: reset
			Reset the header for reuse.
		*/
		void reset();

		/*
			Function: set_query
			Set the query part for this request

			Parameters:
				str - The query string
		*/
		void set_query(const std::string &str);

		/*
			Function: get_query
			Get the query part for this request

			Returns:
				The query string
		*/
		const std::string &get_query() const;

		/*
			Function: set_header
			Set the given header

			Parameters:
				header - The header to set
				value - The value to set it to
		*/
		void set_header(const std::string &header, const std::string &value);

		/*
			Function: set_protocol
			Set the protocol

			Parameters:
				proto - The protocol
		*/
		void set_protocol(const std::string &proto);

		/*
			Function: set_method
			Set the request method

			Parameters:
				meth - The method
		*/
		void set_method(const std::string &meth);

		/*
			Function: get_protocol
			Get the protocol

			Returns:
				Protocol
		*/
		const std::string &get_protocol() const;

		/*
			Function: get_method
			Get the request method

			Returns:
				Method
		*/
		const std::string &get_method() const;

		/*
			Function: get_header_string
			Get the given header string

			Parameters:
				header - The header
				default_value - The default value

			Returns:
				The header if set, otherwise the given default value
		*/
		const std::string &get_header_string(const std::string &header, const std::string &default_value) const;

		/*
			Function: get_header_int
			Get the given header integer

			Parameters:
				header - The header
				default_value - The default value

			Returns:
				The header if set, otherwise the given default value
		*/
		int get_header_int(const std::string &header, int default_value) const;

};


}

#endif
