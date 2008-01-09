#include "StdAfx.h"
#include "FadeOutInScene.h"
#include "Camera.h"
#include <boost/foreach.hpp>

#define FADETIME 2.0f

const float FadeOutInScene::FITime = 1.0f;
const float FadeOutInScene::FOTime = 1.0f;

FadeOutInScene::FadeOutInScene(std::vector<boost::shared_ptr<BaseScene>> _fadeout_done_scenes)
{
	fadeout_done_scenes_ = _fadeout_done_scenes;
	faded_out_ = false;
	timeleft_ = FADETIME;
}

FadeOutInScene::~FadeOutInScene(void)
{
}

void FadeOutInScene::Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes)
{
	timeleft_-=_timespan;
	if((timeleft_ < FADETIME / 2)&& !faded_out_)
	{
		BOOST_FOREACH(boost::shared_ptr<BaseScene> scene, fadeout_done_scenes_)
		{
			_new_scenes.push_back(scene);
		}
		faded_out_ = true;
	}
}

void FadeOutInScene::Draw()
{
	float alpha = (timeleft_/FADETIME);
	if(alpha < 0)
		alpha = 0;

	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetBottom(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glEnd();

}

bool FadeOutInScene::IsRoot()
{
	return false;
}

bool FadeOutInScene::IsRemovable()
{
	return timeleft_ < 0;
}
