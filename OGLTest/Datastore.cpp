#include "StdAfx.h"
#include "Datastore.h"

Datastore::Datastore(void)
{
	display_list_count_ = 1;
}

Datastore::~Datastore(void)
{

}

int Datastore::AddVerts(boost::shared_ptr<std::vector<Vector3f>> _data)
{
	vert_list_.push_back(_data);
	return (int)vert_list_.size()-1;
}

boost::shared_ptr<std::vector<Vector3f>> Datastore::GetVerts(int _index)
{
	return vert_list_[_index];
}

int Datastore::BeginDisplayList()
{
	glNewList(display_list_count_, GL_COMPILE);
	return display_list_count_;
}

void Datastore::EndDisplayList()
{
	display_list_count_++;
	glEndList();
}