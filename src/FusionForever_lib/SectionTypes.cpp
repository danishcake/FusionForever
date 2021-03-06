#include "StdAfx.h"
#include "SectionTypes.h"
#include "SectionMetadata.h"
#include "Section.h"

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
#include "Deterer.h"
#include "FlakCannon.h"
#include "HeavyChainGun.h"
#include "SmartBomber.h"
#include "PhotonCannon.h"
#include "ShotgunArtillery.h"
#include "HeavyBlaster.h"
#include "PlasmaFlinger.h"
#include "PlasmaSpitter.h"
#include "LightDestroyer.h"
#include "XMLSection.h"
#include "XMLCore.h"
#include <set>

namespace SectionTypes
{
	std::map<std::string, Section*(*)()> p_map;
	std::set<std::string> core_set;

	Section* GetSection(std::string _name)
	{
		if(p_map.find(_name) != p_map.end())
		{
			if(p_map[_name] == NULL)
			{
				return XMLSection::CreateXMLSection(_name);
			} 
			else
			{
				return p_map[_name]();
			}
		} else
		{
			return NULL;
		}
	}

	std::vector<std::string> GetSectionNames()
	{
		std::vector<std::string> names;
		std::pair<std::string, Section*(*)()> entry;
		BOOST_FOREACH(entry, p_map)
		{
			names.push_back(entry.first);
		}
		return names;
	}

	std::vector<std::string> GetCoreNames()
	{
		std::vector<std::string> names;
		BOOST_FOREACH(std::string entry, core_set)
		{
			names.push_back(entry);
		}
		return names;
	}

	void RegisterSectionType(Section* (*_factory_method)(), std::string _name)
	{
		p_map[_name] = _factory_method;

		Section* section = _factory_method();
		section->RegisterMetadata();
		SectionMetadata::RegisterSection(_name);
		delete section;
	}

	void RegisterXMLSectionType(std::string _name)
	{
		p_map[_name] = NULL;
	}

	void RegisterXMLCoreType(std::string _name)
	{
		core_set.insert(_name);
	}

/* Factory methods - creates and instance of the section. Registered with
   a global map via the static variable below */

	Section_ptr CreatePlasmaArtillery(){return new PlasmaArtillery();}
	Section_ptr CreateSwarmerInstance(){return new Swarmer();}
	Section_ptr CreateSpinningJointInstance(){return new SpinningJoint();}
	Section_ptr CreateJointTrackerInstance(){return new JointTracker();}
	Section_ptr CreateJointAnglesInstance(){return new JointAngles();}
	Section_ptr CreateHomingMissileLauncherInstance(){return new HomingMissileLauncher();}
	Section_ptr CreateHeatBeamGunInstance(){return new HeatBeamGun();}
	Section_ptr CreateChainGunInstance(){return new ChainGun();}
	Section_ptr CreateBlasterInstance(){return new Blaster();}
	Section_ptr CreateTrackerArmInstance(){return new TrackerArm();}
	Section_ptr CreateLancerInstance(){return new Lancer();}
	Section_ptr CreateBurnerInstance(){return new Burner();}
	Section_ptr CreateDetererInstance(){return new Deterer();}
	Section_ptr CreateFlakCannonInstance(){return new FlakCannon();}
	Section_ptr CreateHeavyChainGunInstance(){return new HeavyChainGun();}
	Section_ptr CreateSmartBomberInstance(){return new SmartBomber();}
	Section_ptr CreatePhotonCannonInstance(){return new PhotonCannon();}
	Section_ptr CreateShotgunArtilleryInstance(){return new ShotgunArtillery();}
	Section_ptr CreateHeavyBlasterInstance(){return new HeavyBlaster();}
	Section_ptr CreatePlasmaFlingerInstance(){return new PlasmaFlinger();}
	Section_ptr CreatePlasmaSpitterInstance(){return new PlasmaSpitter();}
	Section_ptr CreateLightDestroyerInstance(){return new LightDestroyer();}


	void RegisterSections()
	{
		XMLSection::Preload();
		XMLCore::Preload();
		SectionTypes::RegisterSectionType(CreatePlasmaArtillery, "PlasmaArtillery");
		SectionTypes::RegisterSectionType(CreateSwarmerInstance, "Swarmer");
		SectionTypes::RegisterSectionType(CreateSpinningJointInstance, "SpinningJoint");
		SectionTypes::RegisterSectionType(CreateJointTrackerInstance, "JointTracker");
		SectionTypes::RegisterSectionType(CreateJointAnglesInstance, "JointAngles");
		SectionTypes::RegisterSectionType(CreateHomingMissileLauncherInstance, "HomingMissileLauncher");
		SectionTypes::RegisterSectionType(CreateHeatBeamGunInstance, "HeatBeamGun");
		SectionTypes::RegisterSectionType(CreateChainGunInstance, "ChainGun");
		SectionTypes::RegisterSectionType(CreateBlasterInstance, "Blaster");
		SectionTypes::RegisterSectionType(CreateTrackerArmInstance, "TrackerArm");
		SectionTypes::RegisterSectionType(CreateLancerInstance, "Lancer");
		SectionTypes::RegisterSectionType(CreateBurnerInstance, "Burner");
		SectionTypes::RegisterSectionType(CreateDetererInstance, "Deterer");
		SectionTypes::RegisterSectionType(CreateFlakCannonInstance, "FlakCannon");
		SectionTypes::RegisterSectionType(CreateHeavyChainGunInstance, "HeavyChainGun");
		SectionTypes::RegisterSectionType(CreateSmartBomberInstance, "SmartBomber");
		SectionTypes::RegisterSectionType(CreatePhotonCannonInstance, "PhotonCannon");
		SectionTypes::RegisterSectionType(CreateShotgunArtilleryInstance, "ShotgunArtillery");
		SectionTypes::RegisterSectionType(CreateHeavyBlasterInstance, "HeavyBlaster");
		SectionTypes::RegisterSectionType(CreatePlasmaFlingerInstance, "PlasmaFlinger");
		SectionTypes::RegisterSectionType(CreatePlasmaSpitterInstance, "PlasmaSpitter");
		SectionTypes::RegisterSectionType(CreateLightDestroyerInstance, "LightDestroyer");
	}
}