#include "StdAfx.h"
#include "Radar.h"
#include "Core.h"
#include "GLColor.h"

Vector3f Radar::player_position_ = Vector3f(0, 0, 0);
const float Radar::RadarFadeTime = 2.0f;
float Radar::time_since_player_update_ = 0.0f;

Radar::Radar(void)
{
	average_tonnage_enemies_ = 0;
	average_tonnage_friends_ = 0;
	radar_range_ = 1000.0f;
	radar_size_ = Vector3f(256.0f / 10.0f, 256.0f / 10.0f, 0);
	radar_size_.x /= Camera::Instance().GetAspectRatio();
	radar_position_ = Vector3f(10, 256 - radar_size_.y - 40, 0);
	for(int i = 0; i < 129; i++)
	{
		float angle = (((float)i) / 128.0f) * M_PI * 2.0f;
		outline_.push_back(Vector3f((radar_size_.x / 2) * sinf(angle),
									(radar_size_.y / 2) * cosf(angle),
									0) +
						   (radar_size_ / 2.0f));
	}
	time_since_player_update_ = 3;
}

Radar::~Radar(void)
{
}


void Radar::Update(std::vector<Core*> _enemies, std::vector<Core*> _friends, float _timespan)
{
	time_since_player_update_ += _timespan;
	small_points_friends_.clear();
	small_points_enemies_.clear();
	large_points_friends_.clear();
	large_points_enemies_.clear();
	huge_points_friends_.clear();
	huge_points_enemies_.clear();


	float total_tonnage_enemies = 0;
	float total_tonnage_friends = 0;
	BOOST_FOREACH(Core* core, _enemies)
	{
		Vector3f point_position = core->GetPosition() - player_position_;
		point_position /= radar_range_;
		if(point_position.lengthSq() > 1.0f) //limit to +- 1
		{
			point_position.normalize();
		}
		point_position += Vector3f(1, 1, 0);
		point_position /= 2.0f; //Scaled for 0-1

		point_position *= radar_size_;
		if(core->GetMass() < average_tonnage_enemies_)
			small_points_enemies_.push_back(point_position);
		else if(core->GetMass() < 2 *  average_tonnage_enemies_)
			large_points_enemies_.push_back(point_position);
		else
			huge_points_enemies_.push_back(point_position);

		total_tonnage_enemies += core->GetMass();
	}
	if(_enemies.size() > 0)
		average_tonnage_enemies_ = total_tonnage_enemies / _enemies.size();

	BOOST_FOREACH(Core* core, _friends)
	{
		Vector3f point_position = core->GetPosition() - player_position_;
		point_position /= radar_range_;
		if(point_position.lengthSq() > 1.0f) //limit to +- 1
		{
			point_position.normalize();
		}
		point_position += Vector3f(1, 1, 0);
		point_position /= 2.0f; //Scaled for 0-1

		point_position *= radar_size_;
		if(core->GetMass() < average_tonnage_friends_)
			small_points_friends_.push_back(point_position);
		else if(core->GetMass() < 2 *  average_tonnage_friends_)
			large_points_friends_.push_back(point_position);
		else
			huge_points_friends_.push_back(point_position);

		total_tonnage_friends += core->GetMass();
	}
	if(_friends.size() > 0)
		average_tonnage_friends_ = total_tonnage_friends / _friends.size();
}

void Radar::Draw()
{
	if(time_since_player_update_ < RadarFadeTime)
	{
		GLColor friend_color = GLColor(0, 200, 0);
		GLColor enemy_color = GLColor(200, 0, 0);
		GLColor edge_color = GLColor(100, 100, 100);

		if(time_since_player_update_ > RadarFadeTime / 2.0f)
		{
			float fade = 1.0f - ((time_since_player_update_ - (RadarFadeTime / 2.0f)) / (RadarFadeTime / 2.0f));
			friend_color = friend_color.GetFaded(fade);
			enemy_color = enemy_color.GetFaded(fade);
			edge_color = edge_color.GetFaded(fade);
		}

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 256, 0, 256, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(radar_position_.x, radar_position_.y, 0);
		//Draw points

		glEnableClientState(GL_VERTEX_ARRAY);

		//Draw background rings and central spot
		glLineWidth(2);
		glColor3ubv((GLubyte*)&edge_color);
		//glColor3ub(100,100,100);
		glVertexPointer(3, GL_FLOAT, 0, &outline_[0]);
		glDrawArrays(GL_LINE_STRIP, 0, outline_.size());
		glLineWidth(1);


		if(small_points_enemies_.size() > 0)
		{
			glVertexPointer(3, GL_FLOAT, 0, &small_points_enemies_[0]);
			glColor3ubv((GLubyte*)&enemy_color);
			glDrawArrays(GL_POINTS, 0, small_points_enemies_.size());
		}

		if(small_points_friends_.size() > 0)
		{
			glColor3ubv((GLubyte*)&friend_color);
			glVertexPointer(3, GL_FLOAT, 0, &small_points_friends_[0]);
			glDrawArrays(GL_POINTS, 0, small_points_friends_.size());
		}

		glPointSize(2);
		if(large_points_enemies_.size() > 0)
		{
			glVertexPointer(3, GL_FLOAT, 0, &large_points_enemies_[0]);
			glColor3ubv((GLubyte*)&enemy_color);
			glDrawArrays(GL_POINTS, 0, large_points_enemies_.size());
		}

		if(large_points_friends_.size() > 0)
		{
			glColor3ubv((GLubyte*)&friend_color);
			glVertexPointer(3, GL_FLOAT, 0, &large_points_friends_[0]);
			glDrawArrays(GL_POINTS, 0, large_points_friends_.size());
		}

		glPointSize(3);
		if(huge_points_enemies_.size() > 0)
		{
			glVertexPointer(3, GL_FLOAT, 0, &huge_points_enemies_[0]);
			glColor3ubv((GLubyte*)&enemy_color);
			glDrawArrays(GL_POINTS, 0, huge_points_enemies_.size());
		}

		if(huge_points_friends_.size() > 0)
		{
			glColor3ubv((GLubyte*)&friend_color);
			glVertexPointer(3, GL_FLOAT, 0, &huge_points_friends_[0]);
			glDrawArrays(GL_POINTS, 0, huge_points_friends_.size());
		}

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

void Radar::SetPlayerPosition(Vector3d _player_position)
{
	player_position_ = _player_position; 
	time_since_player_update_ = 0;
}