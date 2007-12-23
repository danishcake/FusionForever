#include "StdAfx.h"
#include "RigidArm.h"

bool RigidArm::initialised_ = false;
int RigidArm::rigidarm_outline_display_list_ = 0;
int RigidArm::outline_verts_index_ = 0;
int RigidArm::rigidarm_fill_display_list_ = 0;
int RigidArm::fill_verts_index_ = 0;

RigidArm::RigidArm(void)
: Section()
{
	if(!initialised_)
	{
		RigidArm::initialise_outline();
		RigidArm::initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = rigidarm_outline_display_list_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = rigidarm_fill_display_list_;
	findRadius();

	health_ = 800;
}

RigidArm::~RigidArm(void)
{
}

void RigidArm::initialise_fill(void)
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
	rigidarm_fill_display_list_ = CreateFillDisplayList(temp_fill);
}

void RigidArm::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Vector3f rotate_point_ = Vector3f(2.5f,2.5f,0);

	temp_outline->push_back(Vector3f(2.5f,0,0));	//0
	temp_outline->push_back(Vector3f(0,2.5f,0));	//1
	temp_outline->push_back(Vector3f(0,10,0));		//2
	temp_outline->push_back(Vector3f(2.5f,7.5f,0));//3
	temp_outline->push_back(Vector3f(5,10,0));		//4
	temp_outline->push_back(Vector3f(5,2.5,0));	//5

	for(int i = 0; i < temp_outline->size(); i++)
	{
		(*temp_outline)[i] -= rotate_point_;
	}

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	rigidarm_outline_display_list_ = CreateOutlinedDisplayList(temp_outline);	
}
