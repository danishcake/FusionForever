#include "StdAfx.h"
#include "Burner.h"
#include "BurnerBeam.h"

bool Burner::initialised_ = false;
int Burner::outline_dl_ = 0;
int Burner::outline_verts_index_ = 0;
int Burner::fill_dl_ = 0;
int Burner::fill_verts_index_ = 0;

Burner::Burner(void)
: BeamFiringSection()
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

	health_ = FlexFloat(800, 800);
	beam_ = BeamSection_ptr(new BurnerBeam());
	mass_ = 300;

	beam_charge_time_ = 0.7f;
	beam_cooldown_time_ = 0.2f;
	beam_fire_time_ = 5.0f;
	beam_energy_use_ = 4;
	section_type_ = "Burner";
}

Burner::~Burner(void)
{
}

void Burner::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -2.5f, 0));		//0
	temp_outline->push_back(Vector3f(2.5f, 0, 0));		//1
	temp_outline->push_back(Vector3f(2.5f, 6, 0));		//2
	temp_outline->push_back(Vector3f(1.5f, 6, 0));		//3
	temp_outline->push_back(Vector3f(1.5f, 2.5f, 0));	//4
	temp_outline->push_back(Vector3f(-1.5f, 2.5f, 0));	//5
	temp_outline->push_back(Vector3f(-1.5f, 6, 0));		//6
	temp_outline->push_back(Vector3f(-2.5f, 6, 0));		//7
	temp_outline->push_back(Vector3f(-2.5f, 0, 0));		//8

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);	

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[4]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[8]);

	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[4]);

	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[8]);
	
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[8]);

	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[7]);
	temp_fill->push_back((*temp_outline)[8]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_= Filled::CreateFillDisplayList(temp_fill);
}
