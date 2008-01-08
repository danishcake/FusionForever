#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>

class BaseScene
{
public:
	BaseScene(void);
public:
	virtual ~BaseScene(void);
	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes) = 0;
	virtual void Draw() = 0;
	virtual bool IsRoot() = 0;
	virtual bool IsRemovable() = 0;
};
