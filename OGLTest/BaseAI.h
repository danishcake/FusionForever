#pragma once
#include <boost/shared_ptr.hpp>
#include <list>
#include "AIAction.h"

class Core;

class BaseAI
{
public:
	BaseAI(void) {};
	virtual ~BaseAI(void) {};

	virtual AIAction Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies) = 0;
};
