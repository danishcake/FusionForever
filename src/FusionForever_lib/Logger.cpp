#include "StdAfx.h"
#include "Logger.h"

std::string Logger::prefix_;

Logger::Logger(std::string _filename)
{
	std::string filename = prefix_ + _filename;
	output_.open(filename.c_str(), std::ios::trunc);
}

Logger::~Logger(void)
{
	output_.close();
}

