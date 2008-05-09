#pragma once
#include "Decoration.h"

#define MAX_LINE_SECTIONS 20

class LineTrace :
	public Decoration
{
protected:
	Vector3f line_trace_[MAX_LINE_SECTIONS];
	int front_index_;
	virtual void initialise_fill();
	BaseEntity* source_;
	bool first_run_;
public:
	LineTrace(BaseEntity* _source);
	virtual ~LineTrace(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
	virtual void DrawSelf();
	virtual void EndSubscription(BaseEntity* _source);
};
