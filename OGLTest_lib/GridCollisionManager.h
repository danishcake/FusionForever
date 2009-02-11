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
	std::vector<Section_ptr>& GetAtCoordinate(int _x, int _y){return sections_[_x][_y];}
	std::vector<Section_ptr>& GetExactAtCoordinate(int _x, int _y){return sections_exact_[_x][_y];}
	virtual void Render();
	static void ClearStatic();
private:
	std::vector<Section_ptr> sections_[GRID_SECTIONS][GRID_SECTIONS];
	std::vector<Section_ptr> sections_exact_[GRID_SECTIONS][GRID_SECTIONS];
	static float left_edge_;
	static float width_;
	static float bottom_edge_;
	static float height_;
	static float grid_width_;
	static float grid_height_;

	static unsigned int left_edge_count_;
	static unsigned int right_edge_count_;
	static unsigned int top_edge_count_;
	static unsigned int bottom_edge_count_;

	std::vector<Section_ptr>* adjacency_lookup_[GRID_SECTIONS][GRID_SECTIONS][9];
	int adjacency_lookup_count_[GRID_SECTIONS][GRID_SECTIONS];
};
