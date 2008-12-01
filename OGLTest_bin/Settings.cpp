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
			resolution->SetAttribute("Width", "800");
		if(resolution->QueryValueAttribute<int>("Height", &height) == TIXML_SUCCESS)
			resolution_.y = height;
		else
			resolution->SetAttribute("Height", "600");
	} else
	{
		TiXmlElement* res = new TiXmlElement("Resolution");
		res->SetAttribute("Width", "800");
		res->SetAttribute("Height", "600");
		settings_doc_.LinkEndChild(res);
	}

	TiXmlElement* fullscreen = settings_handle.FirstChild("Fullscreen").Element();
	if(fullscreen)
	{
		int is_fullscreen;
		if(fullscreen->QueryValueAttribute<int>("Value", &is_fullscreen) == TIXML_SUCCESS)
			fullscreen_ = is_fullscreen != 0;
		else
			fullscreen->SetAttribute("Value", "0");
	} else
	{
		TiXmlElement* fs = new TiXmlElement("Fullscreen");
		fs->SetAttribute("Value", "0");
		settings_doc_.LinkEndChild(fs);
	}

}

Settings::~Settings(void)
{
	settings_doc_.SaveFile("Settings.xml");
}
