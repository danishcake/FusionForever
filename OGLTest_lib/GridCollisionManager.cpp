#include "StdAfx.h"
#include "GridCollisionManager.h"

GridCollisionManager::GridCollisionManager()
{
	left_edge_ = -GRID_SECTIONS * MINIMUM_GRID_SIZE / 2.0f;
	width_ = GRID_SECTIONS * MINIMUM_GRID_SIZE;
	bottom_edge_ = -GRID_SECTIONS * MINIMUM_GRID_SIZE / 2.0f;
	height_ = GRID_SECTIONS * MINIMUM_GRID_SIZE;
	grid_width_ = width_/GRID_SECTIONS;
	grid_height_ = height_/GRID_SECTIONS;
	total_returned_ = 0;
	largest_returned_ = 0;
	total_tracked_ = 0;
	for(int x = 0; x < GRID_SECTIONS; x++)
	{
		for(int y = 0; y < GRID_SECTIONS; y++)
		{
			int added_count = 0;
			//Left
			if(x != 0)
				adjacency_lookup_[x][y][added_count++] = &sections_[x-1][y];
			//Left up
			if(x != 0 && y != 0)
				adjacency_lookup_[x][y][added_count++] = &sections_[x-1][y-1];
			//Left down
			if(x != 0 && y != GRID_SECTIONS - 1)
				adjacency_lookup_[x][y][added_count++] = &sections_[x-1][y+1];

			//Up
			if(y != GRID_SECTIONS - 1)
				adjacency_lookup_[x][y][added_count++] = &sections_[x][y+1];
			//Down
			if(y != 0)
				adjacency_lookup_[x][y][added_count++] = &sections_[x][y-1];

			//Right
			if(x != GRID_SECTIONS-1)
				adjacency_lookup_[x][y][added_count++] = &sections_[x+1][y];
			//Left up
			if(x != GRID_SECTIONS-1 && y != 0)
				adjacency_lookup_[x][y][added_count++] = &sections_[x+1][y - 1];
			//Left down
			if(x != GRID_SECTIONS-1 && y != GRID_SECTIONS - 1)
				adjacency_lookup_[x][y][added_count++] = &sections_[x+1][y+1];

			adjacency_lookup_[x][y][added_count++] = &sections_[x][y];

			adjacency_lookup_count_[x][y] = added_count;
		}
	}
}

GridCollisionManager::~GridCollisionManager(void)
{
}

void GridCollisionManager::Clear()
{
	//Logger::Instance() << "Minimum efficiency" << ((float)largest_returned_ / (float)total_tracked_) << "\n";

	/*
	Resize rules: 
	If any edge has more then 10% then expand thatway by 10%.
	If any edge has zero then move towards centre by shink by half a unit%.
	*/

	if(total_tracked_ > 20)
	{
		if(grid_width_ > MINIMUM_GRID_SIZE)
		{
			if(left_edge_count_ == 0)
			{
				left_edge_ += grid_width_/2.0f;
				grid_width_ = (grid_width_* (GRID_SECTIONS-2) - (grid_width_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
			}
			if(right_edge_count_ == 0)
			{
				grid_width_ = (grid_width_* (GRID_SECTIONS-2) - (grid_width_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
			}
		}
		if(grid_height_ > MINIMUM_GRID_SIZE)
		{
			if(bottom_edge_count_ == 0)
			{
				bottom_edge_ += grid_height_ / 2.0f;
				grid_height_ = (grid_height_* (GRID_SECTIONS-2) - (grid_height_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
			}
			if(top_edge_count_ == 0)
			{
				grid_height_ = (grid_height_* (GRID_SECTIONS-2) - (grid_height_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
			}
		}

		if(left_edge_count_ > total_tracked_ / 10)
		{
			left_edge_ -= grid_width_/2.0f;
			grid_width_ = (grid_width_* (GRID_SECTIONS-2) + (grid_width_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
		}
		if(bottom_edge_count_ > total_tracked_ / 10)
		{
			bottom_edge_ -= grid_height_/2.0f;
			grid_height_ = (grid_height_* (GRID_SECTIONS-2) + (grid_height_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
		}
		if(right_edge_count_ > total_tracked_ / 10)
		{
			grid_width_ = (grid_width_* (GRID_SECTIONS-2) + (grid_width_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
		}
		if(top_edge_count_ > total_tracked_ / 10)
		{
			grid_height_ = (grid_height_* (GRID_SECTIONS-2) + (grid_height_/2.0f)) / ((float)GRID_SECTIONS-2.0f);
		}
	}
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
	left_edge_count_ = 0;
	right_edge_count_ = 0;
	top_edge_count_ = 0;
	bottom_edge_count_ = 0;

}

void GridCollisionManager::Register(Section_ptr _section)
{
	total_tracked_++;
	Vector3f g_position = _section->GetGlobalPosition();
	int x = 0;
	int y = 0;
	x = (int)(1.0f + (g_position.x - left_edge_) / grid_width_);
	x = x < 1 ? 0 : x > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : x;

	y = (int)(1.0f + (g_position.y - bottom_edge_) / grid_height_);
	y = y < 1 ? 0 : y > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : y;

	sections_[x][y].push_back(_section);
	left_edge_count_ += (x == 0);
	right_edge_count_ += (x == (GRID_SECTIONS-1));
	top_edge_count_ += (y == (GRID_SECTIONS-1));
	bottom_edge_count_ += (y == 0);
}

void GridCollisionManager::GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point)
{
	int x = 0;
	int y = 0;
	x = (int)(1.0f + (_point.x - left_edge_) / grid_width_);
	x = x < 1 ? 0 : x > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : x;

	y = (int)(1.0f + (_point.y - bottom_edge_) / grid_height_);
	y = y < 1 ? 0 : y > GRID_SECTIONS - 2 ? GRID_SECTIONS-1 : y;

	for(int i = 0; i < adjacency_lookup_count_[x][y]; i++)
	{
		_result.insert(_result.end(), adjacency_lookup_[x][y][i]->begin(), adjacency_lookup_[x][y][i]->end());
	}

	total_returned_ += _result.size();
	if(_result.size() > largest_returned_)
		largest_returned_ = _result.size();
}

void GridCollisionManager::Render()
{
	glPushMatrix();
	glColor3ub(255, 255, 255);
	glLoadIdentity();
	glBegin(GL_LINES);
	for(int x = 1; x < GRID_SECTIONS; x++)
	{
		glVertex3f(left_edge_ + x * grid_width_, bottom_edge_ + GRID_SECTIONS * grid_height_, 0);
		glVertex3f(left_edge_ + x * grid_width_, bottom_edge_,                                0);
	}
	for(int y = 1; y < GRID_SECTIONS; y++)
	{
		glVertex3f(left_edge_,                            bottom_edge_ + y *grid_height_, 0);
		glVertex3f(left_edge_+ GRID_SECTIONS*grid_width_, bottom_edge_ + y *grid_height_, 0);
	}
	glEnd();
	glPopMatrix();
}