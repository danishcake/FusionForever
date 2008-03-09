#pragma once

class HomingProjectile;
class Section;

class HomingJoin
{
private:
	HomingProjectile* a_;
	Section* b_;
public:
	HomingJoin(HomingProjectile* _a, Section* _b);
	~HomingJoin();

	/**
	  * Unregister the first attachment. Should be called from destructor of A.
	  */	
	void UnregisterProjectile();
	/**
	  * Unregister the second attachment. Should be called from destructor of B.
	  */
	void UnregisterSection();

	HomingProjectile* GetProjectile();
	Section* GetSection();	
};

