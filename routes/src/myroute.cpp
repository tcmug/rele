

#include <router.hpp>
#include <string>
#include <unistd.h>

using namespace rele;

/*
	Class: my_route
	This is an example route.
*/
class my_route: public route {

	private:

		int x;

		std::string str;

	public:

		my_route() {
			this->x = 0;
		}

		virtual ~my_route() {

		}

		virtual const char *get_pattern() {
			return "/myroute";
		}

		virtual const char *process(const request *req, response *res) {
			usleep( 1000 * 140 );
			this->str = "Value is ";
			this->x++;
			this->str += std::to_string(this->x);
			this->str += ", Roger! Ain't that nice!...";
			this->str = "<form method='post' enctype='multipart/form-data'>\
				<input type='text' name='text' value='random'/>\
				<input type='file' name='file'/>\
				<input type='submit' value='x'/>\
				</form>";
			res->set_header("Content-Type", "text/html");
			res->set_header("Content-Length", std::to_string(this->str.length()));

			return this->str.c_str();
		}

};


extern "C" route *route_loader() {
	return new my_route();
}
