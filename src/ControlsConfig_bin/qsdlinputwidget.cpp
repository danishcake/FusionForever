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

void QSDLInputWidget::SetDetails(QString _action, QString _description)
{
	mActionName->setText(_action);
	mActionDescription->setText(_description);
	mInputConfig.action = Action::FromStr(_action.toStdString());
}
QString QSDLInputWidget::GetAction()
{
	return mActionName->text();
}

void QSDLInputWidget::SetBinding(InputConfig _input_config)
{
	mInputConfig = _input_config;
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
		Logger::ErrorOut() << "accepted\n";
		std::string conf;
		switch(mInputConfig.type)
		{
		case BindingType::KeyboardBinding:
			conf = std::string("Key: ") + dialogue.GetKey().toStdString();
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
		}
		mConfigure->setText(conf.c_str());

	} else
		Logger::ErrorOut() << "rejected\n";
}