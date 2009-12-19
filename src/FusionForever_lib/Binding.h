#pragma once

#include <sdl.h>
#include <map>
#include "Logger.h"

#ifndef STR_ME
#define STR_ME( X ) ( # X )
#endif

namespace BindingType
{
	enum Enum
	{
		InvalidFirst, Unbound, KeyboardBinding, JoystickButtonBinding, JoystickAxisBinding, MouseButtonBinding, MouseAxisBinding, InvalidLast
	};

	static const std::string ToStr[] =
	{
		"InvalidBindType",
		STR_ME( Unbound ),
		STR_ME( KeyboardBinding ),
		STR_ME( JoystickButtonBinding ),
		STR_ME( JoystickAxisBinding ),
		STR_ME( MouseButtonBinding ),
		STR_ME( MouseAxisBinding ),
		"InvalidBindType"
	};

	static const Enum FromStr(std::string _binding_type)
	{
		for(int i = InvalidFirst+1; i < InvalidLast; i++)
		{
			if(ToStr[i] == _binding_type)
				return static_cast<Enum>(i);
		}
		return InvalidFirst;
	}
}

namespace MouseAxis
{
	enum Enum
	{
		InvalidFirst, MouseX, MouseY, InvalidLast
	};

	static const std::string ToStr[] =
	{
		"InvalidMouseAxis",
		STR_ME( MouseX ),
		STR_ME( MouseY ),
		"InvalidMouseAxis"
	};

	static const Enum FromStr(std::string _axis)
	{
		for(int i = InvalidFirst+1; i < InvalidLast; i++)
		{
			if(ToStr[i] == _axis)
				return static_cast<Enum>(i);
		}
		return InvalidFirst;
	};
}

namespace MouseButton
{
	enum Enum
	{
		InvalidFirst,
		LeftMouseButton,
		MiddleMouseButton,
		RightMouseButton,
		InvalidLast
	};

	static const int GetSDLCode(Enum _button)
	{
		switch(_button)
		{
		case LeftMouseButton:
			return SDL_BUTTON_LEFT;
			break;
		case MiddleMouseButton:
			return SDL_BUTTON_MIDDLE;
			break;
		case RightMouseButton:
			return SDL_BUTTON_RIGHT;
			break;
		}
		return 0;
	}

	static const std::string ToStr[] = 
	{
		"InvalidMouseButton",
		STR_ME( LeftMouseButton ),
		STR_ME( MiddleMouseButton ),
		STR_ME( RightMouseButton ),
		"InvalidMouseButton"
	};

	static const Enum FromStr(std::string _btn)
	{
		for(int i = InvalidFirst+1; i < InvalidLast; i++)
		{
			if(ToStr[i] == _btn)
				return static_cast<Enum>(i);
		}
		return InvalidFirst;
	};
}

static std::map<int, SDL_Joystick*> joysticks;

struct JoystickButton
{
	int button_index;
	int joystick_index;
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
		jb.joystick_index = _joystick_index;
		return jb;
	}
};

struct JoystickAxis
{
	int axis_index;
	int joystick_index;
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
		ja.joystick_index = _joystick_index;
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
		InvalidFirst, XMovement, YMovement, MoveLeft, MoveRight, MoveUp, MoveDown, Fire, Target, Boost, LookXAxis, LookYAxis, LockMovement, LockAngle, LookBackwards, InvalidLast
	};

	static const std::string ToStr[] = 
	{
		"InvalidSkill",
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
		STR_ME( LookBackwards ),
		"InvalidSkill"
	};

	static const Enum FromStr(std::string _skill)
	{
		for(int i = InvalidFirst+1; i < InvalidLast; i++)
		{
			if(ToStr[i] == _skill)
			{
				return static_cast<Enum>(i);
			}
		}
		return InvalidFirst;
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
