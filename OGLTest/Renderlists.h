#pragma once
#include "vmath.h"
#include <map>

class Renderlists
{
private:
	Renderlists(void);
	static Renderlists instance_;
	std::map<std::string, int> render_lists_;
public:
	~Renderlists(void);
	static Renderlists& Instance() {return instance_;}
	int BeginRenderlist(std::string _name);
	void EndRenderlist();
	
};