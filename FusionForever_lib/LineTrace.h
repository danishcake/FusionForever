#pragma once
#include "Decoration.h"

#define MAX_LINE_SECTIONS 100
#define LINE_SECTION_TIME 0.05f

class LineTrace :
	public Decoration
{
protected:
	Vector3f line_trace_[MAX_LINE_SECTIONS+1];
	int front_index_;
	virtual void InitialiseGraphics(){}
	BaseEntity* source_;
	bool first_run_;
public:
	LineTrace(BaseEntity* _source, GLColor trail_color_);
	virtual ~LineTrace(void);

	virtual void Tick(float _timespan, Matrix4f _transform);
	virtual void DrawSelf();
	virtual void EndSubscription(Subscriber* _source);
};
