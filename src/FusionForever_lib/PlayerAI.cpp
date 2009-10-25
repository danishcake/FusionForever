#include "StdAfx.h"
#include "PlayerAI.h"
#include "TurningRoutines.h"
#include "Core.h"
#include "sdl.h"

#ifndef STR_ME
#define STR_ME( X ) ( # X )
#endif

namespace BindingType
{
	enum Enum
	{
		Unbound, KeyboardBinding, JoystickButtonBinding, MouseAxisBinding, JoystickAxisBinding
	};
}

namespace MouseAxis
{
	enum Enum
	{
		MouseX, MouseY
	};
}

struct JoystickButton
{
	int joystick_index;
	int button_index;
};

struct JoystickAxis
{
	int joystick_index;
	int axis_index;
};

union Binding
{
	SDLKey key;
	JoystickButton joystick_button;
	MouseAxis::Enum mouse_axis;
	JoystickAxis joystick_axis;
	Binding(){}
	Binding(SDLKey _key){key = _key;}
	Binding(JoystickButton _btn){joystick_button = _btn;}
	Binding(JoystickAxis _axis){joystick_axis = _axis;}
	Binding(MouseAxis::Enum _axis){mouse_axis = _axis;}
};

namespace Action
{
	static enum Enum
	{
		XMovement, YMovement, MoveLeft, MoveRight, MoveUp, MoveDown, Fire, Target, Boost, LookXAxis, LookYAxis
	};

	static const std::string ToStr[] = 
	{
		STR_ME( XMovement ),
		STR_ME( YMovement ),
		STR_ME( MoveLeft ),
		STR_ME( MoveRight ),
		STR_ME( MoveUp ),
		STR_ME( MoveDown ),
		STR_ME( Fire ),
		STR_ME( Target ),
		STR_ME( Boost ),
		STR_ME( LookXAxis ),
		STR_ME( LookYAxis )
	};
}

struct InputConfig
{
	BindingType::Enum type;
	Binding binding;
	Action::Enum action;
	InputConfig(BindingType::Enum _type, Binding _binding, Action::Enum _action)
	{
		type = _type;
		binding = _binding;
		action = _action;
	}
};

std::vector<std::vector<InputConfig> > bindings;

void PlayerAI::LoadBindings()
{
	TiXmlDocument doc("Controls.xml");
	if(doc.LoadFile())
	{
		
	} else
	{
		std::vector<InputConfig> p1;
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_LEFT), Action::MoveLeft));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_RIGHT), Action::MoveRight));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_UP), Action::MoveUp));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_DOWN), Action::MoveDown));
		p1.push_back(InputConfig(BindingType::MouseAxisBinding, Binding(MouseAxis::MouseX), Action::LookXAxis));
		p1.push_back(InputConfig(BindingType::MouseAxisBinding, Binding(MouseAxis::MouseY), Action::LookYAxis));

		bindings.push_back(p1);
	}
}

PlayerAI::PlayerAI(int _player_id)
{
	player_id_ = _player_id;	
}

PlayerAI::~PlayerAI(void)
{
}

AIAction PlayerAI::Tick(float _timespan, std::vector<Core*>& /*_allies*/, std::vector<Core*>& _enemies, Core* _self)
{
	AIAction action;
	Uint8* keystates = SDL_GetKeyState(0);
	int mx, my;
	int mb = SDL_GetMouseState(&mx, &my);

	Vector3f point_to_face;
	
	std::vector<InputConfig>& binds = bindings[player_id_];

	for(std::vector<InputConfig>::iterator it = binds.begin(); it != binds.end(); ++it)
	{
		switch(it->type)
		{
		case BindingType::MouseAxisBinding:
			{
			float axis_value = 0;
			float ltv_axis_value = 0;
			switch(it->binding.mouse_axis)
			{
			case MouseAxis::MouseX:
				axis_value = mx;
				ltv_axis_value = ltv_mouse_position_.x;
				break;
			case MouseAxis::MouseY:
				axis_value =  my;
				ltv_axis_value = ltv_mouse_position_.y;
				break;
			}
			switch(it->action)
			{
			case Action::LookXAxis:
				point_to_face.x = axis_value - Camera::Instance().GetWindowWidth() / 2.0f;
				break;
			case Action::LookYAxis:
				point_to_face.y = (Camera::Instance().GetWindowHeight() / 2.0f) - axis_value;
				break;
			case Action::XMovement:
				movement_integrator_.x *= (1.0f - 0.2f * _timespan);
				movement_integrator_.x += axis_value - ltv_axis_value;
				action.dx_ += movement_integrator_.x / (fabs(movement_integrator_.x) > 1.0f ? fabs(movement_integrator_.x) : 1.0f);
				break;
			case Action::YMovement:
				movement_integrator_.y *= (1.0f - 0.2f * _timespan);
				movement_integrator_.y += axis_value - ltv_axis_value;
				action.dx_ += movement_integrator_.y / (fabs(movement_integrator_.y) > 1.0f ? fabs(movement_integrator_.y) : 1.0f);
				break;
			}
			}
			break;
		case BindingType::JoystickAxisBinding:
			break;
		case BindingType::JoystickButtonBinding:
			break;
		case BindingType::KeyboardBinding:
			if(keystates[it->binding.key])
			{
				switch(it->action)
				{
				case Action::Fire:
					action.firing_ = true;
					break;
				case Action::Boost:
					action.thrust_ = true;
					break;
				case Action::Target:
					break;
				case Action::MoveLeft:
					action.dx_--;
					break;
				case Action::MoveRight:
					action.dx_++;
					break;
				case Action::MoveUp:
					action.dy_++;
					break;
				case Action::MoveDown:
					action.dy_--;
					break;
				}
			}
			break;
		}
	}

	Vector3f point_to_face_world = Camera::Instance().ScreenToWorld(point_to_face); //TODO should changes this method to map screen to world, without inverted y
	if(point_to_face.lengthSq()!=0)
	{
		TurnData turn_data = GetTurnDirection(_self->GetAngle(), point_to_face);
		float dotprod = turn_data.turn_factor;
		action.dtheta_ = ClampTurnDirection(dotprod, 0.4f);

		Vector3f peer_factors = (point_to_face * 2) / Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()), static_cast<float>(Camera::Instance().GetWindowHeight()), 0);
		peer_factors *= 0.8f;
		Vector3f camera_centre = _self->GetGlobalPosition() + peer_factors * Vector3f(Camera::Instance().GetWidth() / 2.0f, Camera::Instance().GetHeight() / 2.0f, 0);
		Camera::Instance().SetCentreTarget(camera_centre.x, camera_centre.y, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
	}

	ltv_mouse_position_.x = mx;
	ltv_mouse_position_.y = my;
	return action;
}
