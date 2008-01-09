#include "StdAfx.h"
#include "IntroScene.h"
#include "FadeOutScene.h"

const float IntroScene::intro_max_time_ = 2.0f;

IntroScene::IntroScene(void)
{
	timeleft_ = intro_max_time_;
}

IntroScene::~IntroScene(void)
{
}

void IntroScene::Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes)
{
	timeleft_ -= _timespan;
	if(timeleft_<0)
	{

	}
}
void IntroScene::Draw()
{

}
bool IntroScene::IsRoot()
{
	return true;
}
bool IntroScene::IsRemovable()
{
	return timeleft_ < -FadeOutScene::FOTime;
}
