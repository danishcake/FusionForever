#pragma once
#include "Section.h"
#include "AIAction.h"

/*
The BaseAI class and the Section class are fairly tightly coupled and are so declared in
the same header to resolve circular dependancies
*/

//Forward declare the BaseAI class.
class BaseAI;
//Forward declare TiXML element
class TiXmlElement;

/**
  * The Core class represents a root section onto which other sections are attached
  */
class Core :
	public Section
{
protected:
   /**
     * The AI_ controls the behaviour of the Core - it is 'consulted' during the Tick method
     */
	BaseAI* AI_;
public:
   /**
     * Creates a Core.
     * @param _AI The AI to control the core
     */
   Core(BaseAI* _AI);
   /**
     * Clears up the AI
     */
	virtual ~Core(void);

   /**
     * Ticks the Core and it's AI.
     * @param _timespan The time in seconds since the last frame.
     * @param _spawn_prj Any projectiles fired.
     * @param _spawn_dec Any decorations created.
     * @param _transform The parent transform.
     * @param _friends A list of friends.
     * @param _enemies A list of enemies.
     */
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec,
					  Matrix4f _transform, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager);

  /**
  * Sets the AI to a new AI, clearing up the old one in the process.
  * @param _new_AI The new AI
  */
	void OverrideAI(BaseAI* _new_AI);

	/*
	 * Gets the energy of the Core
	 */
	FlexFloat GetEnergy(){return energy_;}

	/*
	 * Creates a core from an XML file
	 */
	static Core* CreateCore(std::string _filename);

	static bool ParseShip(TiXmlElement* _section, Section** _parent);

	static Core* ParseCore(TiXmlElement* _core);

	static Section* ParseSection(TiXmlElement* _section);
};


/**
  * The BaseAI class represents a AI that makes decisions about what a core should do
  */
class BaseAI : public Subscriber
{
public:
	BaseAI(void) {};
	virtual ~BaseAI(void) {};

   /**
     * Ticks the AI
     * @param _timespan The time in seconds since the last frame.
     * @param _friends A list of friends.
     * @param _enemies A list of enemies.
     * @param _self The owning core.
     */
	virtual AIAction Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self) = 0;
};
