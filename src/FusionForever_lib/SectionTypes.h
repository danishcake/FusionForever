#pragma once
#include <string>
#include <map>
#include <vector>
class Section;

namespace SectionTypes
{
	Section* GetSection(std::string _name);
	std::vector<std::string> GetSectionNames();
	std::vector<std::string> GetCoreNames();
	void RegisterSectionType(Section* (*factory_method_)(), std::string _name);
	void RegisterXMLSectionType(std::string _name);
	void RegisterXMLCoreType(std::string _name);
	void RegisterSections();
}