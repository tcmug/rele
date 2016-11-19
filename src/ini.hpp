#ifndef INI_HPP
#define INI_HPP


#include <string>
#include <map>

class ini {

	private:

		std::map <std::string, std::string> settings;

	public:

		ini(const char *filename);

        std::string get_string(std::string value, std::string def) const;
		int get_int(std::string value, int def) const;
		float get_float(std::string value, float def) const;

};

#endif
