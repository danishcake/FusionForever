#pragma once
#include "section.h"

class RigidArm :
	public Section
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int rigidarm_outline_display_list_;
	static int outline_verts_index_;
	static int rigidarm_fill_display_list_;
	static int fill_verts_index_;

public:
	RigidArm(void);
	virtual ~RigidArm(void);	
};
