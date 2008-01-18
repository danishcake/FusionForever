#include "StdAfx.h"
#include "SpinningJoint.h"

bool SpinningJoint::initialised_ = false;
int SpinningJoint::spinningjoint_outline_display_list_ = 0;
int SpinningJoint::outline_verts_index_ = 0;
int SpinningJoint::spinningjoint_fill_display_list_ = 0;
int SpinningJoint::fill_verts_index_ = 0;

SpinningJoint::SpinningJoint(void)
: Section()
{
	if(!initialised_)
	{
		SpinningJoint::initialise_outline();
		SpinningJoint::initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = spinningjoint_outline_display_list_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = spinningjoint_fill_display_list_;
	findRadius();

	health_ = 800;
}

SpinningJoint::~SpinningJoint(void)
{
}

void SpinningJoint::initialise_fill(void)
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
	spinningjoint_fill_display_list_ = CreateFillDisplayList(temp_fill);
}

void SpinningJoint::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));		//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0));//3

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	spinningjoint_outline_display_list_ = CreateOutlinedDisplayList(temp_outline);	
}

void SpinningJoint::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	angle_ += _timespan * 1.0f;
}