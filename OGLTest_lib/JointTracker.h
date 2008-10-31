#pragma once
#include "Section.h"

class JointTracker :
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

public:
	JointTracker(bool _only_when_firing);
	virtual ~JointTracker(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);
	/**
	  * Sets the SectionType
	  */
	virtual void ToXML(TiXmlElement* _node);
protected:
	virtual void EndSubscription(Subscriber* _source);
};
