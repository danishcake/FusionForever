#pragma once
#include "Decoration.h"

class Sparks :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
public:
	Sparks(void);
	virtual ~Sparks(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
};
