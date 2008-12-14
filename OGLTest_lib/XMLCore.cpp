#include "StdAfx.h"
#include "XMLCore.h"
#include <sstream>
#include "Triangulate.h"
#include "RotatingAI.h"

std::map<std::string, XMLCoreData> XMLCore::name_map_ = std::map<std::string, XMLCoreData>();


XMLCore::XMLCore(XMLCoreData _fill_outline_data, BaseAI* _AI)
: Core(_AI)
{
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(_fill_outline_data.outline_verts_index); 
	outline_.SetDisplayList(_fill_outline_data.outline_dl);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(_fill_outline_data.fill_verts_index);
	fill_.SetDisplayList(_fill_outline_data.fill_dl);
	findRadius();

	health_ = FlexFloat(_fill_outline_data.default_health, _fill_outline_data.default_health);
	default_sub_section_position_ = _fill_outline_data.default_subsection_position;
	mass_ = _fill_outline_data.mass;
	energy_ = FlexFloat(_fill_outline_data.energy_storage, _fill_outline_data.energy_storage);
	power_generation_ = FlexFloat(_fill_outline_data.power_generation);
	thrust_ = FlexFloat(_fill_outline_data.thrust);
	filename_ = _fill_outline_data.filename;
}

XMLCore::~XMLCore(void)
{
}

XMLCore* XMLCore::CreateXMLCore(std::string _name)
{
	std::string file_name;
	if(_name.find("Scripts/Sections") == std::string::npos)
		file_name = "Scripts/Sections/" + _name + ".xmlCore";
	else
		file_name = _name + ".xmlCore";

	if(name_map_.find(_name) != name_map_.end())
	{
		return new XMLCore(name_map_[_name], new RotatingAI(0.1f));
	} else
	{
		XMLCoreData indicies;
		indicies.filename = _name;
		TiXmlDocument section = TiXmlDocument(file_name.c_str());
		if(section.LoadFile())
		{
			TiXmlHandle section_handle = TiXmlHandle(&section);

			TiXmlElement* core_def = section_handle.FirstChild("CoreDefinition").Element();
			if(!core_def)
			{
				Logger::LogError(std::string("Error parsing XMLCore") + file_name + std::string(" - no CoreDefinition element defined at root"));
				return NULL;
			}

			TiXmlElement* health_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Health").Element();
			TiXmlElement* mass_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Mass").Element();
			TiXmlElement* outline_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Outline").Element();
			TiXmlElement* subsection_position_element =		section_handle.FirstChild("CoreDefinition").FirstChild("SubSectionPosition").Element();
			TiXmlElement* size_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Size").Element();
			TiXmlElement* thrust_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Thrust").Element();
			TiXmlElement* power_generation_element =		section_handle.FirstChild("CoreDefinition").FirstChild("PowerGeneration").Element();
			TiXmlElement* energy_storage_element =			section_handle.FirstChild("CoreDefinition").FirstChild("EnergyStorage").Element();

			indicies.default_subsection_position = Vector3f();
			if(subsection_position_element)
			{
				subsection_position_element->QueryValueAttribute("x", &indicies.default_subsection_position.x);
				subsection_position_element->QueryValueAttribute("y", &indicies.default_subsection_position.y);
			}

			indicies.size = Vector3f();
			if(size_element)
			{
				size_element->QueryValueAttribute("x", &indicies.size.x);
				size_element->QueryValueAttribute("y", &indicies.size.y);
			}

			indicies.default_health = 500;
			if(health_element)
			{
				try
				{
					indicies.default_health = boost::lexical_cast<float,std::string>(health_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::LogError("Health in " + file_name + " not numeric:" + health_element->GetText());
				}
			}

			indicies.mass = 100;
			if(mass_element)
			{
				try
				{
					indicies.mass = boost::lexical_cast<float,std::string>(mass_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::LogError("Mass in " + file_name + " not numeric:" + mass_element->GetText());
				}
			}

			indicies.thrust = 0;
			if(thrust_element)
			{
				try
				{
					indicies.thrust = boost::lexical_cast<float,std::string>(thrust_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::LogError("Thrust in " + file_name + " not numeric:" + thrust_element->GetText());
				}
			}

			indicies.energy_storage = 0;
			if(energy_storage_element)
			{
				try
				{
					indicies.energy_storage = boost::lexical_cast<float,std::string>(energy_storage_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::LogError("Energy storage in " + file_name + " not numeric:" + energy_storage_element->GetText());
				}
			}

			indicies.power_generation = 0;
			if(power_generation_element)
			{
				try
				{
					indicies.power_generation = boost::lexical_cast<float,std::string>(power_generation_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::LogError("Power generation in " + file_name + " not numeric:" + power_generation_element->GetText());
				}
			}

			if(outline_element)
			{
				if(ParseSVGPath(outline_element->GetText(), indicies))
				{
					name_map_[_name] = indicies;
					return new XMLCore(indicies, new RotatingAI(0.1f));
				} else
				{
					Logger::LogError("Error parsing outline in " + file_name);
				}
			} else
			{
				Logger::LogError("Required field Outline missing in " + file_name);
			}
		} else
		{
			Logger::LogError(std::string("Unable to open file '") + file_name + std::string("' :") + std::string(section.ErrorDesc()));
		}
	}
	return NULL;	
}

bool XMLCore::ParseSVGPath(std::string _path, XMLCoreData& _out)
{
	bool error_found = false;

	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

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
					Logger::LogError("XMLCore: No comma found in coordinate");
					error_found = true;
				}
				
			}
			prev_stored = i;
		}
	}
	
	if(temp_outline->size() <= 2)
	{
		Logger::LogError("XMLCore: Must define more than 2 points");
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
		
		_out.fill_dl = Filled::CreateFillDisplayList(boost::shared_ptr<std::vector<Vector3f>>(temp_fill));
		_out.fill_verts_index = Datastore::Instance().AddVerts(temp_fill);
		_out.outline_dl = Outlined::CreateOutlinedDisplayList(temp_outline);
		_out.outline_verts_index = Datastore::Instance().AddVerts(temp_outline);
	}

	return !error_found;
}

void XMLCore::InitialiseGraphics()
{
}

void XMLCore::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", filename_);
}