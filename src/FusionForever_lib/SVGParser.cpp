#include "stdafx.h"
#include "SVGParser.h"
#include <boost/algorithm/string.hpp>


namespace SVGParser
{
	std::vector<Vector3f> ParsePath(std::string _path)
	{
		bool error_found = false;

		std::vector<Vector3f> temp_outline;

		bool is_absolute = false;
		bool ltv_is_absolute = false;
		int prev_stored = 0;
		int comma_pos = 0;
		for(unsigned int i = 0; i < _path.length(); i++)
		{
			ltv_is_absolute = is_absolute;
			bool found = false; //Found an M, L, m or l
			if(_path[i] == 'M' || _path[i] == 'L')
			{
				is_absolute = true;
				found = true;
			} else if(_path[i] == 'm' || _path[i] == 'l')
			{
				is_absolute = false;
				found = true;
			} else if(_path[i] == 'z')
			{
				found = true;
			} else if(_path[i] == ',')
			{
				if(comma_pos==0)
					comma_pos = i;
			}
			if(found)
			{
				if(i!=0)
				{ //Between prev_stored and i there is a coordinate
					if(comma_pos != 0)
					{
						std::string coordx = _path.substr(prev_stored+1, comma_pos - prev_stored-1);
						boost::trim(coordx);
						std::string coordy = _path.substr(comma_pos+1, i - comma_pos - 1);
						boost::trim(coordy);
						float c_x = boost::lexical_cast<float, std::string>(coordx);
						float c_y = boost::lexical_cast<float, std::string>(coordy);
						if(ltv_is_absolute)
							temp_outline.push_back(Vector3f(c_x, c_y, 0.0f));
						else
							temp_outline.push_back(temp_outline[temp_outline.size()-1] + Vector3f(static_cast<float>(c_x), static_cast<float>(c_y), 0));
						comma_pos = 0;
					} else
					{
						std::string msg = "XMLSection: No comma found in coordinate\n";
						Logger::ErrorOut() << msg;
						throw msg;
					}
					
				}
				prev_stored = i;
			}
		}
		
		if(temp_outline.size() <= 2)
		{
			std::string msg = "Outline does not contain enough points\n";
			Logger::ErrorOut() << msg;
			throw msg;
		}

		return temp_outline;
	}
}