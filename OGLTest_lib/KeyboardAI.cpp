#include "StdAfx.h"
#include "KeyboardAI.h"
#include "TurningRoutines.h"
#include <sstream>
#include "Core.h"
#include "Radar.h"

int KeyboardAI::instance_count_ = 0;
std::vector<int> KeyboardAI::player_ids_;

KeyboardAI::KeyboardAI(void)
{
	player_id_ = -1;
}

KeyboardAI::~KeyboardAI(void)
{
	if(player_id_ != -1)
	{
		player_ids_.erase(std::remove(player_ids_.begin(), player_ids_.end(), player_id_), player_ids_.end());
	}
}

AIAction KeyboardAI::Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self)
{
	if(player_id_ == -1)
	{
		player_id_ = _self->GetSectionID();
		if(player_id_ != -1)
		{
			player_ids_.push_back(player_id_);
		}
		else
			Logger::ErrorOut() << "Player ID of -1 encountered\n";
	}
	AIAction action;
	Uint8* keystates = SDL_GetKeyState(0);
	int x = 0;
	int y = 0;
	Uint8 mouse_state = SDL_GetMouseState(&x, &y);

	if(keystates[SDLK_w])
		action.dy_++;
	if(keystates[SDLK_s])
		action.dy_--;
	if(keystates[SDLK_a])
		action.dx_--;
	if(keystates[SDLK_d])
		action.dx_++;

	if(keystates[SDLK_SPACE])
	{
		//Map mouse coordinates onto world coordinates and then find if there is anything at that spot
		Vector3f world_click = Camera::Instance().ScreenToWorld(Vector3f(static_cast<float>(x), static_cast<float>(y), 0.0f));
		Section_ptr clicked_section = NULL;
		BOOST_FOREACH(Core_ptr core, _enemies)
		{
			if(core->CheckCollisions(world_click, clicked_section))
			{
				break;
			}
		}
		if(clicked_section)
		{
			clicked_section = clicked_section->GetRoot();
			action.target_ = static_cast<Core_ptr>(clicked_section);
		}
	}


	if(SDL_BUTTON_LMASK & mouse_state)
		action.firing_ = true;

	if(keystates[SDLK_PLUS] || keystates[SDLK_EQUALS])
		Camera::Instance().ZoomIn();
	if(keystates[SDLK_MINUS] || keystates[SDLK_UNDERSCORE])
		Camera::Instance().ZoomOut();

	if(SDL_BUTTON_MMASK & mouse_state)
		action.thrust_ = true;

	Vector3f point_to_face = Vector3f((x - Camera::Instance().GetWindowWidth() / 2.0f),
		                              (Camera::Instance().GetWindowHeight() / 2.0f) - y, 0); // Mouse y coordinates are screen coordinates and so upside down
	Vector3f point_to_face_world = Camera::Instance().ScreenToWorld(point_to_face); //TODO should changes this method to map screen to world, without inverted y
	if(point_to_face.lengthSq()!=0)
	{
		TurnData turn_data = GetTurnDirection(_self->GetAngle(), point_to_face);
		float dotprod = turn_data.turn_factor;
		action.dtheta_ = ClampTurnDirection(dotprod, 0.4f);

		Vector3f peer_factors = (point_to_face * 2) / Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()), static_cast<float>(Camera::Instance().GetWindowHeight()), 0);
		peer_factors *= 0.8f;
		Vector3f camera_centre = _self->GetGlobalPosition() + peer_factors * Vector3f(Camera::Instance().GetWidth() / 2.0f, Camera::Instance().GetHeight() / 2.0f, 0);
		Camera::Instance().SetCentre(camera_centre.x, camera_centre.y, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
	}
	
	Radar::SetPlayerPosition(_self->GetGlobalPosition());
	return action;
}
