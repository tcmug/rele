

#include "ini.hpp"


//
//
//
// Constructor
ini::ini(const char *filename) {
	FILE *fp;
	char key[512], value[512];
	char c;
	int len;

	fp = fopen(filename, "r");

	if (fp) {
		do {
			c = fscanf(fp, "%s", key);
			len = strlen(key);
			if (key[len-1] == ':') {
				key[len-1] = 0;
				c = fscanf(fp, "%s", value);
				this->settings[key] = value;

			}

		} while (c != EOF);
	}

	fclose(fp);

}


//
//
//
// Get string
std::string ini::get_string(std::string value, std::string def) const {
	std::map <std::string, std::string>::const_iterator i = this->settings.find(value);
	if (i == this->settings.end()) {
		return def;
	}
	return i->second;
}



//
//
//
// Get int
int ini::get_int(std::string value, int def) const {
	std::map <std::string, std::string>::const_iterator i = this->settings.find(value);
	if (i == this->settings.end()) {
		return def;
	}
	return std::stoi(i->second.c_str());
}



//
//
//
// Get int
float ini::get_float(std::string value, float def) const {
	std::map <std::string, std::string>::const_iterator i = this->settings.find(value);
	if (i == this->settings.end()) {
		return def;
	}
	return std::stof(i->second.c_str());
}
