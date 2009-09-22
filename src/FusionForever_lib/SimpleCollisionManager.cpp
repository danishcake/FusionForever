#include "StdAfx.h"
#include "SimpleCollisionManager.h"

SimpleCollisionManager::SimpleCollisionManager(void)
{
}

SimpleCollisionManager::~SimpleCollisionManager(void)
{
}

void SimpleCollisionManager::Clear()
{
	sections_.clear();
}

void SimpleCollisionManager::Register(Section_ptr _section)
{
	sections_.push_back(_section);
}

void SimpleCollisionManager::GetAtPoint(std::vector<Section_ptr>& _result, Vector3f _point)
{
	_result.insert(_result.begin(), sections_.begin(), sections_.end());
}

void SimpleCollisionManager::Render()
{
}