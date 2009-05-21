#pragma once
#include "FiringSection.h"

/**
  * The FlakCannon fires a swarm of fast projectiles in a cone
  */
class FlakCannon :
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

public:
   /**
     * Creates a FlakCannon
     */
	FlakCannon(void);
	virtual ~FlakCannon(void);

   /**
     * Ticks the FlakCannon
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_prj Any projectiles fired.
     * @param _spawn_dec Any decorations created.
     * @param _transform The parent transform.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);

	virtual void ToXML(TiXmlElement* _node);
};
