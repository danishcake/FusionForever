#pragma once
#include "Section.h"
#include "BeamSection.h"
#include "BeamCharge.h"

/**
  * The BeamFiringSection represents a section that fires a beam.
  */
class BeamFiringSection :
	public Section
{
protected:
	/**
	  * The charging animation
	  */
	BeamCharge beam_charge_;
    /**
     * The beam to be ticked and drawn when firing.
     */
	BeamSection_ptr beam_;
	/*
	 * Beam charge up time
	 */
	float beam_charge_time_;
	/*
	 * Beam firing time
	 */
	float beam_fire_time_;
	/*
	 * Beam cooldown time - cools down for this long before charging can begin again
	 */
	float beam_cooldown_time_;
	/*
	 * Beam time, position of beam weapon in charge-discharge-cooldown cycle
	 */
	float beam_sum_time_;
	/*
	 * Beam last frame firing value
	 */
	bool ltv_firing_;
	/*
	 * Beam decoration spawn counter
	 */
	float beam_deco_spawn_;
public:
   /**
     * Creates a BeamFiringSection.
     */
	BeamFiringSection(void);
	virtual ~BeamFiringSection(void);

   /**
     * Ticks the beam, which finds any collisions.
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_prj Any projectiles fired by the BeamFiringSection.
     * @param _spawn_dec Any decorations created by the BeamFiringSection.
     * @param _transform The parent transform.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);
   /**
     * Draws the BeamFiringSection and when firing, the beam as well.
     */
	virtual void DrawSelf();
};
