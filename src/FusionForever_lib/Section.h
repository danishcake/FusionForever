#pragma once
#include "BaseEntity.h"
#include "Filled.h"
#include "Outlined.h"
#include <list>
#include <vector>
#include <algorithm>
#include "Projectile.h"
#include "Decoration.h"
#include "vmath-collisions.h"
#include "Datastore.h"
#include "Camera.h"
#include "FlexFloat.h"
#include "SectionDecoration.h"
#include "SectionMetadata.h"

class Section;
//typedef boost::shared_ptr<Section> Section_ptr;
typedef Section* Section_ptr;
class Core;
class ICollisionManager;
//typedef boost::shared_ptr<Core> Core_ptr;
typedef Core* Core_ptr;

class Property;

static const float SECTION_FLASH_TIME = 2.0f;

//Forward declare TiXML element
class TiXmlElement;

struct ThrusterData
{
	float length_scale;
	float width_scale;
	GLColor color;
	Vector3f position;
	float angle;
	bool angular_correction;
	float antiparallel_factor;
};

class Section :
	public BaseEntity
{
private:
	static int section_count_;
	static int section_freed_;
	bool first_tick_;

	void CalculateTransformedFill();
	void CalculateTransformedOutline();
protected:
	int section_id_;
	std::string section_type_;
	Section_ptr parent_;
	Core_ptr root_;
	Filled fill_;
	std::vector<Vector3f> transformed_fill_verts_;
	std::vector<Vector3f> transformed_outline_verts_;
	bool transformed_fill_verts_valid_;
	bool transformed_outline_verts_valid_;
	Outlined outline_;
	Outlined shield_outline_;

	std::vector<Section_ptr> sub_sections_;
	FlexFloat health_;

	bool has_shield_;
	FlexFloat shield_;
	FlexFloat shield_shock_time_; //After damage only recharge after this long of no damage
	FlexFloat shield_down_time_;  //After total shield destruction restore to 20% after this time
	FlexFloat shield_recharge_rate_; //Recharge rate
	FlexFloat shield_recharge_cost_; //Energy per shield
	FlexFloat shield_radius_;
	float time_since_damage_;


	bool firing_;
	bool ltv_firing_;
	std::vector<float> firing_edges_;
	float firing_delay_;

	float damage_flash_timer_;
	float flash_timer_;
	bool flash_state_;
	GLColor outline_color_base_;
	Vector3f default_sub_section_position_;
	void findRadius();

	void SetParentAndRoot(Section_ptr _parent, Core_ptr _root);

	FlexFloat thrust_;
	FlexFloat energy_;
	FlexFloat power_generation_;
	std::vector<ThrusterData> trails_;

	/*
	 * Drains power from core - expects a negative number
	 */
	void PowerTick(float _power_delta);
	bool PowerRequirement(float _minimum_power);
	void AddEnergyCap(float _energy_cap) {energy_.AddMaxValue(_energy_cap);}

public:
	Section(void);
	virtual ~Section(void);
	void AddChild(Section * child);
	virtual void DrawSelf();
	/* Checks collisions with projectile, nonrecursive */
	bool CheckCollisions(Projectile_ptr _projectile);
	/* Checks between a coordinate and section, recursive*/
	bool CheckCollisions(Vector3f _location, Section_ptr& _section);
	/* Checks collisions between a coordinate and sections, recursively, builds a list of collisions*/
	void CheckCollisions(Vector3f _location, std::vector<Section*>& _sections);
	/* Checks collisions between a line segment and sections, nonrecursive */
	bool CheckCollisions(const Vector3f _lineP1, const Vector3f _lineP2, Vector3f& _collision_point);
	/* Checks collisions between self and another section, nonrecursive */
	bool CheckCollisions(Section* _section);
	void RayCollisionFilter(Vector3f P1, Vector3f P2, std::vector<Section_ptr>& _valid_sections, float& _min_distance, float& _max_distance);
	void SetColor(GLColor _color);
	void SetOutlineColor(GLColor _color);
	GLColor GetColor();
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);
	void DeathTick();
	void CollectShields(std::vector<Section_ptr>& _shields);
	virtual void GetDeathSpawn(std::vector<Decoration_ptr>& _spawn_dec);

	//Getters/Setters
	const float GetHealth(){return health_.GetValue();}
	const float GetMaxHealth(){return health_.GetMaxValue();}
	void SetMaxHealth(float _max_health);

	void TakeDamage(float _damage, int _section_id);
	void SetFlashing(){flash_state_ = true; damage_flash_timer_ = 100000.0f;} //Two mechanisms for flashing, damage and this method. Settings flash timer prevent conflict
	void SetNotFlashing(){flash_state_ = false; damage_flash_timer_ = 0.0f;}
	void ScaleHealth(float _factor);
	void SetFiring(bool _firing);
	void SetFiringDelay(float _firing_delay) {firing_delay_ = _firing_delay;}
	float GetFiringDelay(){return firing_delay_;}
	int GetSectionID(){return section_id_;}
	Section* GetSectionByID(int _section_id);

	bool IsCore(){return root_ == NULL;}
	Core_ptr GetRoot();
	Section_ptr GetParent(){return parent_;}

	bool HasShield(){return has_shield_;}
	float GetShieldRadius(){return shield_radius_.GetValue();}
	float GetShieldHealth(){return shield_.GetValue();}

	//Predicates
	static bool IsRemovable(Section_ptr section)
	{
		bool dead = (section->health_ <= 0);
		if(dead)
		{
			Camera::Instance().Shake(0.3f);
		}
		return dead;
	}
	//Editor support
	std::vector<Section_ptr> DetachChildren();
	void AttachChildren(std::vector<Section_ptr> _children);
	/*
	 * Draws any editor only stuff
	 */
	void DrawEditorSupport(float _grid_size, Section_ptr _selected);
	/*
	 * Gets a list of gettable/settable properties for the editor. 
	 * Override and then call to get the basics
	 */
	virtual void GetProperties(std::vector<Property*>& _properties );

	/*
	 * To be overriden by call sub classes, then called to set the basics
	 */
	virtual void ToXML(TiXmlElement* _node);

	/*
	 * To be overriden by call sub classes that require special parameters.
	 * @return False on missing required elements.
	 */
	virtual bool ParseSpecific(TiXmlElement* _node);

	void SaveToXML(std::string _filename);

	/* Private and protected methods exposed for unit testing purposes */
	void UnitTestPowerTick(float _power_delta){PowerTick(_power_delta);}

	/* Lua construction support */
	std::string GetSectionType(){return section_type_;}
	std::vector<Section_ptr>& GetSubsections(){return sub_sections_;}

	/*
	 * Register section metadata - section name associated with a number of tags, values and coordinates
	 */
	virtual void RegisterMetadata();
};
