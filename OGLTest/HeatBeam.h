#pragma once
#include "BeamSection.h"

class HeatBeam :
	public BeamSection
{
public:
	HeatBeam(void);
	~HeatBeam(void);

	virtual void DrawSelf();
	virtual void initialise_outline(){}
	virtual void initialise_fill(){}
};
