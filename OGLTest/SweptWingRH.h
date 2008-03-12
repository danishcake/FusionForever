#pragma once
#include "Section.h"

class SweptWingRH :
	public Section
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

public:
	SweptWingRH(void);
	virtual ~SweptWingRH(void);	
};
