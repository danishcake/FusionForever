#include "StdAfx.h"
#include "HomingMissile.h"
#include "Puff.h"

bool HomingMissile::initialised_ = false;
int HomingMissile::fill_dl_ = 0;
int HomingMissile::fill_verts_index_ = 0;

static const float TOTAL_LIFETIME = 5;
static const float ACCELERATION_START = 0.5f;
static const float ACCELERATION_END = 1.4f;
static const float SPEED_MIN = 100;
static const float SPEED_MAX = 400;
static const float TURN_START = 300;
static const float TURN_END = 50;


HomingMissile::HomingMissile(Vector3f _position, BaseEntity* _target, GLColor _trail_color)
: HomingProjectile(_target)
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);

	lifetime_ = TOTAL_LIFETIME;
	damage_ = 380;
	turn_rate_ = 150;
	scalar_speed_ = 200;
	velocity_.y = scalar_speed_;
	position_ = _position;
	first_run_ = true;
	trail_color_ = _trail_color;
	mass_ = 300;
}

HomingMissile::~HomingMissile(void)
{

}

void HomingMissile::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Puff* p = new Puff();
	p->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(p));
}

void HomingMissile::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f(2 ,-2,0));
	temp_fill->push_back(Vector3f(-2,-2,0));
	temp_fill->push_back(Vector3f(0 , 3,0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void HomingMissile::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	if(first_run_)
	{
		first_run_ = false;
		line_trace_ = new LineTrace((BaseEntity*)this, trail_color_);
		_spawn_dec.push_back(line_trace_);
	}
	float scale;
	if(lifetime_ > TOTAL_LIFETIME - ACCELERATION_START)
		scale = 0.0f;
	else if(lifetime_ > TOTAL_LIFETIME - ACCELERATION_END)
		scale = ((TOTAL_LIFETIME-ACCELERATION_START) - lifetime_) / (ACCELERATION_END - ACCELERATION_START);
	else
		scale = 1.0f;

	turn_rate_ = TURN_START + (TURN_END - TURN_START) * scale;
	scalar_speed_ = SPEED_MIN + (SPEED_MAX - SPEED_MIN) * scale;
	HomingProjectile::Tick(_timespan, _spawn_dec, _transform);
}
