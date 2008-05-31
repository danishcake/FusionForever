#include "StdAfx.h"
#include "LuaSection.h"
#include "Logger.h"

#include <sstream>
#include "Triangulate.h"
#include <boost/foreach.hpp>

std::map<std::string, FilledOutlinedData> LuaSection::name_map_ = std::map<std::string, FilledOutlinedData>();


LuaSection::LuaSection(FilledOutlinedData _fill_outline_data)
: Section()
{
	outline_verts_ = Datastore::Instance().GetVerts(_fill_outline_data.outline_verts_index); 
	outline_display_list_ = _fill_outline_data.outline_dl;
	fill_verts_ = Datastore::Instance().GetVerts(_fill_outline_data.fill_verts_index);
	fill_display_list_ = _fill_outline_data.fill_dl;
	findRadius();

	health_ = _fill_outline_data.default_health;
	max_health_ = health_;
	default_sub_section_position_ = _fill_outline_data.default_subsection_position;
}

LuaSection::~LuaSection(void)
{
}


void LuaSection::initialise_fill(void)
{

}

void LuaSection::initialise_outline(void)
{

}


LuaSection* LuaSection::CreateLuaSection(std::string _name, lua_State* luaVM)
{
	std::string file_name = std::string(_name);
	file_name.append(".lua");
	if(name_map_.find(_name) != name_map_.end())
	{
		return new LuaSection(name_map_[_name]);
	} else
	{
		bool loaded_and_run_ok = false;
		lua_getglobal(luaVM,"LuaSection");
		if(!lua_isnil(luaVM, -1))
		{
			lua_pushnil(luaVM);
			lua_setglobal(luaVM, "LuaSection");
		}
		lua_pop(luaVM,1); //Pops LuaSection from stack

		int load_result = luaL_loadfile(luaVM, file_name.c_str());
		if(load_result == LUA_ERRSYNTAX)
		{
			Logger::LogError("LuaSection::CreateLuaSection: A syntax error occurred while loading\n");
		} else if(load_result == LUA_ERRMEM)
		{
			Logger::LogError("LuaSection::CreateLuaSection: A memory allocation error occurred while loading\n");
		} else if(load_result == LUA_ERRFILE)
		{
			Logger::LogError("LuaSection::CreateLuaSection: Unable to load file:");
			Logger::LogError(_name);
			Logger::LogError("\n");
		} else
		{//Loaded OK. Function ready to run at top of stack.
			int run_result = lua_pcall(luaVM, 0, LUA_MULTRET, 0);
			if(run_result == LUA_ERRRUN)
			{
				Logger::LogError("LuaSection::CreateLuaSection: A runtime error occurred\n");
				if(lua_isstring(luaVM, -1))
					Logger::LogError(lua_tostring(luaVM, -1));
			}
			else if(run_result == LUA_ERRMEM)
			{
				Logger::LogError("LuaSection::CreateLuaSection: A memory allocation error occurred while running\n");
				if(lua_isstring(luaVM, -1))
					Logger::LogError(lua_tostring(luaVM, -1));
			}
			else if(run_result == LUA_ERRERR)
			{
				Logger::Log("LuaSection::CreateLuaSection: Error handling function error\n");
				if(lua_isstring(luaVM, -1))
					Logger::LogError(lua_tostring(luaVM, -1));
			}
			else
			{//Everything worked OK, script loaded
				 loaded_and_run_ok = true;
			}
		}

		if(loaded_and_run_ok)
		{
			lua_getglobal(luaVM,"LuaSection");
			if(lua_istable(luaVM, -1))
			{
				FilledOutlinedData indices;
				indices.size = Vector3f(0,0,0);
				indices.default_subsection_position = Vector3f(0,0,0);
				indices.default_health = 1000;


				//Get the health
				lua_pushstring(luaVM, "Health");
				lua_gettable(luaVM, -2);
				if(lua_isnumber(luaVM, -1))
				{
					indices.default_health = lua_tonumber(luaVM, -1);
				}
				lua_pop(luaVM, 1);
				//Get the size
				lua_pushstring(luaVM, "Size");
				lua_gettable(luaVM, -2);
				if(lua_istable(luaVM, -1))
				{
					lua_pushstring(luaVM, "x");
					lua_gettable(luaVM, -2);
					if(lua_isnumber(luaVM, -1))
					{
						indices.size.x = lua_tonumber(luaVM, -1);
					}
					lua_pop(luaVM, 1);

					lua_pushstring(luaVM, "y");
					lua_gettable(luaVM, -2);
					if(lua_isnumber(luaVM, -1))
					{
						indices.size.y = lua_tonumber(luaVM, -1);
					}
					lua_pop(luaVM, 1);
				}
				lua_pop(luaVM, 1);
				//Get default subsection position
				lua_pushstring(luaVM, "SubSectionPosition");
				lua_gettable(luaVM, -2);
				if(lua_istable(luaVM, -1))
				{
					lua_pushstring(luaVM, "x");
					lua_gettable(luaVM, -2);
					if(lua_isnumber(luaVM, -1))
					{
						indices.default_subsection_position.x = lua_tonumber(luaVM, -1);
					}
					lua_pop(luaVM, 1);

					lua_pushstring(luaVM, "y");
					lua_gettable(luaVM, -2);
					if(lua_isnumber(luaVM, -1))
					{
						indices.default_subsection_position.y = lua_tonumber(luaVM, -1);
					}
					lua_pop(luaVM, 1);
				}
				lua_pop(luaVM, 1);
				//Get lua section table
				lua_pushstring(luaVM, "data");
				lua_gettable(luaVM, -2);
				
				if(lua_isstring(luaVM, -1))
				{
					std::string outline_data = lua_tostring(luaVM, -1);
					lua_pop(luaVM, 1);
					if(ParseSVGPath(outline_data, indices))
					{
						lua_pop(luaVM, 1);
						name_map_[_name] = indices;
						return new LuaSection(indices);						
					} else
					{
						Logger::LogError(std::string("Could not parse data in LuaSection ") + _name);
						lua_pop(luaVM, 1);
					}					
				}
				else
				{
					Logger::LogError(std::string("LuaSection ") + _name + std::string(" must have data element declared as a string"));
					lua_pop(luaVM, 1);
				}
			}
			else
				Logger::LogError(std::string("LuaSection ") + _name + std::string(" must have key:value pair LuaSection:table"));
			lua_pop(luaVM,1); //Pop LuaSection from stack
		}
	}
	return NULL;	
}

bool LuaSection::ParseSVGPath(std::string _path, FilledOutlinedData& _out)
{
	bool error_found = false;

	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	bool is_absolute = false;
	bool ltv_is_absolute = false;
	int prev_stored = 0;
	int comma_pos = 0;
	for(int i = 0; i < _path.length(); i++)
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
					std::string coordy = _path.substr(comma_pos+1, i - comma_pos - 1);
					int c_x = atoi(coordx.c_str());
					int c_y = atoi(coordy.c_str());
					if(ltv_is_absolute)
						temp_outline->push_back(Vector3f(static_cast<float>(c_x), static_cast<float>(c_y), 0.0f));
					else
						temp_outline->push_back((*temp_outline)[temp_outline->size()-1] + Vector3f(static_cast<float>(c_x), static_cast<float>(c_y), 0));
					comma_pos = 0;
				} else
				{
					Logger::LogError("LuaSection: No comma found in coordinate");
					error_found = true;
				}
				
			}
			prev_stored = i;
		}
	}
	
	if(temp_outline->size() <= 2)
	{
		Logger::LogError("LuaSection: Must define more than 2 points");
		error_found = true;
	}

	if(!error_found)
	{		
		float lowX = temp_outline->at(0).x;
		float lowY = temp_outline->at(0).y;
		float highX = temp_outline->at(0).x;
		float highY = temp_outline->at(0).y;
		//First scale to size param
		BOOST_FOREACH(Vector3f vec, *temp_outline)
		{
			if(vec.x > highX)
				highX = vec.x;
			if(vec.x < lowX)
				lowX = vec.x;
			if(vec.y > highY)
				highY = vec.y;
			if(vec.y < lowY)
				lowY = vec.y;
		}
		float width = highX - lowX;
		float height = highY - lowY;
		float scaleX = _out.size.x > 0 ? _out.size.x / width : 1;
		float scaleY = _out.size.y > 0 ? _out.size.y / height : 1;
		
		BOOST_FOREACH(Vector3f& vec, *temp_outline)
		{
			vec.x *= scaleX;
			vec.y *= scaleY;
		}

		//Now triangulate the points
		boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
		
		//temp_outline->push_back((*temp_outline)[0]); //Need a closed shape
		temp_outline->pop_back(); //Remove last item - this is a line loop afterall!
		Triangulate::Process(temp_outline, temp_fill); //Form fill
		

		_out.fill_dl = CreateFillDisplayList(boost::shared_ptr<std::vector<Vector3f>>(temp_fill));
		_out.fill_verts_index = Datastore::Instance().AddVerts(temp_fill);
		_out.outline_dl = CreateOutlinedDisplayList(temp_outline);
		_out.outline_verts_index = Datastore::Instance().AddVerts(temp_outline);
	}

	return !error_found;
}
