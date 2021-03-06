#pragma once
#include "ICollisionManager.h"

class SimpleCollisionManager :
	public ICollisionManager
{
public:
	SimpleCollisionManager(void);
	virtual ~SimpleCollisionManager(void);

	virtual void Clear();
	virtual void Register(Section_ptr _section);
	virtual void GetAtPoint(std::vector<Section_ptr>& _result, Vector3f);
	virtual void Render();
private:
	std::vector<Section_ptr> sections_;
};
