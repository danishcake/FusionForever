#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

class Logger
{
private:
	Logger(void);

	void Log_internal(std::string _message);
	std::ofstream& Log_internal_stream();
	std::ofstream output_;

public:
	static Logger& Instance()
	{
		static Logger logger;
		return logger;
	}
	~Logger(void);

	Logger& Logger::operator <<( int i )
	{
		Logger::Instance().Log_internal_stream() << i;
		printf("%d", i);
		return Logger::Instance();
	}

	Logger& Logger::operator <<( float i )
	{
		Logger::Instance().Log_internal_stream() << i;
		printf("%f", i);
		return Logger::Instance();
	}

	Logger& Logger::operator <<( double i )
	{
		Logger::Instance().Log_internal_stream() << i;
		printf("%e", i);
		return Logger::Instance();
	}

	Logger& Logger::operator <<( std::string i )
	{
		Logger::Instance().Log_internal_stream() << i;
		printf("%s", i.c_str());
		return Logger::Instance();
	}


	static void LogError(std::string _message)
	{//To screen and log file
		Logger::Instance().Log_internal(_message);
	}

	static void Log(std::string _message)
	{//To log file only
		Logger::Instance().Log_internal(_message);
	}
};
