#pragma once
#include "Decoration.h"

#define MAX_LINE_SECTIONS 100

class LineTrace :
	public Decoration
{
protected:
	Vector3f line_trace_[MAX_LINE_SECTIONS];
	int front_index_;
public:
	LineTrace(void);
	virtual ~LineTrace(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
	virtual void DrawSelf();
};
