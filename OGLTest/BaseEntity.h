#pragma once
#include "vmath.h"
#include <list>
#include <boost/shared_ptr.hpp>

class BaseEntity
{
protected:
	Vector3f position_;
	Vector3f velocity_;
	float radius_;
	float angle_;
  bool position_specified_;   //If this is false then sections can set a default positions

	Matrix4f ltv_transform_;
	Vector3f ltv_position_;

public:
	BaseEntity(void);
	virtual ~BaseEntity(void);
	
	BaseEntity* localTransToGlobal(BaseEntity* _baseEntity);
	virtual void Tick(float _timespan, Matrix4f _transform);

	void SetPosition(Vector3f _position) {position_ = _position; position_specified_ = true;}
	Vector3f GetPosition() {return position_;}
	
	void SetVelocity(Vector3f _velocity) {velocity_ = _velocity;}
	Vector3f GetVelocity() {return velocity_;}
	
	float GetRadius() {return radius_;}

	bool GetPositionSpecified(){return position_specified_;}

	void SetAngle(float _angle) {angle_ = _angle;}
	float GetAngle() {return angle_;}

	float GetGlobalAngle()
	{
		Vector3f v = Vector3f(0,1,0);
		v = ltv_transform_ * v;
		return atan2f(-v.x,v.y)*180.0f/M_PI;
	}
};
