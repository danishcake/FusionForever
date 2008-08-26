#include "StdAfx.h"
#include "JointAngles.h"

bool JointAngles::initialised_ = false;
int JointAngles::outline_dl_ = 0;
int JointAngles::outline_verts_index_ = 0;
int JointAngles::fill_dl_ = 0;
int JointAngles::fill_verts_index_ = 0;

JointAngles::JointAngles(float _first_angle, float _second_angle, float _transition_time, float _pause_time)
: Section()
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
	first_angle_ = _first_angle;
	second_angle_ = _second_angle;
	transition_time_ = _transition_time;
	pause_time_ = _pause_time;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 200;
}

JointAngles::~JointAngles()
{
	
}

void JointAngles::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0));	//3

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

void JointAngles::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	total_time_ += _timespan;
	total_time_ = fmodf(total_time_, 2 * transition_time_ + 2 * pause_time_);
	if(total_time_ < pause_time_)
	{
		angle_ = first_angle_;
	}
	else if(total_time_ < pause_time_ + transition_time_)
	{
		angle_ = first_angle_ + (second_angle_ - first_angle_) * (total_time_-pause_time_)/transition_time_;
	}
	else if(total_time_ < 2 * pause_time_ + transition_time_)
	{
		angle_ = second_angle_;
	}
	else
	{
		angle_ = second_angle_ - (second_angle_ - first_angle_) * (total_time_ - 2 * pause_time_ - transition_time_)/transition_time_;
	}
}
