#pragma once
#include "Subscriber.h"
#include <string>

class Section;


class Property : Subscriber
{
private:
	void (*setter_)(Section* _section, float _value);
	float (*getter_)(Section* _section);
	Section* section_;
	std::string description_;
public:
	Section* instance_;

	bool HasSetter(){return setter_ != NULL;}
	bool HasGetter(){return getter_ != NULL;}

	float Get();
	void Set(float _value);
	std::string GetDescription(){return description_;}

	Property(Section* _section, void (*_setter)(Section* _section, float _value), float (*_getter)(Section* _section), std::string _description);
	~Property(void);

	virtual void EndSubscription(Subscriber* _source);
};
