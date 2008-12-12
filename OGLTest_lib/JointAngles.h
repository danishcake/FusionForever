#pragma once
#include "Section.h"

class JointAngles :
	public Section
{
protected:
	virtual void InitialiseGraphics();

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
	JointAngles();
	virtual ~JointAngles(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);

	float GetFirstAngle(){return first_angle_;}
	float GetSecondAngle(){return second_angle_;}
	float GetTransitionTime(){return transition_time_;}
	float GetPauseTime(){return pause_time_;}
	void SetFirstAngle(float _value){first_angle_ = _value;}
	void SetSecondAngle(float _value){second_angle_ = _value;}
	void SetTransitionTime(float _value){transition_time_ = _value;}
	void SetPauseTime(float _value){pause_time_ = _value;}

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
};
