#include <UnitTest++.h>
#include <vmath-collisions.h>
#include <vector>

TEST(CheckBeamCollisions)
{
	std::vector<Vector3f> hull;
	hull.push_back(Vector3f(0,0,0));
	hull.push_back(Vector3f(50,0,0));
	hull.push_back(Vector3f(50,50,0));
	hull.push_back(Vector3f(0,50,0));


	//Cast rays from both sides and check closest point is returned each time
	Vector3f lineAP1 = Vector3f(10,-50,0);
	Vector3f lineAP2 = Vector3f(10,100,0);

	Vector3f lineBP1 = Vector3f(10,100,0);
	Vector3f lineBP2 = Vector3f(10,-50,0);

	Vector3f collision_point_A;
	Vector3f collision_point_B;

	bool should_collide_A = Collisions2f::LineInPolygon(lineAP1, lineAP2, &hull[0], hull.size(), collision_point_A);
	bool should_collide_B = Collisions2f::LineInPolygon(lineBP1, lineBP2, &hull[0], hull.size(), collision_point_B);

	CHECK(should_collide_A);
	CHECK_CLOSE(10, collision_point_A.x, 0.001f);
	CHECK_CLOSE(0, collision_point_A.y, 0.001f);

	CHECK(should_collide_B);
	CHECK_CLOSE(10, collision_point_B.x, 0.001f);
	CHECK_CLOSE(50, collision_point_B.y, 0.001f);



}