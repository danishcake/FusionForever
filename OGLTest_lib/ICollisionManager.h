#pragma once
#include "Section.h"
#include <vector>


class ICollisionManager
{
public:
	virtual ~ICollisionManager(void){}

	virtual void Clear() = 0;
	virtual void Register(Section_ptr _section) = 0;
	virtual void GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point) = 0;
	virtual void Render() = 0;

protected: 
	int total_returned_;
	int largest_returned_;
	int total_tracked_;
};
