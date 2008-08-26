#include "StdAfx.h"
#include "BeamCharge.h"

bool BeamCharge::initialised_ = false;
int BeamCharge::fill_dl_ = 0;
int BeamCharge::fill_verts_index_ = 0;

BeamCharge::BeamCharge(void)
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255, 255, 255));
	lifetime_ = 1;
	ltv_transform_ = Matrix4f::createScale(0);
	angle_ = Random::RandomRange(0,360);
}

BeamCharge::~BeamCharge(void)
{
}

void BeamCharge::SetScale(float _scale)
{
	scale_ = pow(_scale, 2.0f);
	lifetime_ = 1.0f;
}

void BeamCharge::Tick(float _timespan, Matrix4f _transform)
{
	angle_ = Random::RandomRange(0, 360);
	Matrix4f scaleM  = Matrix4f::createScale(scale_);
	Decoration::Tick(_timespan, _transform);
	_transform = ltv_transform_ * scaleM;
	ltv_transform_ = _transform;
}

void BeamCharge::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	for(int i = 0; i < 32; i++)
	{
		temp_fill->push_back(Vector3f(0,0,0));
		float angle = (float)i * 2 * M_PI / (float)32;
		temp_fill->push_back(Vector3f(6 * cosf(angle), 7 * sinf(angle), 0));
		angle = (float)(i+1) * 2 * M_PI / (float)32;
		temp_fill->push_back(Vector3f(6 * cosf(angle), 7 * sinf(angle), 0));
	}

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

