#pragma once
#include "FiringSection.h"
#include "SmallBullet.h"


/**
  * The Blaster class represents a section that fires smallbullets rapidly
  */
class Blaster :
	public FiringSection
{
protected:
   /**
     * Initialises the outline vertices
     */
	virtual void initialise_outline();
   /**
     * Initialises the fill.
     * This should be called after initialise_outline and make use of the same vertices.
     */
	virtual void initialise_fill();
   /**
     * Triggers initialisation.
     * Defaults to false. Constructor should check for false and
     * call the initialise_* functions, then set to true. This ensure the class is
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

public:
   /**
     * Creates a blaster
     */
	Blaster(void);
	virtual ~Blaster(void);

   /**
     * Ticks the blaster
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_prj Any projectiles fired.
     * @param _spawn_dec Any decorations created.
     * @param _transform The parent transform.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies);
};