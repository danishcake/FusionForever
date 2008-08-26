#pragma once
#include "Decoration.h"

class BeamCharge :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	
	float scale_;
public:
	BeamCharge(void);
	virtual ~BeamCharge(void);

	void SetScale(float _scale);
	virtual void Tick(float _timespan, Matrix4f _transform);
};
