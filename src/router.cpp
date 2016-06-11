
#include "router.hpp"
#include "dynamic_source.hpp"


using namespace rele;


void router::add_route(route *r) {
	this->routes.push_back(std::make_pair(std::regex(r->get_pattern()), r));
}


const char *router::route_request(const request *req, response *res) {
	for (const auto &i : this->routes) {	
		if (std::regex_match(req->get_query(), i.first)) {
			i.second->lock_source();
			const char *data = i.second->process(req, res);
			i.second->unlock_source();
			return data;
		}
	}
	return 0;
}


route *router::get_route_by_pattern(const char *pattern) {
	for (const auto &i : this->routes) {
		if (strcmp(i.second->get_pattern(), pattern) == 0) {
			return i.second;
		}
	}
	return 0;
}


route *router::remove_route_by_pattern(const char *pattern) {
	std::vector <std::pair <std::regex, class route*> >::iterator i;
	for (i = this->routes.begin(); i != this->routes.end(); i++) {
		if (strcmp(i->second->get_pattern(), pattern) == 0) {
			route *rt = i->second;
			this->routes.erase(i);
			return rt;
		}
	}
	return 0;
}




typedef route *(*t_route_maker)();

dynamic_source dyns;




void router::load_dynamic_routes() {

	if (!dyns.state_changed()) {
		return;
	}

	t_route_maker mkr = (t_route_maker)dyns.get_symbol("route_loader");
	route *r = mkr();
	r->source = &dyns;
	if (this->get_route_by_pattern(r->get_pattern())) {
		route *rt = this->remove_route_by_pattern(r->get_pattern());
		delete rt;
	}

	this->add_route(r);

	std::cout << " * Route added" << std::endl;

}


