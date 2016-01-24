
#include "dynamic_source.hpp"

using namespace rele;

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include <assert.h>

#include "logger.hpp"

time_t ccc_get_mtime(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) == -1) {
		//perror(path);
	}
	return statbuf.st_mtime;
}




dynamic_source::dynamic_source() {
	this->lib_handle = NULL;
	this->reference_count = 0;
	this->state = 0;
	this->path = "routes/";
	this->lib = "routes/myroute.so";
	this->source = "routes/src/myroute.cpp";
}



bool dynamic_source::state_changed() {

	std::unique_lock <std::mutex>lk(this->state_m);
	bool require_comp = ccc_get_mtime(this->lib.c_str()) < ccc_get_mtime(this->source.c_str());
	this->state = require_comp ? 1 : this->lib_handle == 0;

	if (this->state == 1) {

		rele::logger::get_instance()->log("I'm the compiling thread!");

		if (this->lib_handle > 0) {
			rele::logger::get_instance()->log("Waiting for others to stop using the library.");
			std::unique_lock<std::mutex> lck(this->reference_m);
			reference_cv.wait(lck, [this] { return this->reference_count == 0; });

			dlclose(this->lib_handle);
			rele::logger::get_instance()->log("Unloaded!");
			this->lib_handle = 0;
		}

		std::string cmd = "cd ";
		cmd += this->path;
		cmd += " && make";
		::system(cmd.c_str());
		std::cout << "Compiled!" << std::endl;

		this->lib_handle = dlopen(this->lib.c_str(), RTLD_NOW);
		if (this->lib_handle == NULL){
			rele::logger::get_instance()->log_error(std::string("Compilation error: ") + this->lib);
			exit(1);
		}

		rele::logger::get_instance()->log(std::string("Library loaded: ") + this->lib);

		this->state = 0;

		return true;

	}

	//rele::logger::get_instance()->log(std::string("State OK for ") + this->lib);

	return false;
}



void dynamic_source::start_task() {
	std::unique_lock<std::mutex> lck(this->reference_m);
	this->reference_count++;
}


void dynamic_source::end_task() {
	// Signal possibly waiting unload mutex.
	std::lock_guard<std::mutex> lck(this->reference_m);
	this->reference_count--;
	this->reference_cv.notify_one();
}


void *dynamic_source::get_symbol(const char *sym) {
	assert(this->lib_handle != NULL);
	return dlsym(this->lib_handle, sym);
}
