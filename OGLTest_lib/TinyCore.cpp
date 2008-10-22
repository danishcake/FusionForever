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
		InitialiseGraphics();
		initialised_ = true;
	}
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);

	findRadius();

	health_ = FlexFloat(3000, 3000);
	default_sub_section_position_ = Vector3f(0, 5, 0);
	mass_ = 200;
	thrust_ = FlexFloat(120);
}

TinyCore::~TinyCore(void)
{
}


void TinyCore::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-7.5f,0,0));	//0
	temp_outline->push_back(Vector3f(-2.5f,10,0));	//1
	temp_outline->push_back(Vector3f(2.5f,10,0));	//2
	temp_outline->push_back(Vector3f(7.5f,0,0));	//3
	temp_outline->push_back(Vector3f(2.5f,-5,0));	//4
	temp_outline->push_back(Vector3f(-2.5f,-5,0));	//5

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);	

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

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
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void TinyCore::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", "TinyCore");
}
