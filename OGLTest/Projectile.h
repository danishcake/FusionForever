#pragma once
#include "BaseEntity.h"
#include "Outlined.h"
#include "Decoration.h"
#include "Datastore.h"

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
	virtual void Hit(std::list<boost::shared_ptr<Decoration>> _spawn) = 0;
	
	float GetDamage(){return damage_;}
	float GetLifetime(){return lifetime_;}
	void SetLifetime(float _lifetime){lifetime_ = _lifetime;}

	static bool IsProjectileRemovable(boost::shared_ptr<Projectile> prj)
	{
		return (prj->GetLifetime() <= 0);
	}
};
