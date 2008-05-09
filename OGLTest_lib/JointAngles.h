#pragma once
#include "Section.h"

class JointAngles :
	public Section
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

	float first_angle_;
	float second_angle_;
	float transition_time_;
	float pause_time_;
	float total_time_;

public:
	JointAngles(float _first_angle, float _second_angle, float _transition_time, float _pause_time);
	virtual ~JointAngles(void);
	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
};
