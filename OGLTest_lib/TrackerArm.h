#pragma once
#include "Section.h"

class TrackerArm :
	public Section
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

	BaseEntity* target_;
	bool only_when_firing_;
	float angle_range_;
	float angle_centre_;

public:
	TrackerArm(bool _only_when_firing);
	virtual ~TrackerArm(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);
	virtual void SetAngle(float _angle){angle_ = _angle; angle_centre_ = _angle;}
	/**
	  * Sets the SectionType
	  */
	virtual void ToXML(TiXmlElement* _node);
protected:
	virtual void EndSubscription(Subscriber* _source);
};
