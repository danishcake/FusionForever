#pragma once
#include "Decoration.h"

class Explosion :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;

public:
	Explosion();
	virtual ~Explosion(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
};
