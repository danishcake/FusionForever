#pragma once
#include "Decoration.h"

/*
struct AngularVector
{
	float angle;
	float length;
};*/


class ThrusterTrail :
	public Decoration
{
protected:
	std::vector<float> points_;
	std::vector<Vector3f> cartesian_points_;
	virtual void InitialiseGraphics(){}
	BaseEntity* source_;
	bool first_run_;
	Vector3f ltv_source_position_;
	float section_length_;
	float length_scale_;
public:
	ThrusterTrail(BaseEntity* _source, float _length_scale, GLColor trail_color_);
	virtual ~ThrusterTrail(void);

	virtual void Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn);
	virtual void DrawSelf();
	virtual void EndSubscription(Subscriber* _source);
};
