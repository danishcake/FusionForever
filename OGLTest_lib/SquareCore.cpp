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
		InitialiseGraphics();
		initialised_ = true;
	}
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);

	findRadius();

	health_ = FlexFloat(5000, 5000);
	default_sub_section_position_ = Vector3f(0, 10, 0);
	mass_ = 500;
	thrust_ = FlexFloat(100);
}

SquareCore::~SquareCore(void)
{
}

void SquareCore::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-10,-10,0));	//0
	temp_outline->push_back(Vector3f(-10,10,0));	//1
	temp_outline->push_back(Vector3f(10,10,0));		//2
	temp_outline->push_back(Vector3f(10,-10,0));//3

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);	

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void SquareCore::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", "SquareCore");
}
