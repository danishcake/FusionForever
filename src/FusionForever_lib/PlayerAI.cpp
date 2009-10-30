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
		Unbound, KeyboardBinding, JoystickButtonBinding, JoystickAxisBinding, MouseButtonBinding, MouseAxisBinding
	};
}

namespace MouseAxis
{
	enum Enum
	{
		MouseX, MouseY
	};
}

namespace MouseButton
{
	enum Enum
	{
		LeftMouseButton = SDL_BUTTON_LEFT,
		MiddleMouseButton = SDL_BUTTON_RIGHT,
		RightMouseButton = SDL_BUTTON_MIDDLE
	};
}

static std::map<int, SDL_Joystick*> joysticks;

struct JoystickButton
{
	int button_index;
	SDL_Joystick* joystick;
	static JoystickButton Create(int _joystick_index, int _button_index)
	{
		JoystickButton jb;
		if(SDL_NumJoysticks() <= _joystick_index)
		{
			Logger::DiagnosticOut() << "Joystick index " << _joystick_index << " greater than number of system joysticks, button binding will be ignored\n";
			jb.joystick = NULL;
		} else if(!SDL_JoystickOpened(_joystick_index))
		{
			jb.joystick = SDL_JoystickOpen(_joystick_index);
			joysticks[_joystick_index] = jb.joystick ;
		} else
		{
			jb.joystick = joysticks[_joystick_index];
		}
		jb.button_index = _button_index;
		return jb;
	}
};

struct JoystickAxis
{
	int axis_index;
	SDL_Joystick* joystick;
	static JoystickAxis Create(int _joystick_index, int _axis_index)
	{
		JoystickAxis ja;
		
		if(SDL_NumJoysticks() <= _joystick_index)
		{
			Logger::DiagnosticOut() << "Joystick index " << _joystick_index << " greater than number of system joysticks, axis binding will be ignored\n";
			ja.joystick = NULL;
		}
		else if(!SDL_JoystickOpened(_joystick_index))
		{
			ja.joystick = SDL_JoystickOpen(_joystick_index);
			joysticks[_joystick_index] = ja.joystick;
		} else
		{
			ja.joystick = joysticks[_joystick_index];
		}
		ja.axis_index = _axis_index;
		return ja;
	}
};

union Binding
{
	SDLKey key;
	JoystickButton joystick_button;
	MouseAxis::Enum mouse_axis;
	JoystickAxis joystick_axis;
	MouseButton::Enum mouse_button;
	Binding(){}
	Binding(SDLKey _key){key = _key;}
	Binding(JoystickButton _btn){joystick_button = _btn;}
	Binding(JoystickAxis _axis){joystick_axis = _axis;}
	Binding(MouseAxis::Enum _axis){mouse_axis = _axis;}
	Binding(MouseButton::Enum _btn){mouse_button = _btn;}
};

namespace Action
{
	static enum Enum
	{
		XMovement, YMovement, MoveLeft, MoveRight, MoveUp, MoveDown, Fire, Target, Boost, LookXAxis, LookYAxis, LockMovement, LockAngle, LookBackwards
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
		STR_ME( LookYAxis ),
		STR_ME( LockMovement ),
		STR_ME( LockAngle ),
		STR_ME( LookBackwards )
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

std::map<int, std::vector<InputConfig> > bindings;

void PlayerAI::LoadBindings()
{
	TiXmlDocument doc("Controls.xml");
	if(doc.LoadFile())
	{
		
	} else
	{
		std::vector<InputConfig> p1;
		std::vector<InputConfig> p2;
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_a), Action::MoveLeft));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_d), Action::MoveRight));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_w), Action::MoveUp));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_s), Action::MoveDown));
		p1.push_back(InputConfig(BindingType::MouseAxisBinding, Binding(MouseAxis::MouseX), Action::LookXAxis));
		p1.push_back(InputConfig(BindingType::MouseAxisBinding, Binding(MouseAxis::MouseY), Action::LookYAxis));
		p1.push_back(InputConfig(BindingType::KeyboardBinding, Binding(SDLK_LCTRL), Action::Boost));
		p1.push_back(InputConfig(BindingType::MouseButtonBinding, Binding(MouseButton::LeftMouseButton), Action::Fire));
		p1.push_back(InputConfig(BindingType::MouseButtonBinding, Binding(MouseButton::RightMouseButton), Action::Boost));
		p1.push_back(InputConfig(BindingType::MouseButtonBinding, Binding(MouseButton::MiddleMouseButton), Action::Target));
		
		p2.push_back(InputConfig(BindingType::JoystickAxisBinding, Binding(JoystickAxis::Create(0,0)), Action::XMovement));
		p2.push_back(InputConfig(BindingType::JoystickAxisBinding, Binding(JoystickAxis::Create(0,1)), Action::YMovement));
		p2.push_back(InputConfig(BindingType::JoystickAxisBinding, Binding(JoystickAxis::Create(0,0)), Action::LookXAxis));
		p2.push_back(InputConfig(BindingType::JoystickAxisBinding, Binding(JoystickAxis::Create(0,1)), Action::LookYAxis));
		p2.push_back(InputConfig(BindingType::JoystickButtonBinding, Binding(JoystickButton::Create(0,0)), Action::Fire));
		p2.push_back(InputConfig(BindingType::JoystickButtonBinding, Binding(JoystickButton::Create(0,1)), Action::Boost));
		p2.push_back(InputConfig(BindingType::JoystickButtonBinding, Binding(JoystickButton::Create(0,2)), Action::LockAngle));
		p2.push_back(InputConfig(BindingType::JoystickButtonBinding, Binding(JoystickButton::Create(0,3)), Action::LockMovement));
		p2.push_back(InputConfig(BindingType::JoystickButtonBinding, Binding(JoystickButton::Create(0,5)), Action::LookBackwards));

		bindings[0] = p1;
		bindings[1] = p2;
	}
}

PlayerAI::PlayerAI(int _player_id)
{
	player_id_ = _player_id;
	lock_angle_ = false;
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
	bool lock_movement = false;
	bool lock_angle = false;
	bool look_backwards = false;

	if(SDL_NumJoysticks())
		SDL_JoystickUpdate();
	point_to_face = _self->GetGlobalPosition();
	
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
				point_to_face.x = Camera::Instance().ScreenToWorld(Vector3f(axis_value, 0, 0)).x;
				break;
			case Action::LookYAxis:
				point_to_face.y = Camera::Instance().ScreenToWorld(Vector3f(0, axis_value, 0)).y;
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
			{
				float axis_value = 0;
				if(it->binding.joystick_axis.joystick)
				{
					Sint16 js_axis_val = SDL_JoystickGetAxis(it->binding.joystick_axis.joystick, it->binding.joystick_axis.axis_index);
					if(abs(js_axis_val) < MAXSHORT * 0.2f)
						js_axis_val = 0;
					axis_value = (float)js_axis_val / (float)MAXSHORT;
				}

				switch(it->action)
				{
				case Action::LookXAxis:
					point_to_face.x += axis_value;
					break;
				case Action::LookYAxis:
					point_to_face.y -= axis_value;
					break;
				case Action::XMovement:
					action.dx_ += axis_value;
					break;
				case Action::YMovement:
					action.dy_ -= axis_value;
					break;
				}
				break;
			}
		case BindingType::KeyboardBinding:
		case BindingType::MouseButtonBinding:
		case BindingType::JoystickButtonBinding:

			if((it->type == BindingType::KeyboardBinding && keystates[it->binding.key]) ||
			   (it->type == BindingType::MouseButtonBinding && (mb & SDL_BUTTON(it->binding.mouse_button))) ||
			   (it->type == BindingType::JoystickButtonBinding && it->binding.joystick_button.joystick && SDL_JoystickGetButton(it->binding.joystick_button.joystick, it->binding.joystick_button.button_index)))
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
				case Action::LockMovement:
					lock_movement = true;
					break;
				case Action::LockAngle:
					lock_angle = true;
					break;
				case Action::LookBackwards:
					look_backwards = true;
					break;
				}
			}
			break;
		}
	}
	if(look_backwards)
	{
		point_to_face *= -1;
	}
	if(lock_angle_)
	{
		point_to_face = lock_vector_ + _self->GetAngle();
	}
	//Vector3f point_to_face_world = Camera::Instance().ScreenToWorld(point_to_face); //TODO should changes this method to map screen to world, without inverted y
	Vector3f point_to_face_relative = point_to_face - _self->GetGlobalPosition();
	if(point_to_face_relative.lengthSq()!=0)
	{
		TurnData turn_data = GetTurnDirection(_self->GetAngle(), point_to_face_relative);
		float dotprod = turn_data.turn_factor;
		action.dtheta_ = ClampTurnDirection(dotprod, 0.4f);
		//point_to_face_relative.normalize();

		Vector3f peer_factors = point_to_face_relative / Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()), static_cast<float>(Camera::Instance().GetWindowHeight()), 0);
		if(peer_factors.lengthSq() > 1)
			peer_factors.normalize();
		peer_factors *= 0.5f * 0.8f;
		Vector3f camera_centre = _self->GetGlobalPosition() + peer_factors * Vector3f(Camera::Instance().GetWidth() / 2.0f, Camera::Instance().GetHeight() / 2.0f, 0);
		camera_centre.z = 0;
		Camera::Instance().SetCentreTarget(camera_centre.x, camera_centre.y, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
	}

	ltv_mouse_position_.x = mx;
	ltv_mouse_position_.y = my;

	if(lock_movement)
	{
		action.dx_ = 0;
		action.dy_ = 0;
	}

	if(lock_angle && !lock_angle_)
	{
		lock_vector_ = Vector3f(sinf(_self->GetAngle() * M_PI / 180.0f),
							    cosf(_self->GetAngle() * M_PI / 180.0f), 0);
	}
	lock_angle_ = lock_angle;

	return action;
}
