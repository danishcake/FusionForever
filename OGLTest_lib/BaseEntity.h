#pragma once
#include "vmath.h"
#include "Subscriber.h"

/**
  * The BaseEntity class represents the common properties shared by all entities.
  * A BaseEntity can subscribe to other BaseEntities by using AddSubscriber. When the subscribed to 
  * BaseEntity dies it calls the EndSubscription method on each subscriber. Similarly, a subscriber
  * must call RemoveSubscriber when it dies
  * The EndSubscription method should be overloaded to deal with incoming EndSubscriptions
  */
class BaseEntity : public Subscriber
{
protected:
   /**
     * The position of the entity
     */
	Vector3f position_;
   /**
     * The velocity of the entity
     */
	Vector3f velocity_;
   /**
     * The radius of the entity
     */
	float radius_;
   /**
     * The angle of the entity. This is measured from the y-axis in a CCW direction in degrees
     */
	float angle_;
	/*
	 * Rotational rate
	 */
	float spin_;
	/*
	 * The mass of the entity
	 */
	float mass_;
	/*
	 * The moment of the entity
	 */
	float moment_;
   /**
     * Whether or not the position has been explicitly set.
     * If false then when added as a sub-section the position can be set to a default position.
     * Modified to true by calls to SetPosition.
     */
	bool position_specified_;   //If this is false then sections can set a default positions
   /**
     * The cached transform. Set by calls to BaseEntity::Tick
     */
	Matrix4f ltv_transform_;
   /**
     * The cached position. The position the (0,0,0) ends up when transformed by ltv_transform_.
     */
	Vector3f ltv_position_;
	/*
	 * The previous frame's position
	 */
	Vector3f prev_position_;
public:
   /**
     * Sets up default values for BaseEntity.
     * radius_ = 5, all others zero or identity
     */
	BaseEntity(void);
	virtual ~BaseEntity(void);
   /**
     * Transforms another BaseEntity into the local coordinates of this BaseEntity
     * @param _baseEntity The entity to transform into local coordinates
     */
	BaseEntity* localTransToGlobal(BaseEntity* _baseEntity);
   /**
     * Updates the position according to the velocity and the timespan.
     * Stores new transform in ltv_transform_.
     * @param _timespan The amount of time since the last frame, in seconds.
     * @param _transform The transform of the parent object.
     */
	virtual void Tick(float _timespan, Matrix4f _transform);

   /**
     * Sets the position. Sets position_specified_ to true.
     * @param _position The new position.
     */
	void SetPosition(Vector3f _position) {position_ = _position; position_specified_ = true;}
   /**
     * Gets the position
     * @return The position.
     */
	Vector3f GetPosition() {return position_;}
	/*
	 * Gets the previous position
	 */
	Vector3f GetPreviousPosition() {return prev_position_;}
	/**
     * Sets the velocity.
     * @param _velocity The new velocity.
     */
	void SetVelocity(Vector3f _velocity) {velocity_ = _velocity;}
   /**
     * Gets the velocity.
     * @return The velocity.
     */
	Vector3f GetVelocity() {return velocity_;}

   /**
     * Gets the radius.
     * @return The radius.
     */
	float GetRadius() {return radius_;}

   /**
     * Gets if the position has been specified.
     * @return true if the position has been explicitly specified, otherwise false.
     */
	bool GetPositionSpecified(){return position_specified_;}

   /**
     * Sets the angle.
     * @param _angle The new angle.
     */
	virtual void SetAngle(float _angle) {angle_ = _angle;}
   /**
     * Gets the angle.
     * @return The angle.
     */
	float GetAngle() {return angle_;}

   /**
     * Gets the angle transformed into world space.
     * Useful for orienting projectiles, lasers etc.
     * @return The angle as it is drawn, in world space.in degrees
     */
	float GetGlobalAngle()
	{
		Vector3f v = Vector3f(0,1,0);
		v = ltv_transform_ * v;
		v -= ltv_position_;
		return atan2f(v.x,v.y)*180.0f/M_PI;
	}

	/**
	  * Gets the position in global coordinates.
	  * Useful for obtaining absolute positions of subsections
	  * @return The position 0,0,0 transformed by ltv_transform_
	  */
	Vector3f GetGlobalPosition()
	{
		return ltv_position_;
	}

	void ImpartMomentum(Vector3f _momentum, Vector3f _position);

	float GetMass(){return mass_;}
	void SetMass(float _mass){mass_ = _mass;}

	float GetMoment(){return moment_;}
	void SetMoment(float _moment){moment_ = _moment;}

	float AddMass(float _mass){mass_ += _mass; return mass_;}
	float AddMoment(float _moment){moment_ += _moment; return moment_;}

	float GetSpin(){return spin_;}
	void SetSpin(float _spin){spin_ = _spin;}

	virtual void InitialiseGraphics() = 0;
};
