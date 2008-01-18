#include "StdAfx.h"
#include "BaseEntity.h"

static const float DEGTORAD = 3.14159265 / 180.0;

BaseEntity::BaseEntity(void)
{
	position_ = Vector3f();
	velocity_ = Vector3f();
	radius_ = 5.0;
	angle_ = 0.0;

	ltv_transform_ = Matrix4f();
	ltv_position_ = Vector3f();
}

BaseEntity::~BaseEntity(void)
{
}

void BaseEntity::Tick(float _timespan, Matrix4f _transform)
{
	position_ += velocity_ * _timespan;

	Matrix4f translation = Matrix4f::createTranslation(position_.x, position_.y, 0.0f);
	Matrix4f rotation = Matrix4f::createRotationAroundAxis(0,0,-angle_* DEGTORAD);
	_transform = _transform * translation * rotation ;

	ltv_position_ = _transform.getTranslation();
	ltv_transform_ = _transform;
}
BaseEntity* BaseEntity::localTransToGlobal(BaseEntity* _baseEntity)
{
	_baseEntity->SetPosition(ltv_transform_ * _baseEntity->GetPosition());
	_baseEntity->SetVelocity(ltv_transform_ * _baseEntity->GetVelocity() - ltv_position_);
	return _baseEntity;
}