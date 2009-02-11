#include "StdAfx.h"
#include "Ricochet.h"

static const float RICOCHET_LIFETIME = 0.25f;

bool Ricochet::initialised_ = false;
int Ricochet::fill_dl_ = 0;
int Ricochet::fill_verts_index_ = 0;

Ricochet::Ricochet(float _angle, bool _reversed)
{
	reversed_ = _reversed;
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	lifetime_ = RICOCHET_LIFETIME;
	fill_.SetFillColor(GLColor(255, 220, 190));
	ltv_transform_ = Matrix4f::createScale(0);
	angle_ = _angle + Random::RandomRange(-30,30);
	if(Random::RandomChance(0.15f) && !reversed_)
		angle_ += 180;
}

Ricochet::~Ricochet(void)
{
}

void Ricochet::Tick(float _timespan, Matrix4f _transform)
{
	float life_scale = ((RICOCHET_LIFETIME - lifetime_)/RICOCHET_LIFETIME);
	if(reversed_)
		life_scale = (1 - life_scale);
	Matrix4f scaleM  = Matrix4f::createScale(3*life_scale + 1);
	Matrix4f translationM = Matrix4f::createTranslation(0, 100 * life_scale, 0);
	Decoration::Tick(_timespan, _transform);
	_transform = ltv_transform_ * translationM;
	_transform = _transform * scaleM;
	ltv_transform_ = _transform;
	fill_.GetFillColor().a = static_cast<unsigned char>(255 * (1.0f - life_scale));
}

void Ricochet::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	
	temp_fill->push_back(Vector3f(0,0,0));
	temp_fill->push_back(Vector3f(0,10,0));
	temp_fill->push_back(Vector3f(1,8,0));

	temp_fill->push_back(Vector3f(0,0,0));
	temp_fill->push_back(Vector3f(0,10,0));
	temp_fill->push_back(Vector3f(-1,8,0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void Ricochet::ResetTimer()
{
	lifetime_ = RICOCHET_LIFETIME;
}
