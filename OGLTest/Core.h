#pragma once
#include "section.h"
#include "AIAction.h"
#include <list>
#include <boost/shared_ptr.hpp>

class BaseAI;
class Core;
typedef boost::shared_ptr<Core> Core_ptr;

class Core :
	public Section
{
protected:
	BaseAI* AI_;
public:
	Core(BaseAI*);
	virtual ~Core(void);
	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec,
					  Matrix4f _transform, std::list<Core_ptr>& _allies, std::list<Core_ptr>& _enemies);
};


class BaseAI
{
public:
	BaseAI(void) {};
	virtual ~BaseAI(void) {};

	virtual AIAction Tick(float _timespan, std::list<Core_ptr>& _allies, std::list<Core_ptr>& _enemies, Core* _self) = 0;
};
