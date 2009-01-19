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

	template <class T>
	Logger& Logger::operator <<( Vector3<T> _vector)
	{
		output_ << "(" << _vector.x << "," << _vector.y << "," << _vector.z << ")";
		printf("%s", i.c_str());
		return *this;
	}

	template <class T>
	Logger& Logger::operator <<( Vector2<T> _vector)
	{
		output_ << "(" << _vector.x << "," << _vector.y << ")";
		printf("%s", i.c_str());
		return *this;
	}
};
