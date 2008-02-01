#include "StdAfx.h"
#include "Puff.h"

#define PUFF_SECTIONS 30

#define PUFF_RADIUS 10
#define PUFF_LIFETIME 0.3f

bool Puff::initialised_ = false;
int Puff::fill_dl_ = 0;
int Puff::fill_verts_index_ = 0;

Puff::Puff(void)
{
	if(!initialised_)
	{
		initialise_fill();
		initialised_ = true;
	}
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	if(rand() > RAND_MAX *0.8)
	{
		fill_color_ =GLColor(255, 255, 0);
	}
	else
	{
		fill_color_ =GLColor(255,
							 20 + 100 * ((float)rand()/(float)RAND_MAX),
							  0 + 10 * ((float)rand()/(float)RAND_MAX));
	}
	lifetime_ = PUFF_LIFETIME;
	ltv_transform_ = Matrix4f::createScale(0);
	angle_ = 360.0f* (float)rand()/ (float)RAND_MAX;
}

Puff::~Puff(void)
{
}

void Puff::Tick(float _timespan, Matrix4f _transform)
{
	Matrix4f scaleM  = Matrix4f::createScale(((PUFF_LIFETIME - lifetime_)/PUFF_LIFETIME));
	Decoration::Tick(_timespan, _transform);
	_transform = ltv_transform_ * scaleM;
	ltv_transform_ = _transform;
}

float get_radius(int i)
{
	const float spike_factor = 2;
	const float base = 0.8f;
	const int spikes = 7;
	const float fm = 2 * M_PI / spikes;
	float ang = i * 2 * M_PI / PUFF_SECTIONS;
	return (base + spike_factor * powf(fmodf(ang,fm) - (fm/2), 2.0f)) * PUFF_RADIUS;
}

void Puff::initialise_fill()
{

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	//$B$3+$B$2*(MOD(A7,$B$5)-($B$5/2))^$B$1
	for(int i = 0; i < PUFF_SECTIONS; i++)
	{
		temp_fill->push_back(Vector3f(0,0,0));
		temp_fill->push_back(Vector3f(get_radius(i) * cosf((float)i*2*M_PI/(float)PUFF_SECTIONS),get_radius(i) * sinf((float)i*2*M_PI/(float)PUFF_SECTIONS),0));
		temp_fill->push_back(Vector3f(get_radius(i + 1) * cosf((float)(i+1)*2*M_PI/(float)PUFF_SECTIONS),get_radius(i+1) * sinf((float)(i+1)*2*M_PI/(float)PUFF_SECTIONS),0));
	}


	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

