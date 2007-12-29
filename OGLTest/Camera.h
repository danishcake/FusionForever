#pragma once

class Camera
{
private:
	Camera(void);
	float width_;
	float height_;
	float centre_x_;
	float centre_y_;
	float ratio_;
	float shake_time_;

	int window_width_;
	int window_height_;

public:
	virtual ~Camera(void);
	static Camera& Instance()
	{
		static Camera instance;
		return instance;
	}
	float GetWidth() {return width_;}
	float GetHeight() {return height_;}
	float GetLeft() {return centre_x_ - (width_ / 2.0f);}
	float GetRight() {return centre_x_ + (width_ / 2.0f);}
	float GetTop() {return centre_y_ - (height_ / 2.0f);}
	float GetBottom() {return centre_y_ + (height_ / 2.0f);}
	float GetCentreX() {return centre_x_;}
	float GetCentreY() {return centre_y_;}

	int GetWindowWidth(){return window_width_;}
	int GetWindowHeight(){return window_height_;}

	void SetAspectRatio(int _window_width, int _window_height) 
	{
		ratio_ = _window_width / _window_height;
		window_height_ = _window_height;
		window_width_ = _window_width;
	}
	void SetCentre(float _x, float _y)
	{
		centre_x_ = _x;
		centre_y_ = _y;
	}
	void SetWidth(float _width)
	{
		width_= _width;
		height_ = width_ / ratio_;
	}
	void SetHeight(float _height)
	{
		height_ = _height;
		width_ = height_ * ratio_;
	}
	
	void Shake();
	void TickCamera(float _timespan)
	{
		shake_time_ -= _timespan;
	}
	void SetupCamera();
	
};
