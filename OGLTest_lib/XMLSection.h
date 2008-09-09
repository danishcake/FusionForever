#pragma once
#include "Section.h"
#include <map>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

struct XMLFilledOutlinedData
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

class XMLSection :
	public Section
{
private:
	XMLSection(XMLFilledOutlinedData _fill_outline_data);
	static std::map<std::string, XMLFilledOutlinedData> name_map_;
protected:
	virtual void InitialiseGraphics();
public:	
	virtual ~XMLSection(void);
	static bool ParseSVGPath(std::string _path, XMLFilledOutlinedData& _out);
	static XMLSection* CreateXMLSection(std::string _name);
	static void Triangulate(std::vector<Vector3f>& _temp_outline);
	
};