#pragma once
#include "Subscriber.h"
#include <string>
#include <vector>
#include <map>

class Section;

namespace PropertyType
{
	enum Enum
	{
		Float, EnumeratedList
	};
};

typedef std::map<int, std::string> Enumeration;

class Property : Subscriber
{
private:
	void (*setter_float_)(Section* _section, float _value);
	float (*getter_float_)(Section* _section);

	void (*setter_enum_)(Section* _section, int _value);
	int (*getter_enum_)(Section* _section);
	Enumeration enumeration_;

	Section* section_;
	std::string description_;

	PropertyType::Enum property_type_;
public:
	Property(Section* _section, void (*_setter_float)(Section* _section, float _value), float (*_getter_float)(Section* _section), std::string _description);
	Property(Section* _section, void (*_setter_enum)(Section* _section, int _value), int (*_getter_enum)(Section* _section), Enumeration _enumeration, std::string _description);
	~Property(void);


	Section* instance_;

	bool HasSetter()
	{
		if(property_type_ == PropertyType::Float)
			return setter_float_ != NULL; 
		else
			return setter_enum_ != NULL;
	}

	bool HasGetter()
	{
		if(property_type_ == PropertyType::Float)
			return getter_float_ != NULL; 
		else
			return getter_enum_ != NULL;
	}

	PropertyType::Enum Type(){return property_type_;}

	float GetFloat();
	void SetFloat(float _value);

	Enumeration GetEnumeration(){return enumeration_;}
	void SetEnumerationValue(int _value);
	std::string GetEnumerationItem();
	int GetEnumerationIndex();

	std::string GetDescription(){return description_;}

	virtual void EndSubscription(Subscriber* _source);
};
