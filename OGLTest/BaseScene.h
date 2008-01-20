#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>

class BaseScene;
typedef boost::shared_ptr<BaseScene> BaseScene_ptr;

class BaseScene
{
public:
	BaseScene(void);
public:
	virtual ~BaseScene(void);
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes) = 0;
	virtual void Draw() = 0;
	virtual bool IsRoot() = 0;
	virtual bool IsRemovable() = 0;
};
