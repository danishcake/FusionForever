#pragma once
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

/**
  * The Datastore class represents a repository of vertices and display lists that are lazily
  * instantiated by their respective owners. Follows singleton pattern.
  */
class Datastore
{
private:
	Datastore(void);
	std::vector<boost::shared_ptr<std::vector<Vector3f>>> vert_list_;
	int display_list_count_;
public:
	~Datastore(void);
   /**
     * Gets the Datastore instance.
     */
	static Datastore& Instance()
	{
		static Datastore instance;
		return instance;
	}
   /**
     * Adds a vector of vertices. Used to store outlines and fills.
     * @param _data The vertices to store
     * @return The index to access the vertices from the store.
     */
	int AddVerts(boost::shared_ptr<std::vector<Vector3f>> _data);
   /**
     * Gets a vector of vertices from the store.
     * @param _index The index where the data is stored.
     * @return The vector of vectices
     */
	boost::shared_ptr<std::vector<Vector3f>> GetVerts(int _index);
   /**
     * Starts recording a display list
     * @return The display list
     */
	int BeginDisplayList();
   /**
     * Stops recording a display list
     */
	void EndDisplayList();
};
