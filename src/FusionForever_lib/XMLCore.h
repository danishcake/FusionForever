#pragma once
#include "Core.h"
#include <map>

struct XMLCoreData
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
	float thrust_scale;
	GLColor thrust_color;

	Vector3f default_subsection_position;
	Vector3f size;
	std::string filename;
};

class XMLCore :
	public Core
{
private:
	XMLCore(XMLCoreData _core_data, BaseAI* _AI);
	static std::map<std::string, XMLCoreData> name_map_;
protected:
	virtual void InitialiseGraphics();
public:	
	virtual ~XMLCore(void);
	static bool ParseSVGPath(std::string _path, XMLCoreData& _out);
	static XMLCore* CreateXMLCore(std::string _name);
	static void Triangulate(std::vector<Vector3f>& _temp_outline);
	virtual void ToXML(TiXmlElement* _node);
	
};