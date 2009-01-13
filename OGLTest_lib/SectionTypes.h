#pragma once
#include <string>
#include <map>
#include <vector>
class Section;

namespace SectionTypes
{
	Section* GetSection(std::string _name);
	std::vector<std::string> GetNames();
	void RegisterSectionType(Section* (*factory_method_)(), std::string _name);
	void RegisterSections();
}