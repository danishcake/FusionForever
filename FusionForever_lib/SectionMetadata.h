#pragma once
#include <string>
#include <vector>
#include <map>

namespace SectionMetadata
{
	void RegisterSectionTag(const std::string _section_type, const std::string _tag);
	void RegisterSectionKeyValue(const std::string _section_type, const std::string _key, const double _value);
	void RegisterSectionKeyCoordinate(const std::string _section_type, const std::string _key, const double _x, const double _y);

	std::vector<std::string> GetTags(const std::string _section_type);
	std::map<std::string, double> GetValues(const std::string _section_type);
}