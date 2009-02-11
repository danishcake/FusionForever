#include "StdAfx.h"
#include "Outlined.h"

Outlined::Outlined(void)
{
	outline_color_ = GLColor(255,255,255);
}

Outlined::~Outlined(void)
{
}

void Outlined::DrawOutline(void)
{
	glColor4ub(outline_color_.r, outline_color_.g, outline_color_.b, outline_color_.a);
	glBegin(GL_LINE_LOOP);
	for(unsigned int i = 0; i < outline_verts_->size(); i++)
	{
		glVertex3fv((*outline_verts_)[i]);
	}
	glEnd();
}


void Outlined::DrawOutlinedDisplayList(void)
{
	glColor4ub(outline_color_.r, outline_color_.g, outline_color_.b, outline_color_.a);
	glCallList(outline_display_list_);
}

int Outlined::CreateOutlinedDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts)
{
	int list_id = Datastore::Instance().BeginDisplayList();
	glBegin(GL_LINE_LOOP);
	for(unsigned int i = 0; i < _verts->size(); i++)
	{
		glVertex3fv((*_verts)[i]);
	}
	glEnd();
	Datastore::Instance().EndDisplayList();
	return list_id;
}
