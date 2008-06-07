#include "StdAfx.h"
#include "Logger.h"

Logger::Logger(void)
{
	output_.open("log.txt", std::ios::trunc);
}

Logger::~Logger(void)
{
	output_.close();
}

void Logger::Log_internal(std::string _message)
{
	if(_message[_message.length()-1]!='\n')
	{
		printf("%s\n", _message.c_str());
		output_ << _message.c_str() << '\n';
	}
	else
	{
		printf(_message.c_str());
		output_ << _message.c_str();
	}
}

std::ofstream& Logger::Log_internal_stream()
{
	return output_;
}
