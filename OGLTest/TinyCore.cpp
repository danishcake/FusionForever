#include "StdAfx.h"
#include "TinyCore.h"

bool TinyCore::initialised_ = false;
int TinyCore::outline_dl_ = 0;
int TinyCore::outline_verts_index_ = 0;
int TinyCore::fill_dl_ = 0;
int TinyCore::fill_verts_index_ = 0;

TinyCore::TinyCore(BaseAI* _AI)
: Core(_AI)
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

	health_ = 3000;
	max_health_ = health_;
	default_sub_section_position_ = Vector3f(0, 5, 0);
}

TinyCore::~TinyCore(void)
{
}

void TinyCore::initialise_fill(void)
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

void TinyCore::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-7.5f,0,0));	//0
	temp_outline->push_back(Vector3f(-2.5f,10,0));	//1
	temp_outline->push_back(Vector3f(2.5f,10,0));		//2
	temp_outline->push_back(Vector3f(7.5f,0,0)); //3
	temp_outline->push_back(Vector3f(2.5f,-5,0)); //4
	temp_outline->push_back(Vector3f(-2.5f,-5,0)); //5

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}