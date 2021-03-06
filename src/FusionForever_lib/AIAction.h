#pragma once

class Core;

/**
  *The AIAction class represents an action to be performed by an AI.
  */
class AIAction
{
public:
   /**
     * Creates a zero'd AIAction
     */
	AIAction();
   /**
     * Creates an AIAction
     * @param _dx Clamped x-axis acceleration (-1 to 1)
     * @param _dy Clamped y-axis acceleration (-1 to 1)
     * @param _dtheta Normalised angular acceleration
	 * @param _max_turn Maximum angle to turn in one tick. Typically the total angle turn required
     * @param _firing Firing state
     */
	AIAction(float _dx, float _dy, float _dtheta, bool _firing, bool _thrust, Core* _target = NULL);

   /**
     * Clamped x-axis acceleration (-1 to 1)
     */
	float dx_;
   /**
     * Clamped y-axis acceleration (-1 to 1)
     */
	float dy_;
   /**
     * Clamped angular acceleration (-1 to 1)
     */
	float dtheta_;
	/**
	  * Firing state
	  */
	bool firing_;
	/**
	  * Thrusting state
	  */
	bool thrust_;
	/**
	  * A target for sections to aim at, missiles to home on etc.
	  */
	Core* target_;
};