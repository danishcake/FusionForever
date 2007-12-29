#include "StdAfx.h"
#include "Camera.h"

Camera::Camera(void)
{
	ratio_ = 1.0f;
	width_ = 200.0f;
	height_ = 200.0f;
	centre_x_ = 0.0f;
	centre_y_ = 0.0f;
	shake_time_ = 0.0f;
	window_width_ = 200;
	window_height_ = 200;
}

Camera::~Camera(void)
{
}

void Camera::SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(shake_time_ > 0)
	{
		glRotatef((20 * shake_time_) * (((float)rand() - ((float)RAND_MAX/2))  / (float)RAND_MAX), 0, 0, 1);
	}
	glOrtho(GetLeft(), GetRight(), GetTop(), GetBottom(), -100, 100);

}

void Camera::Shake()
{
	shake_time_ = 1.0f;
}
