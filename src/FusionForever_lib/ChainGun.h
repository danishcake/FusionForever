#pragma once
#include "FiringSection.h"
#include "SmallBullet.h"


/**
  * The ChainGun class represents a section that fires smallbullets slowly at first, accelerating to extremely fast
  */
class ChainGun :
	public FiringSection
{
protected:
   /**
     * Triggers initialisation.
     * Defaults to false. Constructor should check for false and
     * call the InitialiseGraphics functions, then set to true. This ensure the class is
     * only initialised once. This should speed up all subsequent constructions.
     */
	static bool initialised_;
   /**
     * Stores the display list of the outline of the section.
     */
	static int outline_dl_;
   /**
     * Stores the index of the outline vertices in DataStore.
     */
	static int outline_verts_index_;
   /**
     * Stores the display list of the fill of the section.
     */
	static int fill_dl_;
   /**
     * Stores the index of the fill vertices in DataStore.
     */
	static int fill_verts_index_;
   /*
    * Initialises the display list and collision geometry on first instantiation
    */
	virtual void InitialiseGraphics();
	/*
	 * The spun_up state, between 0 and 1
	 */
	float spun_up_factor_;
	/*
	 * The highest rate of fire
	 */
	static float minimum_fire_time_;
	/*
	 * The lowest rate of fire
	 */
	static float start_fire_time_;
	/*
	 * The spin up time
	 */
	static float spin_up_time_;
	/*
	 * The time before the second phase is entered
	 */
	static float phase_two_time_;
	/*
	 * The time continuously fired for
	 */
	float firing_time_;
	float ltv_firing_time_;

public:
   /**
     * Creates a ChainGun
     */
	ChainGun(void);
	virtual ~ChainGun(void);

   /**
     * Ticks the ChainGun
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_prj Any projectiles fired.
     * @param _spawn_dec Any decorations created.
     * @param _transform The parent transform.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);

	/*
	 * Register section metadata - section name associated with a number of 
	 * tags, values and coordinates
	 */
	virtual void RegisterMetadata();
};
