#pragma once
#include "Decoration.h"

class LabelDecoration :
	public Decoration
{
protected:
	virtual void InitialiseGraphics(){}
	BaseEntity* source_;
	Vector3f source_location_;
	Vector3f screen_position_;
	float full_lifetime_;
public:
	LabelDecoration(BaseEntity* _source, Vector3f _screen_position, float _lifetime);
	virtual ~LabelDecoration(void);

	virtual void Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn);
	virtual void DrawSelf();
	virtual void EndSubscription(Subscriber* _source);
};
