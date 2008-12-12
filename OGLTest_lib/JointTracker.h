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

	virtual void EndSubscription(Subscriber* _source);
public:
	JointTracker();
	virtual ~JointTracker(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);
	/**
	  * Sets the SectionType
	  */
	virtual void ToXML(TiXmlElement* _node);
	virtual void GetProperties(std::vector<Property*>& _properties);
	void SetOnlyWhenFiring(int _value){only_when_firing_ = _value != 0;}
	int GetOnlyWhenFiring(){return only_when_firing_ ? 1 : 0;}
	/*
	 * To be overriden by call sub classes that require special parameters.
	 * @return False on missing required elements.
	 */
	virtual bool ParseSpecific(TiXmlElement* _node);
};
