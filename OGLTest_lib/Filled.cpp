#include "StdAfx.h"
#include "Filled.h"


Filled::Filled(void)
{
	fill_color_ = GLColor(255,0,0);
}

Filled::~Filled(void)
{
}

void Filled::DrawFill(void)
{
	glColor4b(fill_color_.r,fill_color_.g, fill_color_.b, fill_color_.a);
	glBegin(GL_TRIANGLES);
	for(unsigned int i = 0; i < fill_verts_->size(); i++)
	{
		glVertex3fv((*fill_verts_)[i]);
	}
	glEnd();
}

void Filled::DrawFillDisplayList(void)
{
	glColor4ub(fill_color_.r,fill_color_.g, fill_color_.b, fill_color_.a);
	glCallList(fill_display_list_);
}

int Filled::CreateFillDisplayList(boost::shared_ptr<std::vector<Vector3f>> _verts)
{
	int list_id = Datastore::Instance().BeginDisplayList();
	glBegin(GL_TRIANGLES);
	for(unsigned int i = 0; i < _verts->size(); i++)
	{
	  glVertex3fv((*_verts)[i]);
	}
	glEnd();
	Datastore::Instance().EndDisplayList();
	return list_id;
}
