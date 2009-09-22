#pragma once
#include "vmath.h"
#include <vector>

class Billboard;

class GasClouds
{
private:
	Billboard* billboard_;
	std::vector<Billboard*> features_;
public:
	GasClouds(void);
	~GasClouds(void);

	void DrawGasClouds(Vector3f _position);
};
