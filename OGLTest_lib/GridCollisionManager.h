#pragma once
#include "ICollisionManager.h"
#include "GLColor.h"

#define GRID_SECTIONS 20
#define MINIMUM_GRID_SIZE 30.0f

class GridCollisionManager :
	public ICollisionManager
{
public:
	GridCollisionManager();
	virtual ~GridCollisionManager(void);

	virtual void Clear();
	virtual void Register(Section_ptr _section);
	virtual void GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point);
	virtual void Render();

private:
	std::vector<Section_ptr> sections_[GRID_SECTIONS][GRID_SECTIONS];
	float left_edge_;
	float width_;
	float bottom_edge_;
	float height_;
	float grid_width_;
	float grid_height_;

	int left_edge_count_;
	int right_edge_count_;
	int top_edge_count_;
	int bottom_edge_count_;

	std::vector<Section_ptr>* adjacency_lookup_[GRID_SECTIONS][GRID_SECTIONS][9];
	int adjacency_lookup_count_[GRID_SECTIONS][GRID_SECTIONS];
};
