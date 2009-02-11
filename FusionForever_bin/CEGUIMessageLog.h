#pragma once
#include "GameGui.h"
#include <CEGUI.h>

struct LogItem
{
public:
	LogItem(std::string _text, float _time);
	~LogItem();
	float time;
	CEGUI::DefaultWindow* window;
};

class CEGUIMessageLog
{
protected:
	std::vector<LogItem> log_items_;
	CEGUI::Window* root_;
public:
	CEGUIMessageLog(CEGUI::Window* _root);
	CEGUIMessageLog();
	~CEGUIMessageLog(void);
	void AddMessage(ScreenText _message);
	void Tick(float _timespan);
};
