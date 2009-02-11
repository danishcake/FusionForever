#pragma once
#include "Decoration.h"

class Ricochet :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	
	bool reversed_;
public:
	Ricochet(float _angle, bool _reversed);
	virtual ~Ricochet(void);

	virtual void Tick(float _timespan, Matrix4f _transform);

	void ResetTimer();
};
