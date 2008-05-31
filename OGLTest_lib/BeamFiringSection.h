#pragma once
#include "Section.h"
#include "BeamSection.h"

/**
  * The BeamFiringSection represents a section that fires a beam.
  */
class BeamFiringSection :
	public Section
{
protected:
   /**
     * The beam to be tick and drawn when firing.
     */
	BeamSection_ptr beam_;
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
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies);
   /**
     * Draws the BeamFiringSection and when firing, the beam as well.
     */
	virtual void DrawSelf();
};
