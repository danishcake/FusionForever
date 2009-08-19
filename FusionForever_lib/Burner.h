#pragma once
#include "BeamFiringSection.h"

class Burner :
	public BeamFiringSection
{
protected:
	virtual void InitialiseGraphics();

	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;
public:
	Burner(void);
	virtual ~Burner(void);
};
