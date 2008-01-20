#include "StdAfx.h"
#include "FadeInScene.h"
#include "Camera.h"

#define FADETIME 0.5f

FadeInScene::FadeInScene()
{
	timeleft_ = FADETIME;
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
	float alpha = (timeleft_/FADETIME);
	if(alpha < 0)
		alpha = 0;

	glColor4f(0, 0, 0, alpha);

	glBegin(GL_TRIANGLES);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetTop(), 0);
	
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetTop(), 0);
	glVertex3f(Camera::Instance().GetRight(), Camera::Instance().GetBottom(), 0);
	glVertex3f(Camera::Instance().GetLeft(), Camera::Instance().GetBottom(), 0);

	glEnd();
}

bool FadeInScene::IsRoot()
{
	return false;
}

bool FadeInScene::IsRemovable()
{
	return timeleft_<=0;
}
