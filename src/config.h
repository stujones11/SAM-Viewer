#ifndef D_CONFIG_H
#define D_CONFIG_H

#include <map>

class Vector
{
public:
	Vector() : x(0), y(0), z(0) {}
	Vector(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector(const std::string &str);
	float x;
	float y;
	float z;
};

class Config
{
public:
	Config(const std::string &filename) : filename(filename) {}
	bool load();
	bool save();
	bool hasKey(const std::string &key) const;
	void set(const std::string &key, const std::string &value);
	const std::string &get(const std::string &key) const;
	const char *getCStr(const std::string &key) const;
	int getInt(const std::string &key) const;
	int getHex(const std::string &key) const;
	bool getBool(const std::string &key) const;
	Vector getVector(const std::string &key) const;

private:
	std::map<std::string, std::string> config;
	std::string filename;
};

#endif // D_CONFIG_H
