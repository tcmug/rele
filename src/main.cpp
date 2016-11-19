
#include "buffer.hpp"
#include "server_process.hpp"
#include "server_thread.hpp"
#include "thread.hpp"
#include "logger.hpp"
#include "dynamic_source.hpp"

#include <iostream>
#include <map>
#include <string>


void intHandler(int n) {
	rele::logger::get_instance()->error("Quitting...");
	exit(0);
}

/*
	Function: main

	This is the main entrypoint to the application.

	Parameters:

		argc - argument count
		argv - arguments
*/
int main(int argc, char *argv[]) {

	signal(SIGINT, intHandler);
	signal(SIGPIPE, SIG_IGN);

	ini conf("rele.ini");

	rele::server_process *p = new rele::server_process(conf);

	p->start();

	delete p;

	return 0;
}

