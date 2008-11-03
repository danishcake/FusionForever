#include "StdAfx.h"
#include "Property.h"
#include "Section.h"

Property::Property(Section* _section, void (*_setter)(Section* _section, float _value), 
									  float (*_getter)(Section* _section),
									  std::string _description)
: setter_(_setter), getter_(_getter), section_(_section), description_(_description)
{
	_section->AddSubscriber(this);
}

Property::~Property(void)
{
	if(section_)
		section_->RemoveSubscriber(this);
}

float Property::Get()
{
	if(getter_)
		return getter_(section_);
	else
		return 0;
}

void Property::Set(float _value)
{
	if(setter_)
		setter_(section_, _value);
}

void Property::EndSubscription(Subscriber* _source)
{
	if(_source == section_)
	{
		section_ = NULL;
		getter_ = NULL;
		setter_ = NULL;
	}
}