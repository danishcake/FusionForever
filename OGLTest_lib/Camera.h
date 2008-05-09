#pragma once

/**
  * The Camera class represents the view on the world.
  * It contains a variety of member functions for getting world parameters
  * and can be made to shake. Follows singleton pattern.
  * Uses an orthographic projection
  */
class Camera
{
private:
   /**
     * Creates a Camera. Private constructor for Singleton pattern.
     */
	Camera(void);
   /**
     * The width of the orthographic projection.
     */
	float width_;
   /**
     * The height of the orthographic projection.
     */
	float height_;
   /**
     * The x-axis centre of the orthographic projection.
     */
	float centre_x_;
   /**
     * The y-axis centre of the orthographic projection.
     */
	float centre_y_;
   /**
     * The x-axis focus of the camera. This is generally the position of the player.
     */
	float focus_x_;
   /**
     * The y-axis focus of the camera. This is generally the position of the player.
     */
	float focus_y_;
   /**
     * The aspect ratio of the camera. Used when resizing one dimension to
     * automatically set size of the other.
     */
	float ratio_;
   /**
     * The camera shakes when this is above 0.
     */
	float shake_time_;
   /**
     * The width of the window.
     */
	int window_width_;
   /**
     * The height on the window.
     */
	int window_height_;

public:
	virtual ~Camera(void);
   /**
     * Gets the instance of the Camera.
     * @return A reference to the instance of the Camera.
     */
	static Camera& Instance()
	{
		static Camera instance;
		return instance;
	}
   /**
     * Gets the width of the orthographic projection.
     * @return The width of the orthographic projection
     */
	float GetWidth() {return width_;}
   /**
     * Gets the height of the orthographic projection.
     * @return The height of the orthographic projection
     */
	float GetHeight() {return height_;}
   /**
     * Gets the left edge of the orthographic projection.
     * @return The left edge of the orthographic projection
     */
	float GetLeft() {return centre_x_ - (width_ / 2.0f);}
   /**
     * Gets the right edge of the orthographic projection.
     * @return The right edge of the orthographic projection
     */
	float GetRight() {return centre_x_ + (width_ / 2.0f);}
   /**
     * Gets the top edge of the orthographic projection.
     * @return The top edge of the orthographic projection
     */
	float GetTop() {return centre_y_ - (height_ / 2.0f);}
   /**
     * Gets the bottom edge of the orthographic projection.
     * @return The bottom edge of the orthographic projection
     */
	float GetBottom() {return centre_y_ + (height_ / 2.0f);}
   /**
     * Gets x-axis centre of the orthographic projection.
     * @return The x-axis centre of the orthographic projection
     */
	float GetCentreX() {return centre_x_;}
   /**
     * Gets y-axis centre of the orthographic projection.
     * @return The x-axis centre of the orthographic projection
     */
	float GetCentreY() {return centre_y_;}
   /**
     * Gets x-axis focus of the orthographic projection. This is generally the player position
     * @return The x-axis focus of the orthographic projection.
     */
	float GetFocusX() {return focus_x_;}
   /**
     * Gets y-axis focus of the orthographic projection. This is generally the player position
     * @return The t-axis focus of the orthographic projection.
     */
	float GetFocusY() {return focus_y_;}

   /**
     * Gets width of the game window.
     * @return The width of the game window
     */
	int GetWindowWidth(){return window_width_;}
   /**
     * Gets height of the game window.
     * @return The width of the game window
     */
	int GetWindowHeight(){return window_height_;}

   /**
     * Sets the aspect ratio and window size.
     * @param _window_width The width of the game window.
     * @param _window_height The height of the game window.
     */
	void SetAspectRatio(int _window_width, int _window_height)
	{
		ratio_ = (float)_window_width / (float)_window_height;
		window_height_ = _window_height;
		window_width_ = _window_width;
	}
   /**
     * Sets the point at the centre of the scene.
     * @param _x The x-axis centre of the scene
     * @param _y The y-axis centre of the scene
     */
	void SetCentre(float _x, float _y)
	{
		centre_x_ = _x;
		centre_y_ = _y;
	}
   /**
     * Sets the focus of the camera. Generally the player position.
     * @param _x The x-axis focus.
     * @param _y The y-axis focus.
     */
	void SetFocus(float _x, float _y)
	{
		focus_x_ = _x;
		focus_y_ = _y;
	}
   /**
     * Sets the width of the orthographic projection.
     * @param _width The width of the orthographic projection.
     */
	void SetWidth(float _width)
	{
		width_= _width;
		height_ = width_ / ratio_;
	}
   /**
     * Sets the height of the orthographic projection.
     * @param _height The height of the orthographic projection
     */
	void SetHeight(float _height)
	{
		height_ = _height;
		width_ = height_ * ratio_;
	}
   /**
     * Causes the camera to shake
     */
	void Shake();
	void TickCamera(float _timespan)
	{
		shake_time_ -= _timespan;
	}
	void SetupCamera();

};
