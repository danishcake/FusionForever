#include "StdAfx.h"
#include "FadeInScene.h"
#include "Camera.h"

#ifdef NOALPHA
const float FadeInScene::FITime = 0.1f;
#else
const float FadeInScene::FITime = 0.5f;
#endif

FadeInScene::FadeInScene()
{
	timeleft_ = FITime;
}

FadeInScene::~FadeInScene(void)
{
}

void FadeInScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	timeleft_-=_timespan;
}

void FadeInScene::Draw()
{
	float alpha = (timeleft_/FITime);
	if(alpha < 0)
		alpha = 0;

#ifdef NOALPHA
	float right = Camera::Instance().GetLeft() + alpha * Camera::Instance().GetWidth();
	alpha = 1.0f;
	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(right, Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glVertex3f(right, Camera::Instance().GetTop(), 0);
	glVertex3f(right, Camera::Instance().GetBottom(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glEnd();


#else
	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetBottom(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glEnd();
#endif

	
}

bool FadeInScene::IsRoot()
{
	return false;
}

bool FadeInScene::IsRemovable()
{
	return timeleft_<=0;
}
