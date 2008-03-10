#include "StdAfx.h"
#include "WidePlate.h"

bool WidePlate::initialised_ = false;
int WidePlate::outline_dl_ = 0;
int WidePlate::outline_verts_index_ = 0;
int WidePlate::fill_dl_ = 0;
int WidePlate::fill_verts_index_ = 0;

WidePlate::WidePlate(void)
: Section()
{
	if(!initialised_)
	{
		initialise_outline();
		initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 1600;
	max_health_ = health_;
	default_sub_section_position_ = Vector3f(2.5, 15, 0);
}

WidePlate::~WidePlate(void)
{
}

void WidePlate::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[3]);
	temp_fill->push_back((*temp_outline)[4]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[5]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void WidePlate::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-7.5f, -2.5f, 0));	//0
	temp_outline->push_back(Vector3f(-7.5f, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(-5.0f, 5.0f, 0));		//2
	temp_outline->push_back(Vector3f(5.0f, 5.0f, 0));//3
	temp_outline->push_back(Vector3f(7.5f, 2.5f, 0));		//4
	temp_outline->push_back(Vector3f(7.5f, -2.5f, 0));	//5

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}
