#include "stdafx.h"
#include "ListAdder.h"
#include "PlasmaArtillery.h"
#include "Swarmer.h"
#include "SpinningJoint.h"
#include "JointAngles.h"
#include "HomingMissileLauncher.h"
#include "HeatBeamGun.h"
#include "ChainGun.h"
#include "Blaster.h"
#include "TrackerArm.h"
#include "JointTracker.h"
#include "Lancer.h"
#include "Burner.h"

/* Factory methods - creates and instance of the section. Registered with
   a global map via the static variable below */

static Section_ptr CreatePlasmaArtillery(){return new PlasmaArtillery();}
static Section_ptr CreateSwarmerInstance(){return new Swarmer();}
static Section_ptr CreateSpinningJointInstance(){return new SpinningJoint();}
static Section_ptr CreateJointTrackerInstance(){return new JointTracker();}
static Section_ptr CreateJointAnglesInstance(){return new JointAngles();}
static Section_ptr CreateHomingMissileLauncherInstance(){return new HomingMissileLauncher();}
static Section_ptr CreateHeatBeamGunInstance(){return new HeatBeamGun();}
static Section_ptr CreateChainGunInstance(){return new ChainGun();}
static Section_ptr CreateBlasterInstance(){return new Blaster();}
static Section_ptr CreateTrackerArmInstance(){return new TrackerArm();}
static Section_ptr CreateLancerInstance(){return new Lancer();}
static Section_ptr CreateBurnerInstance(){return new Burner();}



void RegisterSections()
{
	ListAdder PlasmaArtillery_l = ListAdder(CreatePlasmaArtillery, "PlasmaArtillery");
	ListAdder Swarmer_l = ListAdder(CreateSwarmerInstance, "Swarmer");
	ListAdder SpinningJoint_l = ListAdder(CreateSpinningJointInstance, "SpinningJoint");
	ListAdder JointTracker_l = ListAdder(CreateJointTrackerInstance, "JointTracker");
	ListAdder JointAngles_l = ListAdder(CreateJointAnglesInstance, "JointAngles");
	ListAdder HomingMissileLauncher_l = ListAdder(CreateHomingMissileLauncherInstance, "HomingMissileLauncher");
	ListAdder HeatBeamGun_l = ListAdder(CreateHeatBeamGunInstance, "HeatBeamGun");
	ListAdder ChainGun_l = ListAdder(CreateChainGunInstance, "ChainGun");
	ListAdder Blaster_l = ListAdder(CreateBlasterInstance, "Blaster");
	ListAdder TrackerArm_l = ListAdder(CreateTrackerArmInstance, "TrackerArm");
	ListAdder Lancer_l = ListAdder(CreateLancerInstance, "Lancer");
	ListAdder Burner_l = ListAdder(CreateBurnerInstance, "Burner");
}