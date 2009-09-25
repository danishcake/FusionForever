#include "StdAfx.h"
#include "Explosion.h"

bool Explosion::initialised_ = false;
int Explosion::fill_dl_ = 0;
int Explosion::fill_verts_index_ = 0;

static float ExplosionLifetime = 0.2f;
static int ExplosionSegments = 120;

Explosion::Explosion(void)
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);

	if(Random::RandomChance(0.2f))
	{
		fill_.SetFillColor(GLColor(255, 255, 0));
	}
	else
	{
		fill_.SetFillColor(GLColor(255, static_cast<unsigned char>(Random::RandomQuantity(20,120)), static_cast<unsigned char>(Random::RandomQuantity(0,20))));
	}

	lifetime_ = ExplosionLifetime;
	ltv_transform_ = Matrix4f::createScale(0);
}

Explosion::~Explosion(void)
{
}

void Explosion::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	Matrix4f scaleM  = Matrix4f::createScale(((ExplosionLifetime - lifetime_)/ExplosionLifetime));
	Decoration::Tick(_timespan, _transform, _decoration_spawn);
	_transform = ltv_transform_ * scaleM;
	ltv_transform_ = _transform;
}

void Explosion::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	for(int i = 0; i < ExplosionSegments; i++)
	{
		temp_fill->push_back(Vector3f(0,0,0));
		float r1 = 25 + 0.55f * powf(static_cast<float>(i % 20 - 10), 2);
		float r2 = 25 + 0.55f * powf(static_cast<float>((i+1) % 20 - 10), 2);
		temp_fill->push_back(Vector3f(r1 * cosf((float)i * 2 * M_PI / ExplosionSegments), 
									  r1 * sinf((float)i * 2 * M_PI / ExplosionSegments), 0));
		temp_fill->push_back(Vector3f(r2 * cosf((float)(i + 1) * 2 * M_PI / ExplosionSegments),
									  r2 * sinf((float)(i + 1) * 2 * M_PI / ExplosionSegments), 0));
	}

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

