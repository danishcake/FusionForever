#include "stdafx.h"
#include "SectionMetadata.h"
#include <map>
#include "vmath.h"

namespace SectionMetadata
{
	static std::map<std::string, std::string> tags;
	static std::map<std::string, std::map<std::string, double> > values;
	static std::map<std::string, std::map<std::string, Vector2d>> coordinates;

	void RegisterSectionTag(std::string _section_type, std::string _tag)
	{
		tags[_section_type] = _tag;
	}
	void RegisterSectionKeyValue(std::string _section_type, std::string _key, double _value)
	{
		values[_section_type][_key] = _value;
	}
	void RegisterSectionKeyCoordinate(std::string _section_type, std::string _key, double _x, double _y)
	{
		coordinates[_section_type][_key] = Vector2d(_x, _y);
	}
}
