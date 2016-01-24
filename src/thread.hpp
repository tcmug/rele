#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>

namespace rele {

/*
	Class: thread
	A thread wrapper class (to be removed)
*/
class thread {

	protected:

		std::thread _thread;

	public:

		thread();
		~thread();
		void *join();
		virtual void *run();

};

}; // Namespace

#endif
