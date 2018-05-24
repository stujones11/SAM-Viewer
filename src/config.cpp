#include <fstream>
#include <stdlib.h>

#include "config.h"

Vector::Vector(const std::string &str)
{
	sscanf(str.c_str(), "%f,%f,%f", &x, &y, &z);
}

bool Config::load()
{
	std::ifstream file(filename.c_str());
	if (!file)
		return false;

	std::string line;
	while (std::getline(file, line))
	{
		auto index = line.find("=");
		std::string key = line.substr(0, index);
		std::string value = line.substr(index + 1);
		config[key] = value;
	}
	file.close();
	return true;
}

bool Config::save()
{
	std::ofstream file(filename.c_str());
	if (!file)
		return false;

	for(std::map<std::string, std::string>::iterator it = config.begin();
		it != config.end(); it++)
	{
		file << it->first << "=" << it->second << "\n";
	}
	file.close();
	return true;
}

bool Config::hasKey(const std::string &key) const
{
	return config.find(key) != config.end();
}

void Config::set(const std::string &key, const std::string &value)
{
	config[key] = value;
}

const std::string &Config::get(const std::string &key) const
{
	return config.find(key)->second;
}

const char *Config::getCStr(const std::string &key) const
{
	return get(key).c_str();
}

int Config::getInt(const std::string &key) const
{
	return atoi(getCStr(key));
}

int Config::getHex(const std::string &key) const
{
	return std::stoul(get(key), nullptr, 16);
}

bool Config::getBool(const std::string &key) const
{	
	return get(key) == "true";
}

Vector Config::getVector(const std::string &key) const
{
	return Vector(get(key));
}