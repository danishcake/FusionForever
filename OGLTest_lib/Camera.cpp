#include "StdAfx.h"
#include "Camera.h"

Camera::Camera(void)
{
	ratio_ = 1.0f;
	width_ = 200.0f;
	desired_width_ = 200.0f;
	height_ = 200.0f;
	centre_x_ = 0.0f;
	centre_y_ = 0.0f;
	shake_time_ = 0.0f;
	window_width_ = 200;
	window_height_ = 200;
	focus_x_ = 0.0f;
	focus_y_ = 0.0f;
}

Camera::~Camera(void)
{
}

void Camera::SetupCamera()
{
	SetZoomWidth(old_width_ + (desired_width_ - old_width_) * pow(((CAMERA_ZOOM_TIME - this->zoom_time_)/CAMERA_ZOOM_TIME),0.6f));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(shake_time_ > 0)
	{
		glTranslatef((0.05f *shake_time_) * Random::RandomFactor(), (0.05f *shake_time_) * Random::RandomFactor(), 0);
		glRotatef((4 * shake_time_) * Random::RandomFactor(), 0, 0, 1);
	}
	glOrtho(GetLeft(), GetRight(), GetTop(), GetBottom(), -100, 100);

}

void Camera::Shake(float _amount)
{
	shake_time_ = max(shake_time_, _amount);
}

Vector3f Camera::ScreenToWorld(Vector3f _screen_position)
{
	Vector3f world_position;
	world_position.x = _screen_position.x / window_width_;
	world_position.y = 1.0f - (_screen_position.y / window_height_);
	world_position.x = GetLeft() + world_position.x * GetWidth();
	world_position.y = GetTop()  + world_position.y * GetHeight();
	return world_position;
}
