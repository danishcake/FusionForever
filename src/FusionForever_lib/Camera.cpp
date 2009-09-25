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
	centre_x_target_ = 0.0f;
	centre_y_target_ = 0.0f;
	centre_x_velocity_ = 0.0f;
	centre_y_velocity_ = 0.0f;
	shake_time_ = 0.0f;
	window_width_ = 200;
	window_height_ = 200;
	focus_x_ = 0.0f;
	focus_y_ = 0.0f;
	camera_smoothed_ = true;
}

Camera::~Camera(void)
{
}

void Camera::TickCamera(float _timespan)
{
	shake_time_ -= _timespan;
	zoom_time_ -= _timespan;
	if(zoom_time_ < 0)
		zoom_time_ = 0;
	
	if(!camera_smoothed_)
	{
		centre_x_ = centre_x_target_;
		centre_y_ = centre_y_target_;
	}

	/* Smooth the motion of the camera */
	const float camera_gain = 1.0f;
	const float velocity_limit = 10.0f;

	float centre_dx = centre_x_target_ - centre_x_;
	float centre_dy = centre_y_target_ - centre_y_;
	
	centre_x_velocity_ += centre_dx * fabs(centre_dx) * camera_gain * _timespan;
	centre_y_velocity_ += centre_dy * fabs(centre_dy) * camera_gain * _timespan;


	//Limit velocity so that it doesn't get too fast, but doesn't reduce below a certain threshold
	if(fabs(centre_x_velocity_) > fabs(centre_dx) * velocity_limit && fabs(centre_x_velocity_) > 5)
		centre_x_velocity_ = centre_x_velocity_ * fabs(centre_dx) * velocity_limit / fabs(centre_x_velocity_);
	if(fabs(centre_y_velocity_) > fabs(centre_dy) * velocity_limit && fabs(centre_y_velocity_) > 5)
		centre_y_velocity_ = centre_y_velocity_ * fabs(centre_dy) * velocity_limit / fabs(centre_y_velocity_);


	centre_x_ += centre_x_velocity_ * _timespan;
	centre_y_ += centre_y_velocity_ * _timespan;

	float centre_dx2 = centre_x_target_ - centre_x_;
	float centre_dy2 = centre_y_target_ - centre_y_;

	//If overshot then clamp to correct position
	if(centre_dx2 * centre_dx < 0)
	{
		centre_x_ = centre_x_target_;
		centre_x_velocity_ = 0;
	}
	if(centre_dy2 * centre_dy < 0)
	{
		centre_y_ = centre_y_target_;
		centre_y_velocity_ = 0;
	}
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
	camera_level_ = CameraLevel::None;
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

Vector3f Camera::ScreenDeltaToWorldDelta(Vector3f _screen_delta)
{
	Vector3f world_move;
	Vector3f result = Vector3f(width_ * _screen_delta.x / window_width_, -height_ * _screen_delta.y / window_height_, 0);
	return result;
}