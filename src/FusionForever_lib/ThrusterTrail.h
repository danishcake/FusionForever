#pragma once
#include "Decoration.h"

/*
 * The ThrustTrail class represents the engine plume from an engine. Is is roughly modelled as a string of torsional springs.
 * Each segment turns to be aligned with the one before, and the first is always aligned with the section 'producing' it.
 * As speed increases the turn rate increases, so the plume becomes straight. The intersegment length also increases with
 * speed.
 */
class ThrusterTrail :
	public Decoration
{
protected:
	std::vector<float> points_;
	std::vector<Vector3f> cartesian_points_;
	std::vector<Vector3f> fill_points_;
	virtual void InitialiseGraphics(){}
	BaseEntity* source_;
	bool first_run_;
	Vector3f ltv_source_position_;
	float section_length_;
	float length_scale_;
	float width_scale_;
	Vector3f offset_;
	float angular_offset_;
	float antiparallel_factor_;
	/*
	 * If set then will rotate ship motion before determining plume length.
	 * Effect is to make length as if angle == 0
	 */
	bool angular_corrected_; 
public:
	ThrusterTrail(BaseEntity* _source, float _length_scale, float _width_scale, GLColor _trail_color, float _angle, Vector3f _offset, bool _angular_correction, float _antiparallel_factor);
	virtual ~ThrusterTrail(void);

	virtual void Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn);
	virtual void DrawSelf();
	virtual void EndSubscription(Subscriber* _source);
};
