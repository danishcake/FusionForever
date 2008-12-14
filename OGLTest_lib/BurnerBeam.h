#pragma once
#include "BeamSection.h"

class BurnerBeam :
	public BeamSection
{
public:
	BurnerBeam(void);
	~BurnerBeam(void);

	virtual void DrawSelf();
	virtual void InitialiseGraphics(){}
};
