#pragma once
#include "vmath.h"

class Billboard;

class GasClouds
{
private:
	Billboard* billboard_;
public:
	GasClouds(void);
	~GasClouds(void);

	void DrawGasClouds(Vector3f _position);
};
