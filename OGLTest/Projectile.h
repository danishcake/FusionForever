#pragma once
#include "BaseEntity.h"
#include "Outlined.h"
#include "Decoration.h"
#include "Datastore.h"
#include <boost/shared_ptr.hpp>

class Projectile;
//typedef boost::shared_ptr<Projectile> Projectile_ptr;
typedef Projectile* Projectile_ptr;

class Projectile :
	public BaseEntity, public Outlined
{
protected:
	float damage_;
	float lifetime_;
public:
	Projectile(void);
	virtual ~Projectile(void);

	void DrawSelf();
	virtual void Tick(float _timespan, Matrix4f _transform);
	virtual void Hit(std::list<Decoration_ptr>& _spawn) = 0;
	
	float GetDamage(){return damage_;}
	float GetLifetime(){return lifetime_;}
	void SetLifetime(float _lifetime){lifetime_ = _lifetime;}

	static bool IsProjectileRemovable(Projectile_ptr prj)
	{
		if(prj->GetLifetime() <= 0)
		{
			delete prj;
			return true;
		}
		return false;
		
	}
};
