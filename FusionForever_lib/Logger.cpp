#include "StdAfx.h"
#include "Logger.h"

Logger::Logger(std::string _filename)
{
	output_.open(_filename.c_str(), std::ios::trunc);
}

Logger::~Logger(void)
{
	output_.close();
}

