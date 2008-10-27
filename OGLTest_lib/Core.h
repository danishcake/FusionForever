#pragma once
#include "Section.h"

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
	/*
	 * The target for subsections to aim at
	 */
	Core* target_;
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
	 * Gets the target for subsections to aim at
	 */
	Core* GetTarget(){return target_;}

	/*
	 * Gets the energy of the Core
	 */
	FlexFloat GetEnergy(){return energy_;}
	/*
	 * Adds an amount of thrust
	 */
	void AddThrust(FlexFloat _thrust_delta) {thrust_ += _thrust_delta;}
	/*
	 * Adds an amount of thrust
	 */
	void AddThrust(float _thrust_delta) {thrust_ += _thrust_delta;}
	/*
	 * Creates a core from an XML file
	 */
	static Core* CreateCore(std::string _filename);
	/*
	 * Parses the ship from the root element
	 */
	static bool ParseShip(TiXmlElement* _section, Section** _parent);
	/*
	 * Parses the core element - gets core only attributes
	 */
	static Core* ParseCore(TiXmlElement* _core);
	/*
	 * Parses any non core elements, gets non-core attributes
	 */
	static Section* ParseSection(TiXmlElement* _section_element);
	/*
	 * Parses any attributes common to cores and sections
	 */
	static void ParseCommon(TiXmlElement* _section_element, Section* _section);

	virtual void EndSubscription(Subscriber* _source);
};


