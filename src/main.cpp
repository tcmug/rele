
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

	if (argc < 2) {
		rele::logger::get_instance()->error("ERROR, no port provided");
		exit(1);
	}

	signal(SIGINT, intHandler);

	rele::server_process *p = new rele::server_process(atoi(argv[1]));

	p->start();

	delete p;

	return 0;
}

