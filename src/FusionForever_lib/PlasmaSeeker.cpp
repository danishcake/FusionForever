#include "StdAfx.h"
#include "PlasmaSeeker.h"
#include "ThrusterTrail.h"

bool PlasmaSeeker::initialised_ = false;
int PlasmaSeeker::fill_dl_ = 0;
int PlasmaSeeker::fill_verts_index_ = 0;

static const float SPEED_START = 200;
static const float SPEED_END = 800;
static const float TURN_START = 300;
static const float TURN_END = 50;
static const float TRANSITION_TIME = 1;
static const float TOTAL_LIFETIME = 5;


PlasmaSeeker::PlasmaSeeker(Vector3f _position, BaseEntity* _target, GLColor _trail_color, float _scale, float _damage_scale)
: HomingProjectile(_target)
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(_trail_color.GetFaded(0.75));

	lifetime_ = TOTAL_LIFETIME;
	damage_ = 700 * _damage_scale;
	turn_rate_ = 50;
	scalar_speed_ = 600;
	velocity_.y = scalar_speed_;
	position_ = _position;
	first_run_ = true;
	trail_color_ = _trail_color;
	mass_ = 1;
	wave_magnitude_ = 0;
	scale_ = _scale;
}

PlasmaSeeker::~PlasmaSeeker(void)
{

}

void PlasmaSeeker::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& /*_projectile_spawn*/)
{
/*
	Puff* p = new Puff();
	p->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(p));
	*/
}

void PlasmaSeeker::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
/*
	temp_fill->push_back(Vector3f(3 ,-5,0));
	temp_fill->push_back(Vector3f(-3,-5,0));
	temp_fill->push_back(Vector3f(0 , 5,0));
*/
	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PlasmaSeeker::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	
	float scale = (lifetime_ - (TOTAL_LIFETIME - TRANSITION_TIME)) / TRANSITION_TIME;
	//scale = scale * scale;
	scale = scale > 1.0f ? 1.0f : scale;
	scale = scale < 0.0f ? 0.0f : scale;
	scale = 1 - scale;
	//scale = sqrtf(scale);
	

	turn_rate_ = TURN_START + (TURN_END - TURN_START) * scale;
	scalar_speed_ = SPEED_START + (SPEED_END - SPEED_START) * scale;
	
	HomingProjectile::Tick(_timespan, _spawn_dec, _transform);
	if(first_run_)
	{
		first_run_ = false;
		//ThrusterTrail(BaseEntity* _source, float _length_scale, float _width_scale, GLColor _trail_color, float _angle, Vector3f _offset, bool _angular_correction, float _antiparallel_factor);
		ThrusterTrail* thrust_trail =  new ThrusterTrail(this, 0.6f * scale_, 0.5f * scale_, trail_color_, 0, Vector3f(0,0,0), true, 0);
		ThrusterTrail* thrust_trail2 = new ThrusterTrail(this, 0.5f * scale_, 0.4f * scale_, GLColor(255, 255, 255), 0, Vector3f(0,0,0), true, 0);
		_spawn_dec.push_back(thrust_trail);
		_spawn_dec.push_back(thrust_trail2);

	}
}
