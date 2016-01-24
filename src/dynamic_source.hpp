#ifndef DYNAMIC_SOURCE_HPP

#include <mutex>
#include <condition_variable>
#include <mutex>
#include <string>

namespace rele {

/*
	Class: dynamic_source
	Wrapper for loading and taking care of a dynamic source
*/
class dynamic_source {

	private:

		/*
			Variable: lib_handle
			The so library handle.
		*/
		void *lib_handle;

		/*
			Variable: reference_count
			Records how many threads are using this source.
		*/
		int reference_count;

		/*
			Variable: m
			Mutex variable for processing data.
		*/
		std::mutex reference_m;

		/*
			Variable: cv
			Condition variable for processing data.
		*/
		std::condition_variable reference_cv;

		/*
			Variable: compile_m
			Compile variable
		*/
		std::mutex state_m;

		int state;

		/*
			Variable: source
			Source file to monitor for changes.
		*/
		std::string source;

		/*
			Variable: path
			Path of the route.
		*/
		std::string path;

		/*
			Variable: lib
			Library ([name].so) file to monitor.
		*/
		std::string lib;


	public:


		/*
			Constructor: dynamic_source
		*/
		dynamic_source();


		/*
			Function: compile
			Compile the source in to a dynamic library.
			Returns:
				True when state has changed, false when not.
		*/
		bool state_changed();


		/*
			Function: loaded
			Is the library loaded?

			Returns:
				True if yes, false if not.
		*/
		bool loaded();

		/*
			Function: load
			Load the library
		*/
		bool load();

		/*
			Function: unload
			Unload the library
		*/
		void unload();

		/*
			Function: start_task
			Begin using this <dynamic_source>, eg. to serve a <request>.
		*/
		void start_task();

		/*
			Function: end_task
			End using this <dynamic_source>.
		*/
		void end_task();

		/*
			Function: get_symbol
			Get a symbol from the loaded library.

			Returns:
				Pointer to the symbol or NULL if not
		*/
		void *get_symbol(const char *str);
};

}

#endif
