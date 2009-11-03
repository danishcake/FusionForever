#include "StdAfx.h"
#include "XMLCore.h"
#include <sstream>
#include "Triangulate.h"
#include "SVGParser.h"
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
	thrust_color_ = _fill_outline_data.thrust_color;
	thrust_scale_ = _fill_outline_data.thrust_scale;
	section_type_ = _fill_outline_data.filename;
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
				Logger::ErrorOut() << "Error parsing XMLCore: " << file_name << " - no CoreDefinition element defined at root\n";
				return NULL;
			}

			TiXmlElement* health_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Health").Element();
			TiXmlElement* mass_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Mass").Element();
			TiXmlElement* outline_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Outline").Element();
			TiXmlElement* subsection_position_element =		section_handle.FirstChild("CoreDefinition").FirstChild("SubSectionPosition").Element();
			TiXmlElement* size_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Size").Element();
			TiXmlElement* thrust_element =					section_handle.FirstChild("CoreDefinition").FirstChild("Thrust").FirstChild("Power").Element();
			TiXmlElement* thrust_scale_element =			section_handle.FirstChild("CoreDefinition").FirstChild("Thrust").FirstChild("TrailScale").Element();
			TiXmlElement* thrust_color_element =			section_handle.FirstChild("CoreDefinition").FirstChild("Thrust").FirstChild("TrailColor").Element();
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
					Logger::ErrorOut() << "Health in " << file_name << " not numeric:" << health_element->GetText() << "\n";
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
					Logger::ErrorOut() <<"Mass in " << file_name << " not numeric:" << mass_element->GetText() << "\n";
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
					Logger::ErrorOut() << "Thrust in " << file_name << " not numeric:" << thrust_element->GetText() << "\n";
				}
			}

			indicies.thrust_scale = -1;
			if(thrust_scale_element)
			{
				try
				{
					indicies.thrust_scale = boost::lexical_cast<float,std::string>(thrust_scale_element->GetText());
				}
				catch(boost::bad_lexical_cast &)
				{
					Logger::ErrorOut() << "Thrust scale in " << file_name << " not numeric:" << thrust_scale_element->GetText() << "\n";
				}
			}

			indicies.thrust_color = GLColor(255, 255, 255);
			if(thrust_color_element)
			{
				int r = 255;
				int g = 255;
				int b = 255;
				if(thrust_color_element->QueryValueAttribute("r", &r) != TIXML_SUCCESS ||
				   thrust_color_element->QueryValueAttribute("g", &g) != TIXML_SUCCESS ||
				   thrust_color_element->QueryValueAttribute("b", &b)!= TIXML_SUCCESS)
				{
					Logger::ErrorOut() << "Thrust color in " << file_name << " does not contain all of r, g & b attributes, or they cannot be changed to unsigned char\n";
				} else
				{
					indicies.thrust_color.r = r;
					indicies.thrust_color.g = g;
					indicies.thrust_color.b = b;
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
					Logger::ErrorOut() << "Energy storage in " << file_name << " not numeric:" << energy_storage_element->GetText() << "\n";
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
					Logger::ErrorOut() << "Power generation in " << file_name << " not numeric:" << power_generation_element->GetText() << "\n";
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
					Logger::ErrorOut() << "Error parsing outline in " << file_name << "\n";
				}
			} else
			{
				Logger::ErrorOut() << "Required field Outline missing in " << file_name << "\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to open file '" << file_name << "' :" << section.ErrorDesc() << "\n";
		}
	}
	return NULL;	
}

bool XMLCore::ParseSVGPath(std::string _path, XMLCoreData& _out)
{
	bool error_found = false;

	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	try
	{
		*temp_outline = SVGParser::ParsePath(_path);
	}
	catch(std::string error)
	{
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
	_node->SetAttribute("SectionType", section_type_);
}