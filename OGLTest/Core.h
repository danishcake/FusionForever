#pragma once
#include "section.h"
#include "AIAction.h"
#include <list>

#define CORE_ROT_RATE_MAX 200.0f
#define CORE_MOVE_RATE_MAX 100.0f
#define CORE_EXP_BRAKING 1000.0f

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