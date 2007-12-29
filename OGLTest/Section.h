#pragma once
#include "BaseEntity.h"
#include "Filled.h"
#include "Outlined.h"
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include "Projectile.h"
#include "Decoration.h"
#include "vmath-collisions.h"
#include "Datastore.h"
#include "Camera.h"

class Section :
	public BaseEntity, public Filled, public Outlined
{
private:
	static bool IsSectionDead(Section* section);
protected:
	std::vector<Section*> sub_sections_;
	float health_;
	bool firing_;

	void findRadius();
public:
	Section(void);
	virtual ~Section(void);
	void AddChild(Section * child);
	void DrawSelf();
	bool CheckCollisions(boost::shared_ptr<Projectile> _projectile);
	void SetColor(GLColor _color);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
	
	//Getters/Setters
	float GetHealth(){return health_;}
	float TakeDamage(float _damage){health_-=_damage;}
	void SetFiring(bool _firing){firing_ = _firing;}
	
};
