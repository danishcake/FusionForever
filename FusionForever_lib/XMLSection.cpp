#include "StdAfx.h"
#include "XMLSection.h"
#include <sstream>
#include "Triangulate.h"

std::map<std::string, XMLFilledOutlinedData> XMLSection::name_map_ = std::map<std::string, XMLFilledOutlinedData>();


XMLSection::XMLSection(XMLFilledOutlinedData _fill_outline_data)
: Section()
{
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(_fill_outline_data.outline_verts_index); 
	outline_.SetDisplayList(_fill_outline_data.outline_dl);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(_fill_outline_data.fill_verts_index);
	fill_.SetDisplayList(_fill_outline_data.fill_dl);
	findRadius();
	if(radius_ > 30)
	{
		Logger::ErrorOut() << "Warning: Section\"" << _fill_outline_data.filename << "\" exceeds maximum radius of 30. Radius = " << radius_ << "\n";
	}

	health_ = FlexFloat(_fill_outline_data.default_health, _fill_outline_data.default_health);
	default_sub_section_position_ = _fill_outline_data.default_subsection_position;
	mass_ = _fill_outline_data.mass;
	energy_ = FlexFloat(_fill_outline_data.energy_storage, _fill_outline_data.energy_storage);
	power_generation_ = FlexFloat(_fill_outline_data.power_generation);
	thrust_ = FlexFloat(_fill_outline_data.thrust);
	has_shield_ = _fill_outline_data.has_shield;
	shield_ = FlexFloat(_fill_outline_data.shield_health, _fill_outline_data.shield_health);
	shield_down_time_ = FlexFloat(_fill_outline_data.shield_downtime, _fill_outline_data.shield_downtime);
	shield_radius_ = FlexFloat(_fill_outline_data.shield_radius, _fill_outline_data.shield_radius);
	shield_recharge_cost_ = FlexFloat(_fill_outline_data.shield_recharge_cost, _fill_outline_data.shield_recharge_cost);
	shield_shock_time_ = FlexFloat(_fill_outline_data.shield_shocktime, _fill_outline_data.shield_shocktime);
	shield_recharge_rate_ = FlexFloat(_fill_outline_data.shield_recharge_rate, _fill_outline_data.shield_recharge_rate);
	if(has_shield_)
	{
		shield_outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(_fill_outline_data.shield_outline_verts_index);
		shield_outline_.SetDisplayList(_fill_outline_data.shield_outline_dl);
	}
	section_type_ = _fill_outline_data.filename;
}

XMLSection::~XMLSection(void)
{
}

std::vector<std::string> XMLSection::GetCategories(std::string _name)
{
	if(name_map_.find(_name) != name_map_.end())
	{
		return name_map_[_name].categories;
	} else
	{
		if(ParseXMLSection(_name))
			return name_map_[_name].categories; //Potential bug? If load fails then name_map_ still gains extra index
	}
	return std::vector<std::string>();
}

XMLSection* XMLSection::CreateXMLSection(std::string _name)
{
	if(name_map_.find(_name) != name_map_.end())
	{
		return new XMLSection(name_map_[_name]);
	} else
	{
		if(ParseXMLSection(_name))
			return new XMLSection(name_map_[_name]);
	}
	return NULL;
}

bool XMLSection::ParseXMLSection(std::string _name)
{
	std::string file_name;
	if(_name.find("Scripts/Sections") == std::string::npos)
		file_name = "Scripts/Sections/" + _name + ".xmlSection";
	else
		file_name = _name + ".xmlSection";

	XMLFilledOutlinedData indicies;
	indicies.filename = _name;
	TiXmlDocument section = TiXmlDocument(file_name.c_str());
	if(section.LoadFile())
	{
		TiXmlHandle section_handle = TiXmlHandle(&section);

		TiXmlElement* core_def = section_handle.FirstChild("SectionDefinition").Element();
		if(!core_def)
		{
			Logger::ErrorOut() << "Error parsing XMLCore" << file_name << " - no SectionDefinition element defined at root\n";
			return false;
		}

		TiXmlElement* health_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Health").Element();
		TiXmlElement* mass_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Mass").Element();
		TiXmlElement* outline_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Outline").Element();
		TiXmlElement* subsection_position_element =		section_handle.FirstChild("SectionDefinition").FirstChild("SubSectionPosition").Element();
		TiXmlElement* size_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Size").Element();
		TiXmlElement* thrust_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Thrust").Element();
		TiXmlElement* power_generation_element =		section_handle.FirstChild("SectionDefinition").FirstChild("PowerGeneration").Element();
		TiXmlElement* energy_storage_element =			section_handle.FirstChild("SectionDefinition").FirstChild("EnergyStorage").Element();
		TiXmlElement* shield_health_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("Health").Element();
		TiXmlElement* shield_shocktime_element =		section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("ShockTime").Element();
		TiXmlElement* shield_downtime_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("DownTime").Element();
		TiXmlElement* shield_recharge_rate_element =	section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("RechargeRate").Element();
		TiXmlElement* shield_recharge_cost_element =	section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("RechargeCost").Element();
		TiXmlElement* shield_radius_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("Radius").Element();
		TiXmlElement* categories_element =				section_handle.FirstChild("SectionDefinition").FirstChild("Categories").Element();


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
				Logger::ErrorOut() << "Mass in " << file_name << " not numeric:" << mass_element->GetText() << "\n";
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

		indicies.has_shield = false;
		indicies.shield_health = 0;
		indicies.shield_shocktime = 0;
		indicies.shield_downtime = 0;
		indicies.shield_recharge_rate = 0;
		indicies.shield_recharge_cost = 0;
		indicies.shield_radius = 0;

		if(shield_health_element			&&
		   shield_shocktime_element			&&
		   shield_downtime_element			&&
		   shield_recharge_rate_element		&&
		   shield_recharge_cost_element		&&
		   shield_radius_element)
		{
			bool shield_error = false;

			try
			{
				indicies.shield_health = boost::lexical_cast<float,std::string>(shield_health_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield health in " << file_name << " not numeric:" << shield_health_element->GetText() << "\n";
			}

			try
			{
				indicies.shield_shocktime = boost::lexical_cast<float,std::string>(shield_shocktime_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield shock time in " << file_name << " not numeric:" << shield_shocktime_element->GetText() << "\n";
			}

			try
			{
				indicies.shield_downtime = boost::lexical_cast<float,std::string>(shield_downtime_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield down time in " << file_name << " not numeric:" << shield_downtime_element->GetText() << "\n";
			}

			try
			{
				indicies.shield_recharge_rate = boost::lexical_cast<float,std::string>(shield_recharge_rate_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield recharge rate in " << file_name << " not numeric:" << shield_recharge_rate_element->GetText() << "\n";
			}

			try
			{
				indicies.shield_recharge_cost = boost::lexical_cast<float,std::string>(shield_recharge_cost_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield recharge cost in " << file_name << " not numeric:" << shield_recharge_cost_element->GetText() << "\n";
			}

			try
			{
				indicies.shield_radius = boost::lexical_cast<float,std::string>(shield_radius_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				shield_error = true;
				Logger::ErrorOut() << "Shield radius in " << file_name << " not numeric:" << shield_radius_element->GetText() << "\n";
			}

			if(!shield_error)
			{
				indicies.has_shield = true;
				boost::shared_ptr<std::vector<Vector3f>> verts(new std::vector<Vector3f>());
				for(int i = 0; i < 64; i++)
				{
					float angle = static_cast<float>(i) * 2 * M_PI / 64.0f ;
					(*verts).push_back(Vector3f(indicies.shield_radius * cosf(angle), indicies.shield_radius * sinf(angle), 0));
				}
				indicies.shield_outline_verts_index = Datastore::Instance().AddVerts(verts);
				indicies.shield_outline_dl = Outlined::CreateOutlinedDisplayList(verts);
			}
		}

		if(categories_element)
		{
			TiXmlElement* category = categories_element->FirstChildElement("Category");
			while(category)
			{
				std::string category_text = category->GetText();
				if(category_text.length() > 0)
				{
					indicies.categories.push_back(category_text);
				} else
				{
					Logger::ErrorOut() << "Zero length category text in " << file_name << "\n";
				}
				category = category->NextSiblingElement("Category");
			}
		}


		if(outline_element)
		{
			if(ParseSVGPath(outline_element->GetText(), indicies))
			{
				name_map_[_name] = indicies;
				return true;
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
		Logger::ErrorOut() << "Unable to open file '" << file_name << "' :" <<section.ErrorDesc() << "\n";
	}
	return false;
}

bool XMLSection::ParseSVGPath(std::string _path, XMLFilledOutlinedData& _out)
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
					Logger::ErrorOut() << "XMLSection: No comma found in coordinate\n";
					error_found = true;
				}
				
			}
			prev_stored = i;
		}
	}
	
	if(temp_outline->size() <= 2)
	{
		Logger::ErrorOut() << "LuaSection: Must define more than 2 points\n";
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

void XMLSection::InitialiseGraphics()
{
}

void XMLSection::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", section_type_);
}