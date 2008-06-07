#include "StdAfx.h"
#include "GridCollisionManager.h"

GridCollisionManager::GridCollisionManager(void)
{
	left_edge_ = -100;
	width_ = 200;
	bottom_edge_ = -100;
	height_ = 100;
	grid_width_ = 200/GRID_SECTIONS;
	grid_height_ = 200/GRID_SECTIONS;
	total_returned_ = 0;
	largest_returned_ = 0;
	total_tracked_ = 0;
}

GridCollisionManager::~GridCollisionManager(void)
{
}

void GridCollisionManager::Clear()
{

/*	Logger::Instance() << "Total returned" << total_returned_ << "\n";
	Logger::Instance() << "Largest returned" << largest_returned_ << "\n";
	Logger::Instance() << "Total tracked" << total_tracked_ << "\n";*/
	for(int x = 0; x < GRID_SECTIONS; x++)
	{
		for(int y= 0; y < GRID_SECTIONS; y++)
		{
			sections_[x][y].clear();
		}
	}
	total_returned_ = 0;
	largest_returned_ = 0;
	total_tracked_ = 0;
}

void GridCollisionManager::Register(Section_ptr _section)
{
	total_tracked_++;
	Vector3f g_position = _section->GetGlobalPosition();
	int x = 0;
	int y = 0;
	x = 1 + (g_position.x - left_edge_) / (GRID_SECTIONS-2);
	x = x < 1 ? 0 : x > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : x;

	y = 1 + (g_position.y - bottom_edge_) / (GRID_SECTIONS-2);
	y = y < 1 ? 0 : y > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : y;

	sections_[x][y].push_back(_section);
}

void GridCollisionManager::GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point)
{
	int x = 0;
	int y = 0;
	x = 1 + (_point.x - left_edge_) / (GRID_SECTIONS-2);
	x = x < 1 ? 0 : x > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : x;

	y = 1 + (_point.y - bottom_edge_) / (GRID_SECTIONS-2);
	y = y < 1 ? 0 : y > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : y;

	_result.insert(_result.begin(), sections_[x][y].begin(), sections_[x][y].end());
	total_returned_ += _result.size();
	if(_result.size() > largest_returned_)
		largest_returned_ = _result.size();
}
