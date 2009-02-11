#pragma once
#include <vector>
#include "vmath.h"

class Core;

/*
	This class renders a radar depicting relative position and size of enemies
*/
class Radar
{
private:
	std::vector<Vector3f> small_points_friends_;
	std::vector<Vector3f> large_points_friends_;
	std::vector<Vector3f> huge_points_friends_;
	std::vector<Vector3f> small_points_enemies_;
	std::vector<Vector3f> large_points_enemies_;
	std::vector<Vector3f> huge_points_enemies_;

	std::vector<Vector3f> outline_;
	float average_tonnage_enemies_;
	float average_tonnage_friends_;
	static Vector3f player_position_;
	Vector3f radar_size_;
	Vector3f radar_position_;
	float radar_range_;
	static float time_since_player_update_;

public:
	Radar(void);
	~Radar(void);

	void Update(std::vector<Core*> _enemies, std::vector<Core*> _friends, float _timespan);
	void Draw();
	static void SetPlayerPosition(Vector3d _player_position);


	static const float RadarFadeTime;
};
