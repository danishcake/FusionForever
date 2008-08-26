#pragma once
#include "section.h"
#include <map>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

struct FilledOutlinedData
{
	int outline_dl;
	int outline_verts_index;
	int fill_dl;
	int fill_verts_index;

	float default_health;
	
	float mass;
	float energy_storage;
	float power_generation;
	float thrust;

	Vector3f default_subsection_position;
	Vector3f size;
};

class LuaSection :
	public Section
{
private:
	LuaSection(FilledOutlinedData _fill_outline_data);
	static std::map<std::string, FilledOutlinedData> name_map_;
protected:
	virtual void InitialiseGraphics();
public:	
	virtual ~LuaSection(void);
	static bool ParseSVGPath(std::string _path, FilledOutlinedData& _out);
	static LuaSection* CreateLuaSection(std::string _name, lua_State* luaVM);
	static void Triangulate(std::vector<Vector3f>& _temp_outline);
	
};