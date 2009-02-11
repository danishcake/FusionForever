#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "vmath.h"

class Logger
{
private:
	Logger(std::string _filename);
	std::ofstream output_;

public:
	static Logger& ErrorOut()
	{
		static Logger logger("ErrorLog.txt");
		return logger;
	}

	static Logger& DiagnosticOut()
	{
		static Logger logger("Diagnostic.txt");
		return logger;
	}

	~Logger(void);

	Logger& Logger::operator <<( int i )
	{
		output_ << i;
		printf("%d", i);
		return *this;
	}

	Logger& Logger::operator <<( float i )
	{
		output_ << i;
		printf("%f", i);
		return *this;
	}

	Logger& Logger::operator <<( double i )
	{
		output_ << i;
		printf("%e", i);
		return *this;
	}

	Logger& Logger::operator <<( std::string i )
	{
		output_ << i;
		printf("%s", i.c_str());
		return *this;
	}

	Logger& Logger::operator <<(Vector3f v)
	{
		output_ << "(" << v.x << "," << v.y << "," << v.z << ")";
		printf("(%f,%f,%f)", v.x, v.y, v.z);
		return *this;
	}

	Logger& Logger::operator <<(Vector2f v)
	{
		output_ << "(" << v.x << "," << v.y << ")";
		printf("(%f,%f)", v.x, v.y);
		return *this;
	}
};
