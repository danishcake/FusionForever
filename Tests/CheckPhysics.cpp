#include <UnitTest++.h>
#include <Vector>
#include <list>
#include "TestBaseEntity.h"
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<BaseEntity> be_ptr;

TEST(CheckPhysics)
{
	be_ptr p_be = be_ptr(new TestBaseEntity());
	CHECK_CLOSE(1, p_be->GetMass(), 0.00001f); //Default mass is 1
	CHECK_CLOSE(1, p_be->GetMoment(), 0.00001f); //Default moment is 1
	//Bash to the left
	p_be->ImpartMomentum(Vector3f(10,0,0), Vector3f(0,0,0));
	CHECK_CLOSE(10, p_be->GetVelocity().x, 0.00001f);
	// Bash upwards
	p_be->ImpartMomentum(Vector3f(0,10,0), Vector3f(0,0,0));
	CHECK_CLOSE(10, p_be->GetVelocity().y, 0.00001f);
	//Bash back right and down
	p_be->SetMass(2);
	p_be->ImpartMomentum(Vector3f(-20,-20,0), Vector3f(0,0,0));
	CHECK_CLOSE(0, p_be->GetVelocity().length(), 0.00001f);

	p_be->ImpartMomentum(Vector3f(10,0,0), Vector3f(0,200, 0));
	//Should have 50% to rotation, 50% to momentum
	CHECK_CLOSE(2.5, p_be->GetVelocity().x, 0.0001f);
	CHECK_CLOSE(250000, p_be->GetSpin(), 0.00001f);
}
