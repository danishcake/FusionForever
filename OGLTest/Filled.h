#pragma once
#include <vector>
#include "vmath.h"
#include "GLColor.h"
#include <boost/shared_ptr.hpp>
#include "Datastore.h"

class Filled
{
protected:
	boost::shared_ptr<std::vector<Vector3f>> fill_verts_;
	GLColor fill_color_;
	int fill_display_list_;

	void DrawFill();
	static int CreateFillDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts);
	void DrawFillDisplayList();
	virtual void initialise_fill() = 0;


public:
	Filled(void);
	virtual ~Filled(void);
};
