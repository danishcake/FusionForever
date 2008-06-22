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
	position_specified_ = false;
}

BaseEntity::~BaseEntity(void)
{
	BOOST_FOREACH(BaseEntity* subscriber, subscribers_)
	{
		subscriber->EndSubscription(this);
	}
}

void BaseEntity::Tick(float _timespan, Matrix4f _transform)
{
	position_ += velocity_ * _timespan;

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

void BaseEntity::AddSubscriber(BaseEntity* _subscriber)
{
	subscribers_.push_back(_subscriber);
}

void BaseEntity::RemoveSubscriber(BaseEntity* _subscriber)
{
	subscribers_.remove(_subscriber);
}

void BaseEntity::EndSubscription(BaseEntity* _source)
{
	//Do nothing, but overloaded versions should check the incoming pointer, and invalidate them if they match
}