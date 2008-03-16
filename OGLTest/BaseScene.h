#pragma once
#include <vector>

class BaseScene;
/**
  * A scene smart pointer
  */
//typedef boost::shared_ptr<BaseScene> BaseScene_ptr;
typedef BaseScene* BaseScene_ptr;

/**
  * The BaseScene class represents the methods that must be overriden by a scene.
  * The scenes are stored in a flat list and drawn in order.
  */
class BaseScene
{
public:
	BaseScene(void);
	virtual ~BaseScene(void);

   /**
     * The Tick method controls the activity in the scene.
     * Typically used to tick a list of BaseEntities, handle menus etc.
     * @param _timespan The period of time since the last frame.
     * @param _new_scenes A vector of new scenes that are added in order to the end of the scene list.
     */
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes) = 0;

   /**
     * The draw method triggers the drawing of the scenes contents.
     */
	virtual void Draw() = 0;
   /**
     * The IsRoot method allows the game to cull scenes that are not meant to be ticked.
     * Any scenes with a lower index than the last root scene will not be drawn or ticked.
     * @return True if scene is a root scene, otherwise false.
     */
	virtual bool IsRoot() = 0;
   /**
     * If IsRemovable returns true then the scene can be deleted from the scene vector
     * @return True if scene is removable, otherwise false.
     */
	virtual bool IsRemovable() = 0;
};
