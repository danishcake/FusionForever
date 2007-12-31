#include "StdAfx.h"
#include "Puff.h"

#define PUFF_SECTIONS 10
#define PUFF_RADIUS 7.5f
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
	fill_color_ =GLColor(255,
						 20 + 100 * ((float)rand()/(float)RAND_MAX),
						  0 + 10 * ((float)rand()/(float)RAND_MAX));
	lifetime_ = PUFF_LIFETIME;
	ltv_transform_ = Matrix4f::createScale(0);
}

Puff::~Puff(void)
{
}

void Puff::Tick(float _timespan, Matrix4f _transform)
{
	Matrix4f scaleM  = Matrix4f::createScale(sqrtf((PUFF_LIFETIME - lifetime_)/PUFF_LIFETIME));
	Decoration::Tick(_timespan, _transform);
	_transform = ltv_transform_ * scaleM;
	ltv_transform_ = _transform;
}

void Puff::initialise_fill()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	for(int i = 0; i < PUFF_SECTIONS; i++)
	{
//		float rad = (PUFF_RADIUS/2.0f)+pow(((i*3)%PUFF_SECTIONS),2);
		temp_fill->push_back(Vector3f(0,0,0));
		temp_fill->push_back(Vector3f(PUFF_RADIUS * cosf((float)i*2*M_PI/(float)PUFF_SECTIONS),PUFF_RADIUS*sinf((float)i*2*M_PI/(float)PUFF_SECTIONS),0));
		temp_fill->push_back(Vector3f(PUFF_RADIUS* cosf((float)(i+1)*2*M_PI/(float)PUFF_SECTIONS),PUFF_RADIUS* sinf((float)(i+1)*2*M_PI/(float)PUFF_SECTIONS),0));

	}


	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

