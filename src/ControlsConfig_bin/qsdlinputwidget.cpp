#include "qsdlinputwidget.h"
#include <boost/lexical_cast.hpp>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSDLInputDialogue.h>

QSDLInputWidget::QSDLInputWidget(QWidget *parent)
: QWidget(parent), mInputConfig(BindingType::Unbound, Binding(), Action::Boost)
{
	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);
	mActionName = new QLabel("ACTION", this);
	mActionDescription = new QLabel("Description", this);
	mConfigure = new QPushButton(this);
	mConfigure->setText("Not bound");
	QObject::connect(mConfigure, SIGNAL(clicked()), this, SLOT(Configure()));
	
	
	layout->addWidget(mActionName);
	layout->addWidget(mActionDescription);
	layout->addWidget(mConfigure);
}

QSDLInputWidget::~QSDLInputWidget()
{

}

void QSDLInputWidget::SetDetails(QString _action_str, QString _description, Action::Enum _action)
{
	mActionName->setText(_action_str);
	mActionDescription->setText(_description);
	mInputConfig.action = _action;
}

QString QSDLInputWidget::GetAction()
{
	return mActionName->text();
}

void QSDLInputWidget::SetBinding(InputConfig _input_config)
{
	mInputConfig = _input_config;
	UpdateInputConfig();
}
void QSDLInputWidget::ClearBinding()
{
	mInputConfig.type = BindingType::Unbound;
	UpdateInputConfig();
}

InputConfig& QSDLInputWidget::GetBinding()
{
	return mInputConfig;
}

void QSDLInputWidget::Configure()
{
	QSDLInputDialogue dialogue(this, &mInputConfig);
	if(dialogue.exec() == QDialog::Accepted)
	{
		UpdateInputConfig();
	} else
		Logger::ErrorOut() << "rejected\n";
}

void QSDLInputWidget::UpdateInputConfig()
{
	std::string conf;
	switch(mInputConfig.type)
	{
	case BindingType::KeyboardBinding:
		{
			if(mInputConfig.binding.key > SDLK_SPACE && mInputConfig.binding.key <= SDLK_DELETE)
			{
				char ascii[2];
				ascii[1] = '\0';
				ascii[0] = (char)mInputConfig.binding.key;
				conf = ascii;
			}
			if(mInputConfig.binding.key == SDLK_SPACE) conf = "Space";
			if(mInputConfig.binding.key == SDLK_LSHIFT) conf = "Left Shift";
			if(mInputConfig.binding.key == SDLK_RSHIFT) conf = "Right Shift";
			if(mInputConfig.binding.key == SDLK_LCTRL) conf = "Left Control";
			if(mInputConfig.binding.key == SDLK_RCTRL) conf = "Right Control";
			if(mInputConfig.binding.key == SDLK_LALT) conf = "Left Alt";
			if(mInputConfig.binding.key == SDLK_RALT) conf = "Right Alt";

			if(mInputConfig.binding.key == SDLK_CAPSLOCK) conf = "Capslock";
			if(mInputConfig.binding.key == SDLK_TAB) conf = "Tab";
			if(mInputConfig.binding.key == SDLK_RETURN) conf = "Enter";

			if(mInputConfig.binding.key == SDLK_INSERT) conf = "Insert";
			if(mInputConfig.binding.key == SDLK_DELETE) conf = "Delete";
			if(mInputConfig.binding.key == SDLK_HOME) conf = "Home";
			if(mInputConfig.binding.key == SDLK_END) conf = "End";
			if(mInputConfig.binding.key == SDLK_PAGEUP) conf = "Page Up";
			if(mInputConfig.binding.key == SDLK_PAGEDOWN) conf = "Page Down";
			if(mInputConfig.binding.key == SDLK_LEFT) conf = "Left Arrow";
			if(mInputConfig.binding.key == SDLK_UP) conf = "Up Arrow";
			if(mInputConfig.binding.key == SDLK_RIGHT) conf = "Right Arrow";
			if(mInputConfig.binding.key == SDLK_DOWN) conf = "Down Arrow";
		}
		conf = std::string("Key: ") + conf;
		break;
	case BindingType::MouseButtonBinding:
		conf = std::string("Mouse button: ") + MouseButton::ToStr[mInputConfig.binding.mouse_button];
		break;
	case BindingType::MouseAxisBinding:
		conf = std::string("Mouse axis: ") + MouseAxis::ToStr[mInputConfig.binding.mouse_axis];
		break;
	case BindingType::JoystickButtonBinding:
		conf = std::string("Joystick: ") + boost::lexical_cast<std::string, int>(mInputConfig.binding.joystick_button.joystick_index) + std::string(" button: ") + boost::lexical_cast<std::string, int>(mInputConfig.binding.joystick_button.button_index);
		break;
	case BindingType::JoystickAxisBinding:
		conf = std::string("Joystick axis:") + boost::lexical_cast<std::string, int>(mInputConfig.binding.joystick_axis.joystick_index) + std::string(" axis: ") + boost::lexical_cast<std::string, int>(mInputConfig.binding.joystick_axis.axis_index);
		break;
	case BindingType::Unbound:
		conf = "Not bound"; 
		break;
	}
	mConfigure->setText(conf.c_str());
}