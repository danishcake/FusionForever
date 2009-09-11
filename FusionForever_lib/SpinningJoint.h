#pragma once
#include "Section.h"

class SpinningJoint :
	public Section
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

	float degrees_per_second_;

public:
	SpinningJoint();
	virtual ~SpinningJoint(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);

	float GetSpinRate(){return degrees_per_second_;}
	void SetSpinRate(float _value){degrees_per_second_ = _value;}

	/*
	 * Gets a list of gettable/settable properties for the editor. 
	 * Override and then call to get the basics
	 */
	virtual void GetProperties(std::vector<Property*>& _properties );

	/**
	  * Sets the SectionType
	  */
	virtual void ToXML(TiXmlElement* _node);
	/*
	 * To be overriden by call sub classes that require special parameters.
	 * @return False on missing required elements.
	 */
	virtual bool ParseSpecific(TiXmlElement* _node);

	/*
	 * Register section metadata - section name associated with a number of 
	 * tags, values and coordinates
	 */
	virtual void RegisterMetadata();
};
