#pragma once
#include "section.h"
#include "BaseAI.h"
#include "AIAction.h"

#define CORE_ROT_RATE_MAX 90
#define CORE_MOVE_RATE_MAX 30

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
