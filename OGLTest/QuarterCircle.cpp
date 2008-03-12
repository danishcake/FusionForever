#include "StdAfx.h"
#include "QuarterCircle.h"

bool QuarterCircle::initialised_ = false;
int QuarterCircle::outline_dl_ = 0;
int QuarterCircle::outline_verts_index_ = 0;
int QuarterCircle::fill_dl_ = 0;
int QuarterCircle::fill_verts_index_ = 0;

static const int CIRCLE_SECTIONS = 8;
static const float CIRCLE_RADIUS = 10.0f;

QuarterCircle::QuarterCircle(void)
: Section()
{
	if(!initialised_)
	{
		initialise_outline();
		initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 1000;
	max_health_ = health_;
	default_sub_section_position_ = Vector3f(0, 7.5f, 0);
}

QuarterCircle::~QuarterCircle(void)
{
}

void QuarterCircle::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

	for(int i = 1; i < CIRCLE_SECTIONS; i++)
	{
		temp_fill->push_back((*temp_outline)[0]);
		temp_fill->push_back((*temp_outline)[i]);
		temp_fill->push_back((*temp_outline)[i + 1]);
	}

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void QuarterCircle::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));	//0

	for(int i = 0; i < CIRCLE_SECTIONS + 1; i++)
	{
		temp_outline->push_back(Vector3f(CIRCLE_RADIUS * cos(((float)i/ (2 * (float)CIRCLE_SECTIONS)) * M_PI),CIRCLE_RADIUS * sin(((float)i/ (2 * (float)CIRCLE_SECTIONS)) * M_PI),0));
	}

	//I want this easily rotatable
	Vector3f rotate_point_ = Vector3f(2.5f,2.5f,0);
	for(unsigned int i = 0; i < temp_outline->size(); i++)
	{
		(*temp_outline)[i] -= rotate_point_;
	}


	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);	
}
