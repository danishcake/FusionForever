#pragma once
#include <string>
#include <map>
#include <vector>
class Section;

class ListAdder
{
private:
	static std::map<std::string, Section* (*)()>* GetMap();
public:
	ListAdder(Section* (*factory_method_)(), std::string _name);
	~ListAdder(void);
	static std::vector<std::string> GetNames();
	static Section* GetSection(std::string _name);
};
