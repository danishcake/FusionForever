#include "vmath.h"

/**
  * Gets the direction to turn to face a point
  * Untested - may be wrong!
  * Expects central coordinates (ie with turning entity at 0,0)
  * @param _point_faced The point currently faced
  * @param _point_to_face The point to face
  * @return A factor between 1 and -1 representing the angle change needed.
  */
extern float GetTurnDirection(Vector3f _point_faced, Vector3f _point_to_face);
/**
  * Gets the direction to turn to face a point
  * Expects central coordinates (ie with turning entity at 0,0)
  * @param _angle The current angle
  * @param _point_to_face The point to face
  * @return A factor between 1 and -1 representing the angle change needed.
  */
extern float GetTurnDirection(float _angle, Vector3f _point_to_face);
/**
  * Gets the direction to turn to face a point
  * Untested - may be wrong!
  * @param _angle The current angle
  * @param _angle_to_face The angle to face
  * @return A factor between 1 and -1 representing the angle change needed.
  */
extern float GetTurnDirection(float _angle, float _angle_to_face);

/**
  * Scales the turn rate so that full speed turns are achieved expect in a centre deadzone
  * @param _turn_rate The result of GetTurnDirection
  * @param _centre_width The size of the central deadzone, between 0 and 1
  * @return A factor between 1 and -1 representing the angle change needed. If |_turn_rate| > |_centre_width| then 
  * +-1, else between 0 and +-1.
  */
extern float ClampTurnDirection(float _turn_rate, float _centre_width);