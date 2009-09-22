#pragma once
#include "Decoration.h"

class Section;

/**
  * When a section dies it should be moved to section decoration
  * When the decoration dies it destroys the section
  */
class SectionDecoration :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();

	/* The dying section */
	Section* section_;

	float full_lifetime_;
	float explosion_timer_;
	float big_explosion_timer_;
	GLColor original_color_;


public:
	SectionDecoration(Section* _section);
	virtual ~SectionDecoration(void);

	virtual void Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn);
	virtual void DrawSelf();
};
