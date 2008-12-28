#include "StdAfx.h"
#include "BillboardDeco.h"

BillboardDeco::BillboardDeco(std::string _filename, float _lifetime, BillboardDecoType::Enum _billboard_type)
{
	ltv_transform_ = Matrix4f::createScale(1);


	billboard_ = new Billboard(_filename, BillboardType::WorldSpace);
	lifetime_ = _lifetime;
	total_lifetime_ = _lifetime;
	billboard_type_ = _billboard_type;
	scale_ = 1.0f;
	transparancy_ = 1.0f;
	initial_size_ = billboard_->GetSize();
}

BillboardDeco::~BillboardDeco(void)
{
	delete billboard_;
}

void BillboardDeco::Tick(float _timespan, Matrix4f _transform)
{
	Decoration::Tick(_timespan, _transform);
	float life_fraction = (total_lifetime_ - lifetime_) / total_lifetime_;
	switch(billboard_type_)
	{
		case BillboardDecoType::Explodes:
			if(life_fraction < 0.5f)
			{
				scale_ = powf(life_fraction * 2.0f, 1.7f);
				transparancy_ = 1.0f;
			} else
			{
				scale_ = 1 - (life_fraction * 2.0f - 1.0f) * 0.2f ;
				transparancy_ = (1.0f - life_fraction);
			}
			break;
		case BillboardDecoType::FadesOut:
			scale_ = 1.0f;
			transparancy_ = 1 - life_fraction;
			break;
	}
}

void BillboardDeco::InitialiseGraphics()
{
	//Do nothing, not used. Only present to satisfy virtual function
}

void BillboardDeco::DrawSelf()
{
	//Do custom drawing
	billboard_->SetPosition(position_);
	billboard_->SetSize(initial_size_ * scale_ * 0.4f);
	billboard_->SetColor(GLColor(255,255,255, transparancy_));
	billboard_->Draw();
}

BillboardDeco* BillboardDeco::RandomExplosion()
{
	int index = Random::RandomIndex(5);
	switch(index)
	{
		case 0:
			return new BillboardDeco("puff128x128", 0.4f, BillboardDecoType::Explodes);
			break;
		case 1:
			return new BillboardDeco("puff128x128blue", 0.4f, BillboardDecoType::Explodes);
			break;
		case 2:
			return new BillboardDeco("puff128x128green", 0.4f, BillboardDecoType::Explodes);
			break;
		case 3:
			return new BillboardDeco("puff64x64", 0.25f, BillboardDecoType::Explodes);
			break;
		case 4:
			return new BillboardDeco("puff64x64blue", 0.25f, BillboardDecoType::Explodes);
			break;
		default:
			return new BillboardDeco("puff64x64green", 0.25f, BillboardDecoType::Explodes);
			break;
	}
}
