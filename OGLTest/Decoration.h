#pragma once
#include "BaseEntity.h"
#include "Filled.h"
#include <boost/shared_ptr.hpp>

class Decoration;
typedef boost::shared_ptr<Decoration> Decoration_ptr;

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
	static bool IsRemovable(Decoration_ptr dec);

	float GetLifetime(){return lifetime_;}
};
