#pragma once
#include "Section.h"
#include <map>

struct XMLFilledOutlinedData
{
	int outline_dl;
	int outline_verts_index;
	int fill_dl;
	int fill_verts_index;
	int shield_outline_dl;
	int shield_outline_verts_index;

	float default_health;
	
	float mass;
	float energy_storage;
	float power_generation;
	float thrust;

	bool has_shield;
	float shield_health;
	float shield_shocktime;
	float shield_downtime;
	float shield_recharge_rate;
	float shield_recharge_cost;
	float shield_radius;

	Vector3f default_subsection_position;
	Vector3f size;
	std::string filename;
	std::vector<std::string> categories;
};

class XMLSection :
	public Section
{
private:
	XMLSection(XMLFilledOutlinedData _fill_outline_data);
	static std::map<std::string, XMLFilledOutlinedData> name_map_;
	std::string filename_;
protected:
	virtual void InitialiseGraphics();
public:	
	virtual ~XMLSection(void);
	static bool ParseSVGPath(std::string _path, XMLFilledOutlinedData& _out);
	static XMLSection* CreateXMLSection(std::string _name);
	static bool ParseXMLSection(std::string _name);
	static void Triangulate(std::vector<Vector3f>& _temp_outline);
	virtual void ToXML(TiXmlElement* _node);
	static std::vector<std::string> GetCategories(std::string _name);
	
};