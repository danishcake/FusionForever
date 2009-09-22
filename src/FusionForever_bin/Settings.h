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
};
