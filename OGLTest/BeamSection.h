#pragma once
#include "vmath.h"
#include "vmath-collisions.h"
#include <boost/shared_ptr.hpp>
#include "Decoration.h"
#include "Core.h"
#include "BaseEntity.h"


class BeamSection;
typedef boost::shared_ptr<BeamSection> BeamSection_ptr;

class BeamSection : public BaseEntity
{
protected:
	float max_distance_;
	float damage_per_second_;
	float distance_;
	float deco_cooldown_;	
public:
	BeamSection(void);
	virtual ~BeamSection(void);
	virtual void DrawSelf() = 0;
	virtual void Tick(float _timespan, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
};
