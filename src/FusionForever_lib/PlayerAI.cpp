#include "StdAfx.h"
#include "PlayerAI.h"
#include "TurningRoutines.h"
#include "Core.h"
#include "sdl.h"

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

static union Binding
{
	SDLKey key;
	JoystickButton joystick_button;
	MouseAxis::Enum mouse_axis;
	JoystickAxis joystick_axis;
};

static struct InputConfig
{
	BindingType::Enum type;
	Binding binding;
};

std::map<int, std::vector<InputConfig> > bindings;


PlayerAI::PlayerAI(int _player_id)
{
	player_id_ = _player_id;

}

PlayerAI::~PlayerAI(void)
{
}

AIAction PlayerAI::Tick(float /*_timespan*/, std::vector<Core*>& /*_allies*/, std::vector<Core*>& _enemies, Core* _self)
{
	return AIAction();		
}
