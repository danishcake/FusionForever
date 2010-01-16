#include "StdAfx.h"
#include "XMLSection.h"
#include <sstream>
#include "Triangulate.h"
#include <boost/filesystem.hpp>
#include "SectionMetadata.h"
#include "SVGParser.h"
#include "SectionTypes.h"

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
	trails_ = _fill_outline_data.trails;
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
		TiXmlElement* thrust_power_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Thrust").FirstChild("Power").Element();
		TiXmlElement* power_generation_element =		section_handle.FirstChild("SectionDefinition").FirstChild("PowerGeneration").Element();
		TiXmlElement* energy_storage_element =			section_handle.FirstChild("SectionDefinition").FirstChild("EnergyStorage").Element();
		TiXmlElement* shield_health_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("Health").Element();
		TiXmlElement* shield_shocktime_element =		section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("ShockTime").Element();
		TiXmlElement* shield_downtime_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("DownTime").Element();
		TiXmlElement* shield_recharge_rate_element =	section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("RechargeRate").Element();
		TiXmlElement* shield_recharge_cost_element =	section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("RechargeCost").Element();
		TiXmlElement* shield_radius_element =			section_handle.FirstChild("SectionDefinition").FirstChild("Shield").FirstChild("Radius").Element();
		TiXmlElement* categories_element =				section_handle.FirstChild("SectionDefinition").FirstChild("Categories").Element();
		TiXmlElement* tags_element =					section_handle.FirstChild("SectionDefinition").FirstChild("Tags").Element();


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
		if(thrust_power_element)
		{
			try
			{
				indicies.thrust = boost::lexical_cast<float,std::string>(thrust_power_element->GetText());
			}
			catch(boost::bad_lexical_cast &)
			{
				Logger::ErrorOut() << "Thrust in " << file_name << " not numeric:" << thrust_power_element->GetText() << "\n";
			}
		}

		if(thrust_element)
		{
			TiXmlElement* thrust_trail = thrust_element->FirstChildElement("Trail");
			while(thrust_trail)
			{
				float thrust_length_scale = 1;
				float thrust_width_scale = 1;
				GLColor thrust_color(255, 255, 255);
				Vector3f thrust_position;
				float thrust_angle = 0;
				bool angular_corrected = true;
				float antiparallel_factor = 0.3f;
				TiXmlElement* thrust_length_scale_element = thrust_trail->FirstChildElement("TrailLengthScale");
				TiXmlElement* thrust_width_scale_element = thrust_trail->FirstChildElement("TrailWidthScale");
				TiXmlElement* thrust_color_element = thrust_trail->FirstChildElement("TrailColor");
				TiXmlElement* thrust_position_element = thrust_trail->FirstChildElement("TrailPosition");
				TiXmlElement* thrust_angle_element = thrust_trail->FirstChildElement("TrailAngle");
				TiXmlElement* thrust_angular_corrected_element = thrust_trail->FirstChildElement("TrailAngularCorrection");
				TiXmlElement* thrust_antiparallel_factor_element = thrust_trail->FirstChildElement("TrailAntiparallelFactor");
				// These are all optional and default to 1, white, (0,0), 0 and false respectively
				if(thrust_length_scale_element)
				{
					try
					{
						thrust_length_scale = boost::lexical_cast<float,std::string>(thrust_length_scale_element->GetText());
					}
					catch(boost::bad_lexical_cast &)
					{
						Logger::ErrorOut() << "Thrust length scale in " << file_name << " not numeric:" << thrust_length_scale_element->GetText() << "\n";
					}
				}
				if(thrust_width_scale_element)
				{
					try
					{
						thrust_width_scale = boost::lexical_cast<float,std::string>(thrust_width_scale_element->GetText());
					}
					catch(boost::bad_lexical_cast &)
					{
						Logger::ErrorOut() << "Thrust width scale in " << file_name << " not numeric:" << thrust_width_scale_element->GetText() << "\n";
					}
				}
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
						thrust_color.r = r;
						thrust_color.g = g;
						thrust_color.b = b;
					}
				}
				if(thrust_position_element)
				{
					float x = 0;
					float y = 0;
					if(thrust_position_element->QueryValueAttribute("x", &x) != TIXML_SUCCESS ||
					   thrust_position_element->QueryValueAttribute("x", &x) != TIXML_SUCCESS)
					{
						Logger::ErrorOut() << "Thrust position in " << file_name << " does not contain x & y attributes, or they cannot be changed to float\n";
					} else
					{
						thrust_position.x = x;
						thrust_position.y = y;
					}
				}
				if(thrust_angle_element)
				{
					try
					{
						thrust_angle = boost::lexical_cast<float,std::string>(thrust_angle_element->GetText());
					}
					catch(boost::bad_lexical_cast &)
					{
						Logger::ErrorOut() << "Thrust angle in " << file_name << " not numeric:" << thrust_angle_element->GetText() << "\n";
					}
				}
				if(thrust_angular_corrected_element)
				{
					int trail_angular_correction = 0;
					if(thrust_angular_corrected_element->QueryIntAttribute("value", &trail_angular_correction) != TIXML_SUCCESS)
					{
						Logger::ErrorOut() << "Thrust TrailAngularCorrection value attribute missing or not castable to int in" << file_name << "\n";
					} else
					{
						angular_corrected = trail_angular_correction != 0;
					}
				}
				if(thrust_antiparallel_factor_element)
				{
					try
					{
						antiparallel_factor = boost::lexical_cast<float,std::string>(thrust_antiparallel_factor_element->GetText());
					}
					catch(boost::bad_lexical_cast &)
					{
						Logger::ErrorOut() << "Thrust antiparallel factor in " << file_name << " not numeric:" << thrust_antiparallel_factor_element->GetText() << "\n";
					}
				}

				ThrusterData td;
				td.angle = thrust_angle;
				td.color = thrust_color;
				td.length_scale = thrust_length_scale;
				td.width_scale = thrust_width_scale;
				td.position = thrust_position;
				td.angular_correction = angular_corrected;
				td.antiparallel_factor = antiparallel_factor;
				indicies.trails.push_back(td);
				thrust_trail = thrust_trail->NextSiblingElement("Trail");
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
		if(tags_element)
		{
			TiXmlElement* tag = tags_element->FirstChildElement("Tag");
			while(tag)
			{
				std::string tag_text = tag->GetText();
				if(tag_text.length() > 0)
				{
					indicies.tags.push_back(tag_text);
				} else
				{
					Logger::ErrorOut() << "Zero length tag text in " << file_name << "\n";
				}
				tag = tag->NextSiblingElement("Tag");
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

void XMLSection::InitialiseGraphics()
{
}

void XMLSection::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", section_type_);
}

void XMLSection::Preload()
{
	int success_count = 0;
	int total_count = 0;
	Logger::DiagnosticOut() << "Preloading XMLSections\n";
	/* Iterate over XML sections and load them. */
	boost::filesystem::directory_iterator end_itr;
	for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Sections");
		itr != end_itr;
		++itr)
	{
		if(boost::filesystem::is_regular((itr->status())))
		{
			std::string ext = boost::filesystem::extension(*itr);
			if(ext == ".XMLSection")
			{
				std::string section_name = boost::filesystem::basename(itr->path());
				XMLSection* section = XMLSection::CreateXMLSection(section_name);
				if(section)
				{
					SectionMetadata::RegisterSection(section_name);
					success_count++;
					if(name_map_.find(section_name) != name_map_.end())
					{
						for(std::vector<std::string>::iterator it = name_map_[section_name].tags.begin(); it != name_map_[section_name].tags.end(); ++it)
						{
							SectionMetadata::RegisterSectionTag(section_name, *it);
						}
						SectionMetadata::RegisterSectionKeyCoordinate(section_name,"default_sub_section_position", 
							name_map_[section_name].default_subsection_position.x, 
							name_map_[section_name].default_subsection_position.y);
					} else
					{
						Logger::ErrorOut() << "Unable to lookup the preloaded data for " << section_name << "\n";
					}
					section->RegisterMetadata();
					SectionTypes::RegisterXMLSectionType(section_name);
				} else
				{
					Logger::ErrorOut() << "Unable to preload " << section_name << ". Scripts relying on it may fail\n";
				}
				
				
				delete section;
				total_count++;
			}
		}
	}
	Logger::DiagnosticOut() << "Preloaded " <<success_count << "/" << total_count << " successfully\n";
}
