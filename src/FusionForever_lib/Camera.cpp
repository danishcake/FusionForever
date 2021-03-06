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
	centre_x_velocity_ = 0.0f;
	centre_y_velocity_ = 0.0f;
	shake_time_ = 0.0f;
	window_width_ = 200;
	window_height_ = 200;
	focus_x_ = 0.0f;
	focus_y_ = 0.0f;
	camera_smoothed_ = true;
	zoom_factor_ = 0.8f;
}

Camera::~Camera(void)
{
}

void Camera::SetCentreTarget(float _x, float _y, float _px, float _py, CameraLevel::Enum _level)
{
	if(_level > camera_level_)
	{
		target_centres_.clear();
		target_offsets_.clear();
		camera_level_ = _level;
	}
	if(_level == camera_level_)
	{
		target_centres_.push_back(Vector2f(_x, _y));
		target_offsets_.push_back(Vector2f(_px, _py));
	}
}

void Camera::SetCentre(float _x, float _y, CameraLevel::Enum _level)
{
	//Ignore updates with lower importance
	if(camera_level_ <= _level)
	{
		camera_level_ = _level;
		centre_x_ = _x;
		centre_y_ = _y;
		target_centres_.clear();
		target_centres_.push_back(Vector2f(_x, _y));
		target_offsets_.clear();
		centre_x_velocity_ = 0;
		centre_y_velocity_ = 0;
	}
}

void Camera::TickCamera(float _timespan)
{
	shake_time_ -= _timespan;
	zoom_time_ -= _timespan;
	if(zoom_time_ < 0)
		zoom_time_ = 0;
	
	Vector2f top_right;
	top_right.x = -FLT_MAX;
	top_right.y = -FLT_MAX;
	Vector2f bottom_left;
	bottom_left.x = FLT_MAX;
	bottom_left.y = FLT_MAX;
	
	Vector2f target_centre;
	Vector2f target_focus;
	Vector2f target_offset;

	/*
	When multiple target centres present the screen bounds should be found that encompass them all, 
	plus a bit of room around the edge.
	*/

	for(std::vector<Vector2f>::iterator it = target_centres_.begin(); it != target_centres_.end(); ++it)
	{
		if(it->x < bottom_left.x)
			bottom_left.x = it->x;
		if(it->x > top_right.x)
			top_right.x = it->x;
		if(it->y < bottom_left.y)
			bottom_left.y = it->y;
		if(it->y > top_right.y)
			top_right.y = it->y;
	}
	for(std::vector<Vector2f>::iterator it = target_offsets_.begin(); it != target_offsets_.end(); ++it)
	{
		target_offset += *it;
	}
	if(target_centres_.size() > 1)
	{
		focus_x_ = centre_x_;
		focus_y_ = centre_y_;

		target_centre = (bottom_left + top_right) / 2;
		float dw = (top_right.x - bottom_left.x) + 400;
		float dh = (top_right.y - bottom_left.y) + 400;
		dh *= GetAspectRatio();
		float largest = dw > dh ? dw : dh;
		desired_width_ = largest;
	} else if (target_centres_.size() == 1)
	{
		target_centre = top_right + Vector2f(GetWidth() / 2, GetHeight() / 2) * target_offset * zoom_factor_;
	}

	if(!camera_smoothed_)
	{
		centre_x_ = target_centre.x;
		centre_y_ = target_centre.y;
	} else
	{
		/* Smooth the motion of the camera */
		const float camera_gain = 1.0f;
		const float velocity_limit = 10.0f;

		float centre_dx = target_centre.x - centre_x_;
		float centre_dy = target_centre.y - centre_y_;
		
		centre_x_velocity_ += centre_dx * fabs(centre_dx) * camera_gain * _timespan;
		centre_y_velocity_ += centre_dy * fabs(centre_dy) * camera_gain * _timespan;


		//Limit velocity so that it doesn't get too fast, but doesn't reduce below a certain threshold
		if(fabs(centre_x_velocity_) > fabs(centre_dx) * velocity_limit && fabs(centre_x_velocity_) > 5)
			centre_x_velocity_ = centre_x_velocity_ * fabs(centre_dx) * velocity_limit / fabs(centre_x_velocity_);
		if(fabs(centre_y_velocity_) > fabs(centre_dy) * velocity_limit && fabs(centre_y_velocity_) > 5)
			centre_y_velocity_ = centre_y_velocity_ * fabs(centre_dy) * velocity_limit / fabs(centre_y_velocity_);


		centre_x_ += centre_x_velocity_ * _timespan;
		centre_y_ += centre_y_velocity_ * _timespan;

		float centre_dx2 = target_centre.x - centre_x_;
		float centre_dy2 = target_centre.y - centre_y_;

		//If overshot then clamp to correct position
		if(centre_dx2 * centre_dx < 0)
		{
			centre_x_ = target_centre.x;
			centre_x_velocity_ = 0;
		}
		if(centre_dy2 * centre_dy < 0)
		{
			centre_y_ = target_centre.y;
			centre_y_velocity_ = 0;
		}
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