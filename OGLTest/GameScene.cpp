#include "StdAfx.h"
#include "GameScene.h"

#include "Camera.h"
#include <boost/foreach.hpp>

#include "SquareCore.h"
#include "RotatingAI.h"
#include "KeyboardAI.h"
#include "RigidArm.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"

void GameScene::initSections()
{
	Core_ptr core = Core_ptr(new SquareCore(new KeyboardAI()));
	ownship_ = core;
	friends_.push_back(core);

	Section* s_s;
	Section* s_s2;
	Blaster* b;
	HeatBeamGun* beam;

	beam = new HeatBeamGun();
	beam->SetPosition(Vector3f(7.5,7.5,0));
	beam->SetAngle(1);
	core->AddChild(beam);

	beam = new HeatBeamGun();
	beam->SetPosition(Vector3f(-7.5,7.5,0));
	beam->SetAngle(-1);
	core->AddChild(beam);

	beam = new HeatBeamGun();
	beam->SetPosition(Vector3f(2.5,7.5,0));
	core->AddChild(beam);

	beam = new HeatBeamGun();
	beam->SetPosition(Vector3f(-2.5,7.5,0));
	core->AddChild(beam);

	b = new Blaster();
	b->SetPosition(Vector3f(9.5f,0,0));
	b->SetAngle(-10);
	core->AddChild(b);

	b = new Blaster();
	b->SetPosition(Vector3f(-9.5f,0,0));
	b->SetAngle(10);
	core->AddChild(b);

	core->SetColor(GLColor(0,0,200));

	/* //The simple 4 blaster two laser core
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(-4,10,0));
	b = new Blaster();
	b->SetPosition(Vector3f(0,7.5,0));
	b->SetAngle(30);
	s_s->AddChild(b);
	core->AddChild(s_s);

	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(4,10,0));
	b = new Blaster();
	b->SetAngle(-30);
	b->SetPosition(Vector3f(0,7.5,0));
	s_s->AddChild(b);

	b = new Blaster();
	b->SetPosition(Vector3f(10,2,0));
	b->SetAngle(-30);
	core->AddChild(b);

	b = new Blaster();
	b->SetPosition(Vector3f(-10,2,0));
	b->SetAngle(30);
	core->AddChild(b);

	core->AddChild(s_s);

	Section * spin_joint = new SpinningJoint(60);
	spin_joint->SetPosition(Vector3f(5,0,0));
	s_s = new HeatBeamGun();
	spin_joint->AddChild(s_s);
	core->AddChild(spin_joint);
	
	spin_joint = new SpinningJoint(-60);
	spin_joint->SetPosition(Vector3f(-5,0,0));
	s_s = new HeatBeamGun();
	spin_joint->AddChild(s_s);
	core->AddChild(spin_joint);

	

	core->SetColor(GLColor(0,128,0));
*/

	for(int i = 0; i < 20; i ++)
	{
		Core_ptr en_core = Core_ptr(new SquareCore(new RotatingAI(0.2f)));
		s_s = new RigidArm();
		s_s->SetPosition(Vector3f(4,10,0));
		en_core->AddChild(s_s);
		s_s = new RigidArm();
		s_s->SetPosition(Vector3f(-4,10,0));
		en_core->AddChild(s_s);
		s_s = new RigidArm();
		s_s->SetPosition(Vector3f(4,-10,0));
		s_s->SetAngle(180);
		en_core->AddChild(s_s);
		s_s = new RigidArm();
		s_s->SetPosition(Vector3f(-4,-10,0));
		s_s->SetAngle(180);
		en_core->AddChild(s_s);
		en_core->SetPosition(Vector3f(40 * i,20,0));
		
		Blaster* b = new Blaster();
		b->SetPosition(Vector3f(0,5,0));
		en_core->AddChild(b);

		enemies_.push_back(en_core);
	}
}



GameScene::GameScene(void)
{
	initSections();
	Camera::Instance().SetWidth(500);
}

GameScene::~GameScene(void)
{
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	std::list<Projectile_ptr> enemy_spawn;
	std::list<Projectile_ptr> ownship_spawn;
	std::list<Decoration_ptr> decoration_spawn;

	const Matrix4f identity = Matrix4f();

	Camera::Instance().TickCamera(_timespan);

	BOOST_FOREACH(Core_ptr core, friends_)
	{
		core->Tick(_timespan, ownship_spawn, decoration_spawn, identity, friends_ , enemies_);
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		core->Tick(_timespan, enemy_spawn, decoration_spawn, identity, friends_ , enemies_);
	}

	enemy_projectiles.splice(enemy_projectiles.begin(), enemy_spawn);
	ownship_projectiles.splice(ownship_projectiles.begin(), ownship_spawn);

	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->Tick(_timespan, identity);

		BOOST_FOREACH(Core_ptr core, friends_)
		{
			core->CheckCollisions(*it);
			if((*it)->GetLifetime()<=0)
			{
				(*it)->Hit(decoration_spawn);
				break;
			}
		}
	}

	for(std::list<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->Tick(_timespan, identity);

		BOOST_FOREACH(Core_ptr core, enemies_)
		{
			core->CheckCollisions(*it);
			if((*it)->GetLifetime()<=0)
			{
				(*it)->Hit(decoration_spawn);
				break;
			}
		}
	}

	BOOST_FOREACH(Core_ptr core, friends_)
	{
		if(core->GetHealth() <= 0)
			core->GetDeathSpawn(decoration_spawn);
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		if(core->GetHealth() <= 0)
			core->GetDeathSpawn(decoration_spawn);
	}

	decorations.splice(decorations.begin(), decoration_spawn);

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->Tick(_timespan, identity);
	}

	enemy_projectiles.remove_if(Projectile::IsProjectileRemovable);
	ownship_projectiles.remove_if(Projectile::IsProjectileRemovable);
	decorations.remove_if(Decoration::IsRemovable);
	friends_.remove_if(Section::IsRemovable);
	enemies_.remove_if(Section::IsRemovable);
}

void GameScene::Draw()
{
	glColor3f(0.0f,0.0f,0.0f);

	starfield_.DrawStarfield(ownship_->GetPosition() );
	BOOST_FOREACH(Core_ptr core, friends_)
	{
		core->DrawSelf();
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		core->DrawSelf();
	}

	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::list<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->DrawSelf();
	}
}

bool GameScene::IsRoot()
{
	return true;
}

bool GameScene::IsRemovable()
{
	return false;
}