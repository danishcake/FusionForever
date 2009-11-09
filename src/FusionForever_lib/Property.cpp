#include "StdAfx.h"
#include "Property.h"
#include "Section.h"

Property::Property(Section* _section, void (*_setter_float)(Section* _section, float _value), 
									  float (*_getter_float)(Section* _section),
									  std::string _description)
: setter_float_(_setter_float), getter_float_(_getter_float), section_(_section), description_(_description)
{
	_section->AddSubscriber(this);
	property_type_ = PropertyType::Float;
	getter_enum_ = NULL;
	setter_enum_ = NULL;
}

Property::Property(Section* _section, void (*_setter_enum)(Section* _section, int _value),
									  int (*_getter_enum)(Section* _section),
									  Enumeration _enumeration,
									  std::string _description)
: setter_enum_(_setter_enum), getter_enum_(_getter_enum), enumeration_(_enumeration), section_(_section), description_(_description)
{
	_section->AddSubscriber(this);
	property_type_ = PropertyType::EnumeratedList;
	getter_float_ = NULL;
	setter_float_ = NULL;
}

Property::~Property(void)
{
	if(section_)
		section_->RemoveSubscriber(this);
}

float Property::GetFloat()
{
	if(getter_float_)
		return getter_float_(section_);
	else
		return 0;
}

void Property::SetFloat(float _value)
{
	if(setter_float_)
		setter_float_(section_, _value);
}

void Property::SetEnumerationValue(int _value)
{
	if(setter_enum_)
		setter_enum_(section_, _value);
}

int Property::GetEnumerationIndex()
{
	int enum_value = 0;
	if(getter_enum_)
		enum_value = getter_enum_(section_);
	return enum_value;
}

std::string Property::GetEnumerationItem()
{
	int enum_value = 0;
	if(getter_enum_)
		enum_value = getter_enum_(section_);
	else
		return std::string("No getter for property");
	if(enumeration_.find(enum_value) != enumeration_.end())
		return enumeration_[enum_value];
	else
		return std::string("Enumeration does not define value") + boost::lexical_cast<std::string, int>(enum_value);
}

void Property::EndSubscription(Subscriber* _source)
{
	if(_source == section_)
	{
		section_ = NULL;
		getter_float_ = NULL;
		setter_float_ = NULL;
	}
}