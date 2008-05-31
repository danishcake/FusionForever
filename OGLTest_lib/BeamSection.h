#pragma once
#include "vmath.h"
#include "vmath-collisions.h"
#include "Decoration.h"
#include "Core.h"
#include "BaseEntity.h"

/**
  * A BeamSection smart pointer
  */
class BeamSection;
//typedef boost::shared_ptr<BeamSection> BeamSection_ptr;
typedef BeamSection* BeamSection_ptr;

/**
  * The BeamSection class represents a laser-like beam.
  */
class BeamSection : public BaseEntity
{
protected:
   /**
     * The maximum distance the beam will reach
     */
	float max_distance_;
   /**
     * The rate of damage
     */
	float damage_per_second_;
   /**
     * The distance that the last collision occurred at.
     */
	float distance_;
   /**
     * The time until the next decoration can spawn. Resets to 0.025f (40hz).
     */
	float deco_cooldown_;
public:
   /**
     * Creates a BeamSection with a maximum distance of 100.
     */
	BeamSection(void);
	virtual ~BeamSection(void);

   /**
     * Draws the beam section
     */
	virtual void DrawSelf() = 0;
   /**
     * Finds collisions between enemies and the beam and inflicts damage.
     * Also spawns sparks where the collision occurred.
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_dec Any decorations created by the BeamFiringSection.
     * @param _transform The parent transform.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies);
};
