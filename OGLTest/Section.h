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


class HomingJoin;
class Section;
//typedef boost::shared_ptr<Section> Section_ptr;
typedef Section* Section_ptr;
class Core;
//typedef boost::shared_ptr<Core> Core_ptr;
typedef Core* Core_ptr;

static const float SECTION_FLASH_TIME = 2.0f;

class Section :
	public BaseEntity, public Filled, public Outlined
{
private:
	static int section_count_;
protected:
	int section_id_;
	std::vector<Section_ptr> sub_sections_;
	float health_;
	float max_health_;

	bool firing_;
	bool ltv_firing_;
	std::vector<float> firing_edges_;
	float firing_delay_;

	float damage_timer_;
	GLColor outline_color_base_;
	Vector3f default_sub_section_position_;
	std::list<HomingJoin*> homing_joins_;
	void findRadius();

public:
	Section(void);
	virtual ~Section(void);
	void AddChild(Section * child);
	virtual void DrawSelf();
	bool CheckCollisions(Projectile_ptr _projectile);
	bool CheckCollisions(Vector3f _location, Section*& _section);
	void RayCollisionFilter(Vector3f P1, Vector3f P2, std::list<Section*>& _valid_sections, float& _min_distance, float& _max_distance);
	void SetColor(GLColor _color);
	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
	virtual void GetDeathSpawn(std::list<Decoration_ptr>& _spawn_dec);
	void UnregisterHomingJoin(HomingJoin* _homing_join);
	void RegisterHomingJoin(HomingJoin* _homing_join);


	//Getters/Setters
	float GetHealth(){return health_;}
	void SetMaxHealth(float _max_health){float health_fraction = health_ / max_health_; max_health_ = _max_health; health_ = max_health_ * health_fraction;}
	void TakeDamage(float _damage){health_-=_damage; damage_timer_ = SECTION_FLASH_TIME;}
	void ScaleHealth(float _factor);
	void SetFiring(bool _firing);
	void SetFiringDelay(float _firing_delay) {firing_delay_ = _firing_delay;}

	//Predicates
	static bool IsRemovable(Section_ptr section)
	{
		bool dead = (section->health_ <= 0);
		if(dead)
		{
			Camera::Instance().Shake();
			delete section;
		}
		return dead;
	}
};
