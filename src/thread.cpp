


#include "thread.hpp"

using namespace rele;

#include <iostream>
#include <thread>


// Process thread callback
//
//
void _thread_proc(thread *t) {

	t->run();

}



// Constructor
//
//
thread::thread() {
	this->_thread = std::thread(_thread_proc, this);
}



// Destructor
//
//
thread::~thread() {
	this->join();
}



// Process thread
//
//
void *thread::run() {
	return 0;
}



// Join (wait for thread to finish)
//
//
void *thread::join() {

	this->_thread.join();
	return 0;
}





