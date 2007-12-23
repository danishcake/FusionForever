#pragma once
#include <vector>
#include <string>
#include "vmath.h"
#include <boost/shared_ptr.hpp>

class Datastore
{
private:
	Datastore(void);
	std::vector<boost::shared_ptr<std::vector<Vector3f>>> vert_list_;
	int display_list_count_;
public:
	~Datastore(void);
	static Datastore& Instance() 
	{	
		static Datastore instance;
		return instance;
	}
	int AddVerts(boost::shared_ptr<std::vector<Vector3f>> _data);
	boost::shared_ptr<std::vector<Vector3f>> GetVerts(int _index);
	int BeginDisplayList();
	void EndDisplayList();
};
