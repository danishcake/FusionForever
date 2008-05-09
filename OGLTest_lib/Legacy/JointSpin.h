#pragma once
#include "section.h"
#include "math.h"


//Spin speeds are in degrees per second
#define FAST_SPIN_SPEED 360.0f
#define SLOW_SPIN_SPEED 80.0f



enum JointSpinStyle
{
	Static,
	FastCCW,
	FastCW,
	SlowCCW,
	SlowCW,
	FastReverse,
	SlowReverse
};

class JointSpin :
	public Section
{
protected:
	JointSpinStyle style_;
public:
	JointSpin(JointSpinStyle _style);
	virtual ~JointSpin(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
};
