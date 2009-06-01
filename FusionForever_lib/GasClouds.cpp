#include "StdAfx.h"
#include "GasClouds.h"
#include "Billboard.h"
#include "Camera.h"

GasClouds::GasClouds(void)
{
	billboard_ = new Billboard("GasCloud", BillboardType::WorldSpace);
	billboard_->SetSize(Vector2f(2048, 2048));
	billboard_->SetColor(GLColor(255, 255, 255, 0.2f));
}

GasClouds::~GasClouds(void)
{
	delete billboard_;
}

void GasClouds::DrawGasClouds(Vector3f _position)
{
	int x_minus_times = static_cast<int>(((Camera::Instance().GetWidth() / 2.0f) / 2048) + 2) + 1;
	int y_minus_times = static_cast<int>(((Camera::Instance().GetHeight() / 2.0f) / 2048) + 2);
	float left = -2048 * x_minus_times;
	for(int x = 0; x < x_minus_times * 2; x++)
	{
		float top = -2048 * y_minus_times;
		for(int y = 0; y < y_minus_times * 2; y++)
		{
			Vector3f offset = Vector3f(-fmodf(0.75f * _position.x, 2048),
									   -fmodf(0.75f * _position.y, 2048),
									   0);
			billboard_->SetPosition(Vector3f(left, top, 0) + offset + _position);
			billboard_->Draw();
			top += 2048;
		}
		left += 2048;
	}
/*
	for(int x = -1; x < 2; x++)
	{
		for(int y = -1; y < 2; y++)
		{
			Vector3f position = _position * 1.00f + Vector3f(2048 * x, 2048 * y, 0);
			Vector3f offset = Vector3f(-fmodf(0.7f * _position.x, 2048),
									   -fmodf(0.7f * _position.y, 2048),
									   0);
			billboard_->SetPosition(position + offset);
			billboard_->Draw();
		}
	}*/
}