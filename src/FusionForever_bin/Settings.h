#pragma once
#include <TinyXML.h>
#include "vmath.h"

class Settings
{
private:
	TiXmlDocument settings_doc_;
	Settings(void);

	Vector2<int> resolution_;
	bool fullscreen_;
	float camera_zoom_;
	bool camera_smoothed_;
public:
	~Settings(void);

	static Settings& Instance()
	{
		static Settings instance;
		return instance;
	}

	Vector2<int> GetResolution(){return resolution_;}
	void SetResolution(Vector2<int> _resolution){resolution_ = _resolution;}

	bool GetFullscreen(){return fullscreen_;}
	void SetFullscreen(bool _fullscreen){fullscreen_ =_fullscreen;}

	float GetCameraZoom(){return camera_zoom_;}
	void SetCameraZoom(float _camera_zoom){camera_zoom_ = _camera_zoom;}

	bool GetCameraSmoothed(){return camera_smoothed_;}
	void SetCameraSmoothed(bool _camera_smoothed){camera_smoothed_ = _camera_smoothed;}
};
