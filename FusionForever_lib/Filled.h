#pragma once
#include "GLColor.h"
#include "Datastore.h"

class Filled
{
protected:
	boost::shared_ptr<std::vector<Vector3f>> fill_verts_;
	GLColor fill_color_;
	int fill_display_list_;

public:
	Filled(void);
	virtual ~Filled(void);
	
	void DrawFill();

	static int CreateFillDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts);
	void DrawFillDisplayList();
	void SetDisplayList(int _display_list) {fill_display_list_ = _display_list;}

	GLColor& GetFillColor() {return fill_color_;}
	void SetFillColor(GLColor _fill_color) {fill_color_ = _fill_color;}


	boost::shared_ptr<std::vector<Vector3f>>& GetFillVerts() {return fill_verts_;}
};
