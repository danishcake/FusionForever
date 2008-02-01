#include "StdAfx.h"
#include "HeatBeamGun.h"
#include "HeatBeam.h"

bool HeatBeamGun::initialised_ = false;
int HeatBeamGun::outline_dl_ = 0;
int HeatBeamGun::outline_verts_index_ = 0;
int HeatBeamGun::fill_dl_ = 0;
int HeatBeamGun::fill_verts_index_ = 0;

HeatBeamGun::HeatBeamGun(void)
: BeamFiringSection()
{
	if(!initialised_)
	{
		HeatBeamGun::initialise_outline();
		HeatBeamGun::initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 800;
	max_health_ = health_;
	beam_ = BeamSection_ptr(new HeatBeam());
}

HeatBeamGun::~HeatBeamGun(void)
{
}

void HeatBeamGun::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[4]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[5]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[6]);

	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[4]);

	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[7]);
	
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[7]);


	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_= CreateFillDisplayList(temp_fill);
}

void HeatBeamGun::initialise_outline(void)
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
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}
