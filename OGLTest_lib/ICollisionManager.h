#pragma once
#include "Section.h"


class ICollisionManager
{
public:
	virtual ~ICollisionManager(void){}

	virtual void Clear() = 0;
	virtual void Register(Section_ptr _section) = 0;
	virtual void GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point) = 0;
	virtual void Render() = 0;

protected: 
	unsigned total_returned_;
	unsigned int largest_returned_;
	unsigned total_tracked_;
};
