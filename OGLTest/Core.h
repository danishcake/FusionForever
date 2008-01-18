#pragma once
#include "section.h"
#include "AIAction.h"
#include <list>

class BaseAI;

class Core :
	public Section
{
protected:
	BaseAI* AI_;
public:
	Core(BaseAI*);
	virtual ~Core(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj,
		              std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform,
					  std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies);
};


class BaseAI
{
public:
	BaseAI(void) {};
	virtual ~BaseAI(void) {};

	virtual AIAction Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies, Core* _self) = 0;
};
