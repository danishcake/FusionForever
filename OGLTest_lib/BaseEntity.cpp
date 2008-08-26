#include "StdAfx.h"
#include "BaseEntity.h"

static const float DEGTORAD = 3.14159265f / 180.0f;

BaseEntity::BaseEntity(void)
{
	position_ = Vector3f();
	velocity_ = Vector3f();
	radius_ = 5.0;
	angle_ = 0.0;

	ltv_transform_ = Matrix4f();
	ltv_position_ = Vector3f();
	prev_position_ = Vector3f();
	position_specified_ = false;
	moment_ = 1;
	mass_ = 1;
	spin_ = 0;
}

BaseEntity::~BaseEntity(void)
{
}

void BaseEntity::Tick(float _timespan, Matrix4f _transform)
{
	spin_ *= expf(-_timespan * 5);
	prev_position_ = position_;
	position_ += velocity_ * _timespan;

	angle_ += spin_ * _timespan;

	Matrix4f translation = Matrix4f::createTranslation(position_.x, position_.y, 0.0f);
	Matrix4f rotation = Matrix4f::createRotationAroundAxis(0, 0, angle_* DEGTORAD);
	_transform = _transform * translation * rotation ;

   /*Store position and transform
    * As _transform is passed by value any overriding Tick methods structured like
      Tick(float _timespan, Matrix4f _transform, ...)
      {
         BaseEntity::Tick(_timespan, _transform);
         //Custom tick stuff
      }
      should ensure they use ltv_transform_, not _transform.
    */
	
	ltv_position_ = _transform.getTranslation();
	ltv_transform_ = _transform;
}

BaseEntity* BaseEntity::localTransToGlobal(BaseEntity* _baseEntity)
{
	_baseEntity->SetPosition(ltv_transform_ * _baseEntity->GetPosition());
	_baseEntity->SetVelocity(ltv_transform_ * _baseEntity->GetVelocity() - ltv_position_);
	return _baseEntity;
}


void BaseEntity::ImpartMomentum(Vector3f _momentum, Vector3f _position)
{
	Vector3f offset = _position - position_;
	float perp_dist = 0;
	float angular_factor = 0;
	
	Vector3f norm_momentum = _momentum;
	if(norm_momentum.lengthSq() > 0)
	{
		norm_momentum.normalize();
		float temp = norm_momentum.x;
		norm_momentum.x = -norm_momentum.y;
		norm_momentum.y = temp;
		perp_dist = offset.dotProduct(norm_momentum);
	}
	if(fabsf(perp_dist) < 200.0f)
		angular_factor = fabsf(perp_dist) / 200.0f;
	else
		angular_factor = 1;
	angular_factor *= 0.5f;
	/*
	* Rather than do complicated proper physics I'll approximate - amount of momentum
	  that goes to angular momentum scales from 0% at (0,0) to 50% at (200,0)
	*/
	velocity_ += _momentum * (1.0f - angular_factor) / mass_;
	spin_ += 250 * _momentum.length() * perp_dist * angular_factor / moment_;
}