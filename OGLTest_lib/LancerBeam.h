#pragma once
#include "BeamSection.h"

class LancerBeam :
	public BeamSection
{
public:
	LancerBeam(void);
	~LancerBeam(void);

	virtual void DrawSelf();
	virtual void InitialiseGraphics(){}
};
