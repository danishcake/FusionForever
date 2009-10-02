#include "StdAfx.h"
#include "MediumBullet.h"
#include "Puff.h"
#include "Ricochet.h"
#include "SVGParser.h"
#include "Triangulate.h"

bool MediumBullet::initialised_ = false;
int MediumBullet::fill_dl_ = 0;
int MediumBullet::fill_verts_index_ = 0;

MediumBullet::MediumBullet(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(150,150,255,0.7f));
	damage_ = 75.0f;
	lifetime_ = 3.0f;
	velocity_.y = 500;
	position_ = _position;
	mass_ = 25;
}

MediumBullet::~MediumBullet(void)
{
}

void MediumBullet::InitialiseGraphics()
{
	//Initialise outline and fill from embedded SVG from Inkscape
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	*temp_outline = SVGParser::ParsePath("M -0.5,-12.5 L -1.5,3.5 L 0,7.5 L 1.5,3.5 L 0.5,-12.5 L -0.5,-12.5 z");
	temp_outline->pop_back();

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void MediumBullet::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& /*_projectile_spawn*/)
{
	Decoration_ptr spark = Decoration_ptr(new Ricochet(angle_, false));
	spark->SetPosition(position_);
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(position_);

	_spawn.push_back(puff);
	_spawn.push_back(spark);
}
