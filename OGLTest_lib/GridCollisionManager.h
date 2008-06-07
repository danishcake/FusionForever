#pragma once
#include "ICollisionManager.h"

#define GRID_SECTIONS 20

class GridCollisionManager :
	public ICollisionManager
{
public:
	GridCollisionManager(void);
	virtual ~GridCollisionManager(void);

	virtual void Clear();
	virtual void Register(Section_ptr _section);
	virtual void GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point);

private:
	std::vector<Section_ptr> sections_[GRID_SECTIONS][GRID_SECTIONS];
	float left_edge_;
	float width_;
	float bottom_edge_;
	float height_;
	float grid_width_;
	float grid_height_;
};
