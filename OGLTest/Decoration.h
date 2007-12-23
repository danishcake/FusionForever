#pragma once
#include "BaseEntity.h"
#include "Filled.h"

class Decoration :
	public BaseEntity, public Filled
{
protected:
	float lifetime_;
public:
	Decoration(void);
	virtual ~Decoration(void);
	
	virtual void Tick(float _timespan, Matrix4f _transform);
	void DrawSelf();
	static bool IsRemovable(boost::shared_ptr<Decoration> dec);

	float GetLifetime(){return lifetime_;}
};
