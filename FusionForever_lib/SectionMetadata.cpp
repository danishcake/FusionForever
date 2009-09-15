#include "stdafx.h"
#include "SectionMetadata.h"
#include <map>
#include "vmath.h"

namespace SectionMetadata
{
	static std::map<std::string, std::vector<std::string>> tags;
	static std::map<std::string, std::map<std::string, double> > values;
	static std::map<std::string, std::map<std::string, Vector2d>> coordinates;

	void RegisterSectionTag(std::string _section_type, std::string _tag)
	{
		tags[_section_type].push_back(_tag);
	}

	void RegisterSectionKeyValue(std::string _section_type, std::string _key, double _value)
	{
		values[_section_type][_key] = _value;
	}

	void RegisterSectionKeyCoordinate(std::string _section_type, std::string _key, double _x, double _y)
	{
		coordinates[_section_type][_key] = Vector2d(_x, _y);
	}

	std::vector<std::string> GetTags(const std::string _section_type)
	{
		if(tags.find(_section_type) != tags.end())
			return tags[_section_type];
		return std::vector<std::string>();
	}

	std::map<std::string, double> GetValues(const std::string _section_type)
	{
		if(values.find(_section_type) != values.end())
			return values[_section_type];
		return std::map<std::string, double>();
	}
}
