#include "StdAfx.h"
#include "CEGUIMessageLog.h"
#include <algorithm>

struct isLTEZero: public std::unary_function<LogItem, bool>
{
	CEGUI::Window* root_;
	isLTEZero(CEGUI::Window* _root)
	{
		root_ = _root;
	}

	bool operator() (LogItem &rhs) 
	{
		if(rhs.time <= 0)
		{
			root_->removeChildWindow(rhs.window);
			//delete rhs.window;
			//rhs.window = NULL;
			return true;
		}
		else
			return false;
	}
};

LogItem::LogItem(std::string _text, float _time)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	time = _time;
	window = (CEGUI::DefaultWindow*)wmgr.createWindow( "TaharezLook/StaticText");
	window->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1f, 0),CEGUI::UDim(0.4f, 0)));
	window->setSize(CEGUI::UVector2(CEGUI::UDim(0.8f, 0),CEGUI::UDim(0.2f, 0)));
	window->setProperty("FrameEnabled", "False");
	window->setText(_text);
	window->setProperty("VertFormatting", "VertCentred");
	window->setProperty("HorzFormatting", "WordWrapCentred");
	window->setProperty("BackgroundEnabled", "False");
	window->setAlpha(1);
}

LogItem::~LogItem()
{
	
}

CEGUIMessageLog::CEGUIMessageLog()
: root_(NULL)
{
}

CEGUIMessageLog::CEGUIMessageLog(CEGUI::Window* _root)
: root_(_root)
{
}

CEGUIMessageLog::~CEGUIMessageLog(void)
{
	BOOST_FOREACH(LogItem& li, log_items_)
	{
		li.time = -1;
	}
	//TODO check this doesn't leak
	//log_items_.erase(std::remove_if(log_items_.begin(), log_items_.end(), isLTEZero(root_)),log_items_.end());
}


void CEGUIMessageLog::AddMessage(ScreenText _message)
{
	LogItem li = LogItem(_message.text, _message.time);
	root_->addChildWindow(li.window);
	log_items_.push_back(li);
}

void CEGUIMessageLog::Tick(float _timespan)
{
	//Tick gui messages
	int window_position = static_cast<int>(log_items_.size());
	BOOST_FOREACH(LogItem& li, log_items_)
	{
		li.time -= _timespan;
		float y_scale = 0.4f - (window_position * 0.04f);
		float y_actual_scale = li.window->getPosition().d_y.d_scale;
		float y_new = (y_scale + 10.0f * y_actual_scale) / 11.0f;
		float alpha = li.time < 0.5f ? li.time / 0.5f : 1.0f;
		li.window->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1f, 0), CEGUI::UDim(y_new, 0)));
		li.window->setAlpha(alpha);
		window_position--;
	}
	//Remove from list and free, and remove from parent if time < 0
	log_items_.erase(std::remove_if(log_items_.begin(), log_items_.end(), isLTEZero(root_)),log_items_.end());
}
