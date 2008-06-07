#include "StdAfx.h"
#include "KeyboardAI.h"
#include "TurningRoutines.h"
#include <sstream>

#define ZOOM_TIME 0.3f

KeyboardAI::KeyboardAI(void)
{
	zoom_time_= 0;
}

KeyboardAI::~KeyboardAI(void)
{
}

AIAction KeyboardAI::Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	AIAction action;
	Uint8* keystates = SDL_GetKeyState(0);
	int x = 0;
	int y = 0;
	Uint8 mouse_state = SDL_GetMouseState(&x, &y);

	if(SDL_BUTTON_MMASK & mouse_state)
	{
		Vector3f dv = Vector3f(0,0,0);
		if(keystates[SDLK_UP])
			dv.y++;
		if(keystates[SDLK_DOWN])
			dv.y--;
		if(keystates[SDLK_LEFT])
			dv.x--;
		if(keystates[SDLK_RIGHT])
			dv.x++;
		if(dv.lengthSq()!=0)
		{
			dv.normalize();
			dv.rotate(0, 0, _self->GetAngle());
			action.dx_ = dv.x;
			action.dy_ = dv.y;
		}
	}
	else
	{
		if(keystates[SDLK_UP])
			action.dy_++;
		if(keystates[SDLK_DOWN])
			action.dy_--;
		if(keystates[SDLK_LEFT])
			action.dx_--;
		if(keystates[SDLK_RIGHT])
			action.dx_++;
	}
	if(SDL_BUTTON_LMASK & mouse_state)
		action.firing_ = true;

	Vector3f point_to_face = Vector3f((x - Camera::Instance().GetWindowWidth() / 2.0f),
		                              (Camera::Instance().GetWindowHeight() / 2.0f) - y, 0); // Mouse y coordinates are screen coordinates and so upside down
	if(point_to_face.lengthSq()!=0)
	{
		TurnData turn_data = GetTurnDirection(_self->GetAngle(), point_to_face);
		float dotprod = turn_data.turn_factor;
		action.dtheta_ = ClampTurnDirection(dotprod, 0.4f);

		Vector3f point_faced = Vector3f(sinf(_self->GetAngle() * M_PI / 180.0f), cosf(_self->GetAngle() * M_PI / 180.0f), 0);

		//Position camera
		if(SDL_BUTTON_RMASK & mouse_state)
		{
			zoom_time_ += _timespan;
			zoom_time_ = zoom_time_ > ZOOM_TIME ? ZOOM_TIME : zoom_time_;
		}
		else
		{
			zoom_time_ -= _timespan;
			zoom_time_ = zoom_time_ < 0.0f ? 0.0f : zoom_time_;
		}
		float zoom_scale = powf(zoom_time_ / ZOOM_TIME, 1.7f);
		Camera::Instance().SetCentre(_self->GetPosition().x + point_faced.x * Camera::Instance().GetWidth() * 0.4f * zoom_scale, _self->GetPosition().y + point_faced.y* Camera::Instance().GetHeight() * 0.4f * zoom_scale);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y);

	}

	return action;
}
