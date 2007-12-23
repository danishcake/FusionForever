#ifndef GLCOLOR_H
#define GLCOLOR_H

struct GLColor
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLColor(GLubyte _r, GLubyte _g, GLubyte _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}
	GLColor()
	{
		r = 0;
		g = 0;
		b = 0;
	}
};
#endif
