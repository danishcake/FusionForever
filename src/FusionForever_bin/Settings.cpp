#include "StdAfx.h"
#include "Settings.h"

Settings::Settings(void)
{ 
	resolution_ = Vector2<int>(800, 600);
	fullscreen_ = false;

	settings_doc_.LoadFile("Settings.xml");
	if(settings_doc_.FirstChild() == NULL)
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		settings_doc_.LinkEndChild(declaration);
	}
	TiXmlHandle settings_handle =  TiXmlHandle(&settings_doc_);
	TiXmlElement* resolution = settings_handle.FirstChild("Resolution").Element();
	if(resolution)
	{
		int width;
		int height;
		if(resolution->QueryValueAttribute<int>("Width", &width) == TIXML_SUCCESS)
			resolution_.x = width;
		else
		{
			resolution->SetAttribute("Width", "800");
			resolution_.x = 800;
		}
		if(resolution->QueryValueAttribute<int>("Height", &height) == TIXML_SUCCESS)
			resolution_.y = height;
		else
		{
			resolution->SetAttribute("Height", "600");
			resolution_.y = 600;
		}
	} else
	{
		TiXmlElement* res = new TiXmlElement("Resolution");
		res->SetAttribute("Width", "800");
		res->SetAttribute("Height", "600");
		resolution_.x = 800;
		resolution_.y = 600;
		settings_doc_.LinkEndChild(res);
	}

	TiXmlElement* fullscreen = settings_handle.FirstChild("Fullscreen").Element();
	if(fullscreen)
	{
		int is_fullscreen;
		if(fullscreen->QueryValueAttribute<int>("Value", &is_fullscreen) == TIXML_SUCCESS)
			fullscreen_ = is_fullscreen != 0;
		else
		{
			fullscreen->SetAttribute("Value", "0");
			fullscreen_ = false;
		}
	} else
	{
		TiXmlElement* fs = new TiXmlElement("Fullscreen");
		fs->SetAttribute("Value", "0");
		fullscreen_ = false;
		settings_doc_.LinkEndChild(fs);
	}
	
	TiXmlElement* camerasmoothed = settings_handle.FirstChild("CamSmoothed").Element();
	if(camerasmoothed)
	{
		int is_smoothed;
		if(camerasmoothed->QueryValueAttribute<int>("Value", &is_smoothed) == TIXML_SUCCESS)
			camera_smoothed_ = is_smoothed != 0;
		else
		{
			camerasmoothed->SetAttribute("Value", "1");
			camera_smoothed_ = true;
		}
	} else
	{
		TiXmlElement* missing = new TiXmlElement("CamSmoothed");
		missing->SetAttribute("Value", "1");
		camera_smoothed_ = true;
		settings_doc_.LinkEndChild(missing);
	}
	
	TiXmlElement* camerazoom = settings_handle.FirstChild("CamZoom").Element();
	if(camerazoom)
	{
		float zoom;
		if(camerazoom->QueryValueAttribute<float>("Value", &zoom) == TIXML_SUCCESS)
			camera_zoom_ = zoom;
		else
		{
			camerazoom->SetAttribute("Value", "0.800");
			camera_zoom_ = 0.8f;
		}
	} else
	{
		TiXmlElement* missing = new TiXmlElement("CamZoom");
		missing->SetAttribute("Value", "0.800");
		camera_zoom_ = 0.8f;
		settings_doc_.LinkEndChild(missing);
	}
}

Settings::~Settings(void)
{
	TiXmlHandle settings_handle =  TiXmlHandle(&settings_doc_);

	TiXmlElement* resolution = settings_handle.FirstChild("Resolution").Element();
	resolution->SetAttribute("Width", boost::lexical_cast<std::string, int>(static_cast<int>(resolution_.x)));
	resolution->SetAttribute("Height", boost::lexical_cast<std::string, int>(static_cast<int>(resolution_.y)));

	TiXmlElement* fullscreen = settings_handle.FirstChild("Fullscreen").Element();
	fullscreen->SetAttribute("Value", fullscreen_ ? "1" : "0");

	settings_doc_.SaveFile("Settings.xml");
}
