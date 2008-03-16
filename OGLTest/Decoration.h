#pragma once
#include "BaseEntity.h"
#include "Filled.h"


class Decoration;
/**
  * A Decoration smart pointer
  */
//typedef boost::shared_ptr<Decoration> Decoration_ptr;
typedef Decoration* Decoration_ptr;

/**
  * The Decoration class represent a solid filled shape such as an explosion or spark.
  * It is not intented to interact with other entities.
  */
class Decoration :
	public BaseEntity, public Filled
{
protected:
   /**
     * The amount of time left for the decoration to live
     */
	float lifetime_;
public:
	Decoration(void);
	virtual ~Decoration(void);

   /**
     * Ticks the decoration, updating scale and position.
     * @param _timespan The amount of time that has passed since the last frame.
     * @param _transform The parent transform
     */
	virtual void Tick(float _timespan, Matrix4f _transform);
   /**
     * Draws the decoration
     */
	void DrawSelf();
   /**
     * Gets if the Decoration is ready for culling.
     * @return True is ready for culling, else false.
     */
	static bool IsRemovable(Decoration_ptr dec)
	{
		if(dec->lifetime_ <= 0)
		{
			delete dec;
			return true;
		}
		return false;
	}

   /**
     * Gets the remaining lifetime of the Decoration.
     * @return The lifetime left in seconds
     */
	float GetLifetime(){return lifetime_;}
};
