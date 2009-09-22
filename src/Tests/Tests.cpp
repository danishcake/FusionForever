// test.cpp
#include <UnitTest++.h>
#include "TurningRoutines.h"

TEST(CheckTurningRoutines)
{
  //Create three points on a triangle
  /*
   2
      3
   1  
  0
  */
  Vector3f p1 = Vector3f(10,10,0);
  Vector3f p2 = Vector3f(10,30,0);
  Vector3f p3 = Vector3f(30,20,0);

  CHECK(GetTurnDirection(p2-p1, p3-p1).turn_factor > 0);
  CHECK(GetTurnDirection(p2-p3, p1-p3).turn_factor < 0);
  CHECK(GetTurnDirection(0, p3-p1).turn_factor > 0);
  CHECK(GetTurnDirection(90, p3-p1).turn_factor < 0);
  CHECK(GetTurnDirection(0,45).turn_factor > 0);
  CHECK(GetTurnDirection(270,245).turn_factor < 0);
  CHECK_CLOSE(GetTurnDirection(25,25).turn_factor, 0, 0.01f);
}

static const float DEGTORAD = 3.14159265f / 180.0f;

TEST(CheckCoordinateSystem)
{
	Vector3f p1 = Vector3f(0,10,0);
	Matrix4f rot_90 = Matrix4f::createRotationAroundAxis(0,0, 90 * DEGTORAD);
	Vector3f p2 = rot_90 * p1;
	CHECK_CLOSE(10, p2.x, 0.001f);
}

int main()
{
  return UnitTest::RunAllTests();
}