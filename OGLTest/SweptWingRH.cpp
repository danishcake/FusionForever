#include "StdAfx.h"
#include "SweptWingRH.h"

bool SweptWingRH::initialised_ = false;
int SweptWingRH::outline_dl_ = 0;
int SweptWingRH::outline_verts_index_ = 0;
int SweptWingRH::fill_dl_ = 0;
int SweptWingRH::fill_verts_index_ = 0;

SweptWingRH::SweptWingRH(void)
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

	health_ = 700;
	max_health_ = health_;
	default_sub_section_position_ = Vector3f(5, -10, 0);
}

SweptWingRH::~SweptWingRH(void)
{
}

void SweptWingRH::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void SweptWingRH::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));			//0
	temp_outline->push_back(Vector3f(0, 10, 0));		//1
	temp_outline->push_back(Vector3f(10,-20,0));	//2

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}
