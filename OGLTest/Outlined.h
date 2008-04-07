#pragma once
#include <vector>
#include "vmath.h"
#include "GLColor.h"
#include <boost/shared_ptr.hpp>
#include "Datastore.h"

class Outlined
{
protected:
	boost::shared_ptr<std::vector<Vector3f>> outline_verts_;
	GLColor outline_color_;
	int outline_display_list_;

	void DrawOutline();
	static int CreateOutlinedDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts);
	void DrawOutlinedDisplayList();
	virtual void initialise_outline() = 0;
	
public:
	Outlined(void);
	virtual ~Outlined(void);
};
