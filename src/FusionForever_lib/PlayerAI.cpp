#include "StdAfx.h"
#include "PlayerAI.h"
#include "TurningRoutines.h"
#include "Core.h"
#include "sdl.h"
#include "Binding.h"


std::vector<int> PlayerAI::core_ids_;


std::map<int, std::vector<InputConfig> > bindings;

void PlayerAI::SaveBindings()
{
	TiXmlDocument doc("Controls.xml");
	TiXmlElement* root = new TiXmlElement("Controls");
	doc.LinkEndChild(root);
	for(std::map<int, std::vector<InputConfig> >::iterator it = bindings.begin(); it != bindings.end(); ++it)
	{
		TiXmlElement* player = new TiXmlElement("PlayerBindings");
		player->SetAttribute("id", it->first);
		for(std::vector<InputConfig>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			TiXmlElement* ic = new TiXmlElement("InputConfig");
			ic->SetAttribute("Action", Action::ToStr[it2->action]);
			ic->SetAttribute("BindingType", BindingType::ToStr[it2->type]);
			switch(it2->type)
			{
			case BindingType::KeyboardBinding:
				{
					TiXmlElement* kb_el = new TiXmlElement("Binding");
					kb_el->SetAttribute("Key", it2->binding.key);
					ic->LinkEndChild(kb_el);
				}
				break;
			case BindingType::JoystickButtonBinding:
				{
					TiXmlElement* jsb_el = new TiXmlElement("Binding");
					jsb_el->SetAttribute("JoystickIndex", it2->binding.joystick_button.joystick_index);
					jsb_el->SetAttribute("JoystickButton", it2->binding.joystick_button.button_index);
					ic->LinkEndChild(jsb_el);
				}
				break;
			case BindingType::MouseButtonBinding:
				{
					TiXmlElement* mb_el = new TiXmlElement("Binding");
					mb_el->SetAttribute("MouseButton", MouseButton::ToStr[it2->binding.mouse_button]);
					ic->LinkEndChild(mb_el);
				}
				break;
			case BindingType::JoystickAxisBinding:
				{
					TiXmlElement* jsa_el = new TiXmlElement("Binding");
					jsa_el->SetAttribute("JoystickAxis", it2->binding.joystick_axis.axis_index);
					jsa_el->SetAttribute("JoystickIndex", it2->binding.joystick_axis.joystick_index);
					ic->LinkEndChild(jsa_el);
				}
				break;
			case BindingType::MouseAxisBinding:
				{
					TiXmlElement* ma_el = new TiXmlElement("Binding");
					ma_el->SetAttribute("MouseAxis", MouseAxis::ToStr[it2->binding.mouse_axis]);
					ic->LinkEndChild(ma_el);
				}
				break;
			}
			player->LinkEndChild(ic);
		}
		
		root->LinkEndChild(player);
	}

	doc.SaveFile();
}

void PlayerAI::LoadBindings()
{
	TiXmlDocument doc("Controls.xml");
	bool xml_error = false;
	if(doc.LoadFile())
	{
		TiXmlElement* root = doc.FirstChildElement("Controls");
		if(root)
		{
			TiXmlElement* player = root->FirstChildElement("PlayerBindings");
			while(player)
			{
				int player_id = 0;
				if(player->QueryIntAttribute("id", &player_id) == TIXML_SUCCESS)
				{
					TiXmlElement* ic = player->FirstChildElement("InputConfig");
					while(ic)
					{
						std::string action_string;
						if(ic->QueryValueAttribute("Action", &action_string) != TIXML_SUCCESS)
						{
							Logger::ErrorOut() << "Error reading action attribute, continuing\n";
							continue;
						}
						std::string binding_type_string;
						if(ic->QueryValueAttribute("BindingType", &binding_type_string) != TIXML_SUCCESS)
						{
							Logger::ErrorOut() << "Error reading binding attribute, continuing\n";
							continue;
						}
						
						Action::Enum action = Action::FromStr(action_string);
						BindingType::Enum binding_type = BindingType::FromStr(binding_type_string);
						TiXmlElement* binding = ic->FirstChildElement("Binding");
						if(binding)
						{
							switch(binding_type)
							{
								case BindingType::KeyboardBinding:
									{
										int key = 0;
										if(binding->QueryValueAttribute("Key", &key) == TIXML_SUCCESS)
										{
											bindings[player_id].push_back(InputConfig(binding_type, Binding((SDLKey)key), action));
										} else
										{
											Logger::ErrorOut() << "Missing Key attribute, continuing\n";
											continue;
										}
									}
									break;
								case BindingType::JoystickButtonBinding:
									{
										int joystick_index = 0;
										int joystick_button_index = 0;
										if(binding->QueryValueAttribute("JoystickIndex", &joystick_index) == TIXML_SUCCESS && 
										   binding->QueryValueAttribute("JoystickButton", &joystick_button_index) == TIXML_SUCCESS)
										{
											bindings[player_id].push_back(InputConfig(binding_type, Binding(JoystickButton::Create(joystick_index, joystick_button_index)), action));
										} else
										{
											Logger::ErrorOut() << "Missing JoystickIndex or JoystickButton atttribute, continuing\n";
										}
									}
									break;
								case BindingType::JoystickAxisBinding:
									{
										int joystick_index = 0;
										int joystick_axis_index = 0;
										if(binding->QueryValueAttribute("JoystickIndex", &joystick_index) == TIXML_SUCCESS && 
										   binding->QueryValueAttribute("JoystickAxis", &joystick_axis_index) == TIXML_SUCCESS)
										{
											bindings[player_id].push_back(InputConfig(binding_type, Binding(JoystickAxis::Create(joystick_index, joystick_axis_index)), action));
										} else
										{
											Logger::ErrorOut() << "Missing JoystickIndex or JoystickAxis atttribute, continuing\n";
										}
									}
									break;
								case BindingType::MouseAxisBinding:
									{
										std::string axis;
										if(binding->QueryValueAttribute("MouseAxis", &axis) == TIXML_SUCCESS)
										{
											MouseAxis::Enum mouse_axis = MouseAxis::FromStr(axis);
											if(mouse_axis != MouseAxis::InvalidFirst && mouse_axis != MouseAxis::InvalidLast)
											{
												bindings[player_id].push_back(InputConfig(binding_type, Binding(mouse_axis), action));
											} else
											{
												Logger::ErrorOut() << "MouseAxis attribute incorrect: " << axis << "\n";
											}
										} else
										{
											Logger::ErrorOut() << "Missing MouseAxis attribute\n";
										}
									}
									break;
								case BindingType::MouseButtonBinding:
									{
										std::string button;
										if(binding->QueryValueAttribute("MouseButton", &button) == TIXML_SUCCESS)
										{
											MouseButton::Enum mouse_button = MouseButton::FromStr(button);
											if(mouse_button != MouseButton::InvalidFirst && mouse_button != MouseButton::InvalidLast)
											{
												bindings[player_id].push_back(InputConfig(binding_type, Binding(mouse_button), action));
											} else
											{
												Logger::ErrorOut() << "Missing MouseAxis attribute\n";
											}

										} else
										{
											Logger::ErrorOut() << "Missing MouseButton attribute\n";
										}
									}
									break;

							}
						} else 
						{
							xml_error = true;
							Logger::ErrorOut() << "Missing Binding element\n";
						}

						
						ic = ic->NextSiblingElement("InputConfig");
					}
					
				} else xml_error = true;
				player = player->NextSiblingElement("PlayerBindings");
			}
		} else xml_error = true;
	} else xml_error = true;
	if(xml_error)
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
	core_id_ = -1;
}

PlayerAI::~PlayerAI(void)
{
	if(core_id_ != -1)
	{
		core_ids_.erase(std::remove(core_ids_.begin(), core_ids_.end(), core_id_), core_ids_.end());
	}
}

AIAction PlayerAI::Tick(float _timespan, std::vector<Core*>& /*_allies*/, std::vector<Core*>& _enemies, Core* _self)
{
	if(core_id_ == -1)
	{
		core_id_ = _self->GetSectionID();
		if(core_id_ != -1)
		{
			core_ids_.push_back(core_id_);
		}
		else
			Logger::ErrorOut() << "Player ID of -1 encountered\n";
	}

	AIAction action;
	Uint8* keystates = SDL_GetKeyState(0);
	int mx, my;
	int mb = SDL_GetMouseState(&mx, &my);

	Vector3f point_to_face;
	Vector3f peer_factor;
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
				axis_value = static_cast<float>(mx);
				ltv_axis_value = ltv_mouse_position_.x;
				break;
			case MouseAxis::MouseY:
				axis_value = static_cast<float>(my);
				ltv_axis_value = ltv_mouse_position_.y;
				break;
			}
			switch(it->action)
			{
			case Action::LookXAxis:
				point_to_face.x = Camera::Instance().ScreenToWorld(Vector3f(axis_value, 0, 0)).x;
				peer_factor.x = (axis_value - (Camera::Instance().GetWindowWidth() / 2.0f)) * 2.0f / Camera::Instance().GetWindowWidth();
				break;
			case Action::LookYAxis:
				point_to_face.y = Camera::Instance().ScreenToWorld(Vector3f(0, axis_value, 0)).y;
				peer_factor.y = -(axis_value - (Camera::Instance().GetWindowHeight() / 2.0f)) * 2.0f / Camera::Instance().GetWindowHeight();
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
					peer_factor.x = axis_value;
					break;
				case Action::LookYAxis:
					point_to_face.y -= axis_value;
					peer_factor.y = -axis_value;
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
			   (it->type == BindingType::MouseButtonBinding && (mb & SDL_BUTTON(GetSDLCode(it->binding.mouse_button)))) ||
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
		point_to_face = lock_vector_ + _self->GetGlobalPosition();//+ _self->GetAngle();
	}

	Vector3f point_to_face_relative = point_to_face - _self->GetGlobalPosition();
	if(point_to_face_relative.lengthSq()!=0)
	{
		TurnData turn_data = GetTurnDirection(_self->GetAngle(), point_to_face_relative);
		float dotprod = turn_data.turn_factor;
		action.dtheta_ = ClampTurnDirection(dotprod, 0.4f);

		if(peer_factor.lengthSq() > 1)
			peer_factor.normalize();
		Vector3f camera_centre = _self->GetGlobalPosition();
		Camera::Instance().SetCentreTarget(camera_centre.x, camera_centre.y, peer_factor.x, peer_factor.y, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
	} else
	{
		Camera::Instance().SetCentreTarget(_self->GetPosition().x, _self->GetPosition().y, 0, 0, CameraLevel::Human);
		Camera::Instance().SetFocus(_self->GetPosition().x, _self->GetPosition().y, CameraLevel::Human);
	}

	ltv_mouse_position_.x = static_cast<float>(mx);
	ltv_mouse_position_.y = static_cast<float>(my);

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
