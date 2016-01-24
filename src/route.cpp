
#include "router.hpp"

#include "dynamic_source.hpp"

using namespace rele;

route::route() {
	this->source = 0;
}


route::~route() {
}


const char *route::get_pattern() {
	return "/";
}


const char *route::process(const request *r, response *res) {
	res->set_header("Content-Type", "text/html");
	res->set_header("Content-Length", "12");
	return "Hello world!";
}



void route::lock_source() {
	if (this->source) {
		this->source->start_task();
	}
}

void route::unlock_source() {
	if (this->source) {
		this->source->end_task();
	}
}
