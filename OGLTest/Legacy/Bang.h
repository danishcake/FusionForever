#pragma once
#include "decoration.h"
#include "GLColor.h"

class Bang :
	public Decoration
{
protected:
	std::vector<Vector3f> bang_verts_;
	GLColor bang_color_;
public:
	Bang(Vector3f position);
	virtual ~Bang(void);
	virtual void DrawSelf();
};
