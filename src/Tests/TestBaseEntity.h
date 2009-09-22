#pragma once
#include <list>
#include "BaseEntity.h"


class TestBaseEntity : 
	public BaseEntity
{
public:
	TestBaseEntity(void);
	virtual ~TestBaseEntity(void);
	virtual void InitialiseGraphics() {}
};
