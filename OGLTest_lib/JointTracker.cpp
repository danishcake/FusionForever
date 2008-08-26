#include "StdAfx.h"
#include "JointTracker.h"
#include "TurningRoutines.h"

bool JointTracker::initialised_ = false;
int JointTracker::outline_dl_ = 0;
int JointTracker::outline_verts_index_ = 0;
int JointTracker::fill_dl_ = 0;
int JointTracker::fill_verts_index_ = 0;

static const float TURN_RATE = 90.0f;

JointTracker::JointTracker(bool _only_when_firing)
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
	target_ = NULL;
	findRadius();

	health_ = FlexFloat(800, 800);
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 200;
	only_when_firing_ = _only_when_firing;
}

JointTracker::~JointTracker()
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void JointTracker::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0)); //3

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

void JointTracker::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	if(!target_)
	{
		if(_enemies.size() > 0)
		{
			target_ = reinterpret_cast<BaseEntity*>(_enemies[Random::RandomIndex(static_cast<int>(_enemies.size()))]);
			target_->AddSubscriber(this);
		}
	}
	if(target_ && (!only_when_firing_ || firing_))
	{
		TurnData turn_data = GetTurnDirection(GetGlobalAngle(), target_->GetGlobalPosition() - GetGlobalPosition());
		
		angle_ += ClampTurnDirection(turn_data.turn_factor, 0.01f) * TURN_RATE * _timespan;
	}
}

void JointTracker::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}