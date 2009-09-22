#include "StdAfx.h"
#include "GasClouds.h"
#include "Billboard.h"
#include "Camera.h"
#include "vmath-collisions.h"

GasClouds::GasClouds(void)
{
	const float radius = 1000;
	billboard_ = new Billboard("GasCloud", BillboardType::WorldSpace);
	billboard_->SetSize(Vector2f(2048, 2048));
	billboard_->SetColor(GLColor(255, 255, 255, 0.2f));

	const int nebula_count = 9;
	std::string nebula_textures[nebula_count] = {"Nebula1", "Nebula2", "Nebula3", "Nebula4", "Nebula5", "Nebula6", "Nebula7", "Nebula8", "Nebula9"};

	//Regular 5x5 grid of patches of nebulae, each with 4 nebulae in
	float seperation = 1600;
	float global_spread = 300;
	float local_spread = 500;
	for(int x = 0; x < 5; x++)
	{
		for(int y = 0; y < 5; y++)
		{
			Vector2f position = Vector2f(x * seperation + Random::RandomCentered(0, global_spread), y * seperation + Random::RandomCentered(0, global_spread));
			for(int k = 0; k < 4; k++)
			{
				Billboard* feature = new Billboard(nebula_textures[Random::RandomIndex(nebula_count)], BillboardType::WorldSpace);
				feature->SetPosition(Vector3f(position.x + Random::RandomRange(-local_spread, local_spread), position.y + Random::RandomRange(-local_spread, local_spread), 0));
				feature->SetColor(GLColor(255,255,255, Random::RandomRange(0.4f, 0.8f)));
				feature->SetRotation(Random::RandomCentered(0, 360));
				feature->SetSize(Vector2f(1024, 1024));
				features_.push_back(feature);
			}
		}
	}
}

GasClouds::~GasClouds(void)
{
	delete billboard_;
}

void GasClouds::DrawGasClouds(Vector3f _position)
{
	int x_minus_times = static_cast<int>(((Camera::Instance().GetWidth() / 2.0f) / 2048) + 2) + 1;
	int y_minus_times = static_cast<int>(((Camera::Instance().GetHeight() / 2.0f) / 2048) + 2);
	float left = -2048 * x_minus_times;
	for(int x = 0; x < x_minus_times * 2; x++)
	{
		float top = -2048 * y_minus_times;
		for(int y = 0; y < y_minus_times * 2; y++)
		{
			Vector3f offset = Vector3f(-fmodf(0.75f * _position.x, 2048),
									   -fmodf(0.75f * _position.y, 2048),
									   0);
			billboard_->SetPosition(Vector3f(left, top, 0) + offset + _position);
			billboard_->Draw();
			top += 2048;
		}
		left += 2048;
	}



	/* Clouds are defined over area 0 - 8000 +-800. 
	Scrolling is at 0.2 * camera focus, so only need to tile every 40,000
	Spread means should start drawing when offscreen a bit
	Should tile at edges*/

	Vector3f extent = Vector3f(5 * 1600, 5 * 1600, 0);
	int x_low = (int)((Camera::Instance().GetLeft() - 00) * 0.2 / extent.x) - 1;
	int x_high = (int)((Camera::Instance().GetRight() + 00)* 0.2 / extent.x) + 1;

	
	int y_low = (int)((Camera::Instance().GetTop() - 00) * 0.2 / extent.y)-1;
	int y_high = (int)((Camera::Instance().GetBottom() + 00) * 0.2 / extent.y) +1;

	for(int x = x_low; x <= x_high; x++)
	{
		for(int y = y_low; y <= y_high; y++)
		{
			Vector3f feature_offset = Vector3f(0.2f * _position.x + x * extent.x, 0.2f * _position.y + y * extent.y, 0);
			BOOST_FOREACH(Billboard* feature, features_)
			{
				//This is v inelegant, but can't do it better without larger changes - probably should make them anyway
				Vector3f old_position = feature->GetPosition();
				feature->SetPosition(old_position + feature_offset);
				feature->Draw();
				feature->SetPosition(old_position);
			}
		}
	}

}