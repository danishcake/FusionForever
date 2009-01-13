#include "StdAfx.h"
#include "KeyboardAI.h"
#include "TurningRoutines.h"
#include <sstream>
#include "Core.h"
#include "Radar.h"

#define ZOOM_TIME 0.3f

KeyboardAI::KeyboardAI(void)
{
	zoom_time_= 0;
}

KeyboardAI::~KeyboardAI(void)
{
}

AIAction KeyboardAI::Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self)
{
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
		Logger::Instance() << world_click.x << "," << world_click.y << "\n";
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

		Vector3f point_faced = Vector3f(sinf(_self->GetAngle() * M_PI / 180.0f), cosf(_self->GetAngle() * M_PI / 180.0f), 0);


		//float zoom_scale = powf(zoom_time_ / ZOOM_TIME, 1.7f);
		//Vector3f camera_centre = (1.0f - zoom_scale) * _self->GetPosition() + zoom_scale * point_to_face_world;
		float peer_factor = 2 * point_to_face.length() / Camera::Instance().GetSmallestDimension();
		Vector3f peer_point = Camera::Instance().ScreenDeltaToWorldDelta(point_to_face);
		peer_point.y *= -1;
		Vector3f camera_centre = _self->GetGlobalPosition() + peer_point * peer_factor;
		Camera::Instance().SetCentre(camera_centre.x, camera_centre.y, CameraLevel::Human);
		//Camera::Instance().SetCentre(_self->GetPosition().x + point_to_face_world.x * Camera::Instance().GetWidth() * 0.4f * zoom_scale, _self->GetPosition().y + point_to_face_world.y* Camera::Instance().GetHeight() * 0.4f * zoom_scale, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
		//Logger::Log(boost::lexical_cast<std::string, float>(_self->GetEnergy().GetValue()) + "/" + boost::lexical_cast<std::string, float>(_self->GetEnergy().GetMaxValue()));
	}
	
	Radar::SetPlayerPosition(_self->GetGlobalPosition());
	return action;
}
