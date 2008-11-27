#ifndef GLCOLOR_H
#define GLCOLOR_H

struct GLColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
	GLColor(unsigned char _r, unsigned char _g, unsigned char _b)
	{
		r = _r;
		g = _g;
		b = _b;
		a = 255;
	}
	GLColor()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 255;
	}
	GLColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	GLColor(unsigned char _r, unsigned char _g, unsigned char _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = (unsigned char)(255.0f * _a);
	}

	GLColor GetFaded(float _fade_amount)
	{
		return GLColor((unsigned char)(r * _fade_amount), (unsigned char)(g * _fade_amount), (unsigned char)(b * _fade_amount));
	}
};
#endif
