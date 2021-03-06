#pragma once
#include "BaseEntity.h"
#include "Filled.h"
#include "Decoration.h"
#include "Datastore.h"

class Projectile;
//typedef boost::shared_ptr<Projectile> Projectile_ptr;
typedef Projectile* Projectile_ptr;

class Projectile :
	public BaseEntity
{
protected:
	float damage_;
	float lifetime_;
	Filled fill_;
	int firer_id_;

	static int projectile_spawn_count_;
	static int projectile_free_count_;
public:
	Projectile(void);
	virtual ~Projectile(void);	

	void DrawSelf();
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec,  Matrix4f _transform);
	virtual void Hit(std::vector<Decoration_ptr>& _deco_spawn, std::vector<Projectile_ptr>& _projectile_spawn) = 0;
	
	float GetDamage(){return damage_;}
	float GetLifetime(){return lifetime_;}
	void SetLifetime(float _lifetime){lifetime_ = _lifetime;}

	int GetFirer_ID(){return firer_id_;}
	void SetFirer_ID(int _firer_id){firer_id_ = _firer_id;}

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
