#include "StdAfx.h"
#include "FadeOutScene.h"
#include "Camera.h"

#ifdef NOALPHA
const float FadeOutScene::FOTime = 0.2f;
#else
const float FadeOutScene::FOTime = 1.0f;
#endif

FadeOutScene::FadeOutScene(std::vector<BaseScene_ptr> _fadeout_done_scenes)
{
	fadeout_done_scenes_ = _fadeout_done_scenes;
	timeleft_ = FOTime;
}

FadeOutScene::FadeOutScene()
{
	timeleft_ = FOTime;
}

FadeOutScene::~FadeOutScene(void)
{

}

void FadeOutScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	timeleft_-=_timespan;
	if(timeleft_ < 0)
	{
		BOOST_FOREACH(BaseScene_ptr scene, fadeout_done_scenes_)
		{
			_new_scenes.push_back(scene);
		}
	}
}

void FadeOutScene::Draw()
{
	float alpha = 1.0f - (timeleft_/FOTime);
	if(alpha < 0)
		alpha = 0;

#ifdef NOALPHA
	float left = Camera::Instance().GetRight() - alpha * Camera::Instance().GetWidth();
	alpha = 1.0f;

	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(left, Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(left, Camera::Instance().GetBottom(), 0);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetBottom(), 0);
	glVertex3f(left, Camera::Instance().GetBottom(), 0);

	glEnd();
#else
	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetBottom(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glEnd();

#endif
}

bool FadeOutScene::IsRoot()
{
	return false;
}

bool FadeOutScene::IsRemovable()
{
	return timeleft_<0;
}
