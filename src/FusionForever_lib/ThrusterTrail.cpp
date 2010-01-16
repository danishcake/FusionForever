#include "StdAfx.h"
#include "ThrusterTrail.h"
#include "TurningRoutines.h"

ThrusterTrail::ThrusterTrail(BaseEntity* _source, float _length_scale, float _width_scale, GLColor _trail_color, float _angle, Vector3f _offset, bool _angular_correction, float _antiparallel_factor)
{
	source_ = _source;
	assert(source_ != NULL);
	source_->AddSubscriber(this);
	lifetime_ = 20;
	first_run_ = true;
	fill_.SetFillColor(_trail_color);
	ltv_source_position_ = source_->GetTransform() * _offset;
	section_length_ = 4.0f;
	length_scale_ = _length_scale;
	width_scale_ = _width_scale;
	angular_offset_ = _angle;
	offset_ = _offset;
	angular_corrected_ = _angular_correction;
	antiparallel_factor_ = _antiparallel_factor;
	/* Create initial array of points, triangles etc */
	cartesian_points_.push_back(Vector3f(0,0,0));
	cartesian_points_.push_back(Vector3f(0,0,0));
	points_.push_back(0);
	points_.push_back(0);
		
	for(int i = 2; i < 10; i++)
	{
		points_.push_back(0);
		cartesian_points_.push_back(Vector3f(0,0,0));
		fill_points_.push_back(Vector3f(0, 0, 0));
		fill_points_.push_back(Vector3f(0, 0, 0));
		fill_points_.push_back(Vector3f(0, 0, 0));
		fill_points_.push_back(Vector3f(0, 0, 0));
		fill_points_.push_back(Vector3f(0, 0, 0));
		fill_points_.push_back(Vector3f(0, 0, 0));
	}
}

ThrusterTrail::~ThrusterTrail(void)
{
	if(source_!=NULL)
		source_->RemoveSubscriber(this);
}

void ThrusterTrail::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	Decoration::Tick(_timespan, _transform, _decoration_spawn);
	if(source_!=NULL && _timespan > 0)
	{
		//Get updated position and angle for the root segment, Speed is calculated purely by change in position
		Vector3f np = source_->GetTransform() * offset_;
		Vector3f dp = np - ltv_source_position_;

		float angle = source_->GetGlobalAngle() + angular_offset_;
		//if(!angular_corrected_)
			//angle += angular_offset_;
		Vector3f forward = Vector3f(sin(angle * M_PI / 180.0f), cos(angle * M_PI / 180.0f), 0);			

		float speed  = dp.length() / _timespan;
		float target_length = speed * 0.02f * length_scale_;

		if(dp.lengthSq() > 0.0f)
		{
			//Calculate the dot product of section movement with section angle. This is used to turn the plume towards the back of the segment
			dp.normalize();
			float alignment_scale = dp.dotProduct(forward);
			/* k(x+1)^2 + P = y. Fit points (-1,P) and (1, 1) -> k = (1-P)/4 valid for -1 to 1.*/
			alignment_scale = ((alignment_scale+1) * (alignment_scale+1) * (1.0f - antiparallel_factor_) / 4.0f) + antiparallel_factor_;
			target_length *= alignment_scale;

			if(angular_corrected_)
			{
				TurnData td = GetTurnDirection(angle, dp);
				angle += td.turn_factor * 30;
			}
		}

		//Calculate the weighting to give the new calculated length, so that length changes smoothly regardless of framerate.
		float new_weight = 1.0f - expf(-_timespan * 7.0f);
		float rotation_rate = (400 + speed) * 1;

		if(section_length_ < 0)
			section_length_ = 0;
		section_length_ = (1.0f - new_weight) * section_length_ + new_weight * target_length;
		float section_length_sqr = sqrt(section_length_);

		//Calculate worldspace coordinates of plume polygons
		points_[0] = angle;
		cartesian_points_[0] = np;
		for(int i = 1; i < points_.size(); i++)
		{
			TurnData td = GetTurnDirection(points_[i], points_[i-1]);
			cartesian_points_[i] = cartesian_points_[i-1] + Vector3f(-sin(points_[i] * M_PI / 180.0f), -cos(points_[i] * M_PI / 180.0f), 0) * section_length_;
			points_[i] = points_[i] + td.turn_factor * _timespan * rotation_rate;

			Vector3f perp = cartesian_points_[i] - cartesian_points_[i-1];
			perp.z = perp.x;
			perp.x = -perp.y;
			perp.y = perp.z;
			perp.z = 0;
			if(perp.lengthSq() > 0)
				perp.normalize();
			perp *= (1.0f + 3.0f * section_length_sqr * sin(((float)i / (float)points_.size()) * M_PI));
			perp *= width_scale_;

			int base = (i - 2) * 6 + 3;
			if(i==1)
			{
				fill_points_[0] = cartesian_points_[0];
				fill_points_[1] = cartesian_points_[1] + perp;
				fill_points_[2] = cartesian_points_[1] - perp;
			} else if(i < points_.size() - 1)
			{
				fill_points_[base + 0] = fill_points_[base - 1];
				fill_points_[base + 1] = fill_points_[base - 2];
				fill_points_[base + 2] = cartesian_points_[i] + perp;
				fill_points_[base + 3] = fill_points_[base - 1];
				fill_points_[base + 4] = cartesian_points_[i] + perp;
				fill_points_[base + 5] = cartesian_points_[i] - perp;
			} else
			{
				cartesian_points_[i] = cartesian_points_[i-1] + Vector3f(-sin(points_[i] * M_PI / 180.0f), -cos(points_[i] * M_PI / 180.0f), 0) * section_length_ * 2;
				fill_points_[base + 0] = fill_points_[base - 1];
				fill_points_[base + 1] = fill_points_[base - 2];
				fill_points_[base + 2] = cartesian_points_[i];
			}
		}

		lifetime_ = 1.0f; // should live as long as source exists
		ltv_source_position_ = source_->GetGlobalPosition();

		const float minimum_length = 0.4f;
		const float fade_length = 1.0f;
		
		if(section_length_ < minimum_length)
		{
			fill_.GetFillColor().a = 0;
		} else if(section_length_ < fade_length + minimum_length)
		{
			float alpha = 255.0f * ((section_length_ - minimum_length) / fade_length) ;
			fill_.GetFillColor().a = alpha;
		} else
		{
			fill_.GetFillColor().a = 255;
		}
		
	} else
	{
		fill_.GetFillColor().a = 255.0f * lifetime_;
	}
}

void ThrusterTrail::DrawSelf()
{
	glColor4ub(fill_.GetFillColor().r, fill_.GetFillColor().g, fill_.GetFillColor().b, fill_.GetFillColor().a);
	glPushMatrix();
	glLoadIdentity();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &fill_points_[0]);
	glDrawArrays(GL_TRIANGLES, 0, fill_points_.size());

	glPopMatrix();
}

void ThrusterTrail::EndSubscription(Subscriber* _source)
{
	if(source_ == _source)
		source_ = NULL;
}
