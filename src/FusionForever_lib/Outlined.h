#pragma once
#include "GLColor.h"
#include <boost/shared_ptr.hpp>
#include "Datastore.h"

class Outlined
{
private:
	boost::shared_ptr<std::vector<Vector3f>> outline_verts_;
	GLColor outline_color_;
	int outline_display_list_;


	
public:
	Outlined(void);
	virtual ~Outlined(void);

	void DrawOutline();
	static int CreateOutlinedDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts);
	void DrawOutlinedDisplayList();

	GLColor& GetOutlineColor() {return outline_color_;}
	void SetOutlineColor(GLColor _outline_color) {outline_color_ = _outline_color;}
	void SetDisplayList(int _display_list) {outline_display_list_ = _display_list;}

	boost::shared_ptr<std::vector<Vector3f>>& GetOutlineVerts() {return outline_verts_;}
};
