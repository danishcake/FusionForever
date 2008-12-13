#include "StdAfx.h"
#include "ListAdder.h"
#include "Section.h"


std::map<std::string, Section*(*)()>* ListAdder::GetMap()
{
	static std::map<std::string, Section*(*)()>* p_map = NULL;
	if(p_map == NULL)
	{
		p_map = new std::map<std::string, Section*(*)()>();
	}
	return p_map;
}

ListAdder::ListAdder(Section* (*factory_method_)(), std::string _name)
{
	GetMap()->insert(std::pair<std::string, Section*(*)()>(_name, factory_method_));
}

ListAdder::~ListAdder(void)
{
}

Section* ListAdder::GetSection(std::string _name)
{
	std::map<std::string, Section*(*)()>* p_map = GetMap();
	if(p_map->find(_name) != p_map->end())
	{
		return (*p_map)[_name]();
	} else
	{
		return NULL;
	}
}