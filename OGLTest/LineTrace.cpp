#include "StdAfx.h"
#include "LineTrace.h"

LineTrace::LineTrace(void)
{
	for(int i = 0; i < MAX_LINE_SECTIONS; i++)
		line_trace_[i] = Vector3f();
}

LineTrace::~LineTrace(void)
{
}

void LineTrace::Tick(float _timespan, Matrix4f _transform)
{
	Decoration::Tick(_timespan, _transform);

	front_index_++;
	front_index_ %= MAX_LINE_SECTIONS;
	line_trace_[front_index_] = ltv_position_;
}

void LineTrace::DrawSelf()
{
	glColor3b(200, 50, 50);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINES);
	int points_left = MAX_LINE_SECTIONS;
	int index = front_index_;
	while(points_left > 0)
	{
		index++;
		index %= MAX_LINE_SECTIONS;
		glVertex3fv(line_trace_[index]);
		points_left--;
	}
	glEnd();
	glPopMatrix();
}

