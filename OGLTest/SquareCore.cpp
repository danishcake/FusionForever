#include "StdAfx.h"
#include "SquareCore.h"

bool SquareCore::initialised_ = false;
int SquareCore::outline_dl_ = 0;
int SquareCore::outline_verts_index_ = 0;
int SquareCore::fill_dl_ = 0;
int SquareCore::fill_verts_index_ = 0;

SquareCore::SquareCore(BaseAI* _AI)
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

	health_ = 5000;
}

SquareCore::~SquareCore(void)
{
}

void SquareCore::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void SquareCore::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-10,-10,0));	//0
	temp_outline->push_back(Vector3f(-10,10,0));	//1
	temp_outline->push_back(Vector3f(10,10,0));		//2
	temp_outline->push_back(Vector3f(10,-10,0));//3

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}