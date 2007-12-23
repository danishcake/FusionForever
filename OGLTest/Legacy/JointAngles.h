#pragma once
#include "section.h"

class JointAngles :
	public Section
{
protected:
	float first_angle_;
	float second_angle_;
	float transition_time_;
	float pause_time_;

public:
	JointAngles(float _first_angle, float _second_angle, float _transition_time, float _pause_time);
	virtual ~JointAngles(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
};
