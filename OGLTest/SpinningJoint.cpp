#include "StdAfx.h"
#include "SpinningJoint.h"

bool SpinningJoint::initialised_ = false;
int SpinningJoint::outline_dl_ = 0;
int SpinningJoint::outline_verts_index_ = 0;
int SpinningJoint::fill_dl_ = 0;
int SpinningJoint::fill_verts_index_ = 0;

SpinningJoint::SpinningJoint(float _degrees_per_second)
: Section()
{
	if(!initialised_)
	{
		SpinningJoint::initialise_outline();
		SpinningJoint::initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 800;
	max_health_ = health_;
	degrees_per_second_ = _degrees_per_second;
}

SpinningJoint::~SpinningJoint()
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
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void SpinningJoint::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));		//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0));//3

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}

void SpinningJoint::Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies);
	angle_ += _timespan * degrees_per_second_;
}