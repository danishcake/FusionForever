#pragma once
//class Section;

#include <vector>
#include "vmath.h"
#include "vmath-collisions.h"
#include "GLColor.h"
#include "Projectile.h"
#include "Decoration.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include <algorithm>

#define DEGTORAD (3.14/180)

class Section
{
protected:
	std::vector<Section*> sub_sections_;
	std::vector<Vector3f> outline_verts_;
	std::vector<Vector3f> fill_verts_;
	GLColor outline_color_;
	GLColor fill_color_;
	Matrix4f ltv_transform_;
	Vector3f ltv_position_;
	Vector3f target_position_;

	Section(void);
	void findRadius();
	void fireProjectile(Projectile* projectile, std::list<boost::shared_ptr<Projectile>>& _spawn);

	static bool IsSectionDead(Section * section);
	
public:	
	virtual ~Section(void);
	virtual void DrawSelf();
	void AddChild(Section* _new_section);
	void SetColor(GLColor _fill_color);
	bool CheckCollisions(boost::shared_ptr<Projectile> _projectile);

	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
	virtual void SetTarget(Vector3f position);	

	Vector3f Position;
	float Radius;
	float Angle;
	int Health;
	
};