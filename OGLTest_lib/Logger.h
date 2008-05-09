#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

class Logger
{
private:
	Logger(void);
	static Logger& Instance()
	{
		
		static Logger logger;
		return logger;
	}
	void Log_internal(std::string _message);
	std::ofstream output_;

public:
	~Logger(void);
	static void LogError(std::string _message)
	{//To screen and log file
		Logger::Instance().Log_internal(_message);
	}
	static void LogGameMessage(std::string _message)
	{//To screen only
		Logger::Instance().Log_internal(_message);
	}
	static void Log(std::string _message)
	{//To log file only
		Logger::Instance().Log_internal(_message);
	}
};
