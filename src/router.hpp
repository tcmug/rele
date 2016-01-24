#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <utility>
#include <vector>
#include <string>
#include <regex>

#include "response.hpp"
#include "request.hpp"
#include "route.hpp"


namespace rele {

/*
   Class: router
   A router maps incoming <request> and the accompanying <response> to
   a <route>.
*/
class router {

	private:

		std::vector <std::pair <std::regex, class route*> > routes;

	public:

		/*
			Function: add_route
			Add a <route> to the router

			Parameter:
				r - The <route> to add
		*/
		void add_route(route *r);

		/*
			Function: route_request
			Route the <request> and <response> to the first route which matches the
			request.

			Parameters:
				req - A const pointer to the <request> object.
				res - A pointer to the <response> object.
		*/
		const char *route_request(const request *req, response *res);

		/*
			Function: get_route_by_pattern
			Get a <route> which matches the pattern.

			Parameters:
				pattern - A C string which is a regular expression.

			Returns:
				A pointer to the <route> or NULL if none is matched.
		*/
		route *get_route_by_pattern(const char *pattern);


		/*
			Function: remove_route_by_pattern
			Remove a <route> which matches the given pattern.

			Parameters:
				pattern - A C string which is a regular expression.

			Returns:
				A pointer to the <route> or NULL if none is matched.
		*/
		route *remove_route_by_pattern(const char *pattern);

		/*
			Function: load_dynamic_routes
		*/
		void load_dynamic_routes();

};

}

#endif
