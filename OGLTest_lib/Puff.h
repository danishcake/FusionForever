#pragma once
#include "Decoration.h"

class Puff :
	public Decoration
{
protected:
	virtual void initialise_fill();
	
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;

public:
	Puff(void);
	virtual ~Puff(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
};
