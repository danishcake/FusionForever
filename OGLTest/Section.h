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

class Section;
typedef boost::shared_ptr<Section> Section_ptr;
class Core;
typedef boost::shared_ptr<Core> Core_ptr;

#define SECTION_FLASH_TIME 2.0f

class Section :
	public BaseEntity, public Filled, public Outlined
{
protected:
	std::vector<Section_ptr> sub_sections_;
	float health_;
	float max_health_;
	bool firing_;
	float damage_timer_;
	GLColor outline_color_base_;

	void findRadius();
public:
	Section(void);
	virtual ~Section(void);
	void AddChild(Section * child);
	virtual void DrawSelf();
	bool CheckCollisions(Projectile_ptr _projectile);
	bool CheckCollisions(Vector3f _location, Section*& _section);
	void SetColor(GLColor _color);
	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
	virtual void GetDeathSpawn(std::list<Decoration_ptr>& _spawn_dec);

	//Getters/Setters
	float GetHealth(){return health_;}
	void TakeDamage(float _damage){health_-=_damage; damage_timer_ = SECTION_FLASH_TIME;}
	void SetFiring(bool _firing){firing_ = _firing;}
	
	//Predicates
	static bool IsRemovable(Section_ptr section);
};
