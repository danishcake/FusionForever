#include "StdAfx.h"
#include "KeyboardAI.h"


KeyboardAI::KeyboardAI(void)
{
	dx_ = 0;
	dy_ = 0;
}

KeyboardAI::~KeyboardAI(void)
{
}

AIAction KeyboardAI::Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies, Core* _self)
{
	AIAction action;
	Uint8* keystates = SDL_GetKeyState(0);
	if(keystates[SDLK_UP])
		action.dy_++;
	if(keystates[SDLK_DOWN])
		action.dy_--;
	if(keystates[SDLK_LEFT])
		action.dx_--;
	if(keystates[SDLK_RIGHT])
		action.dx_++;
	
	int x = 0;
	int y = 0;
	Uint8 mouse_state = SDL_GetMouseState(&x, &y);
	if(SDL_BUTTON_LEFT && mouse_state)
		action.firing_ = true;
	Vector3f point_to_face = Vector3f(x-(SDL_GetVideoSurface()->clip_rect.w/2),y-(SDL_GetVideoSurface()->clip_rect.h/2),0); //TODO change to reflect resolution
	if(point_to_face.lengthSq()!=0)
	{
		point_to_face.normalize();
		Vector3f pointed_faced = Vector3f(-sinf(DEG2RAD(_self->GetAngle())), cos(DEG2RAD(_self->GetAngle())), 0);

	}

	return action;
}