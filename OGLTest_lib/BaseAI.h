#pragma once
#include "AIAction.h"
#include "Subscriber.h"

class Core;

/**
  * The BaseAI class represents a AI that makes decisions about what a core should do
  */
class BaseAI : 
	public Subscriber
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
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self) = 0;

   /**
     * Allows the challenges to specify a target to attack
     * @param _target The new target to attack
     */
	virtual void SpecifyTarget(Core* _target) = 0;

	/**
	  * Checks if the AI is human controlled, and therefore wants a health bar
	  */
	virtual bool IsHuman(){return false;}
};
