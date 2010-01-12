#include "qsdlinputdialogue.h"
#include <QLabel>
#include <QHBoxLayout>
#include <sdl.h>
#include <QTimer>
#include <QKeyEvent>
#include <Logger.h>
#include <Binding.h>

QSDLInputDialogue::QSDLInputDialogue(QWidget *parent, InputConfig* _input_config)
: QDialog(parent, Qt::Dialog), mInputConfig(_input_config)
{
	int ok = SDL_Init(SDL_INIT_JOYSTICK) >= 0;
	
	setFixedSize(300, 80);
	setWindowTitle("Waiting for input");

	QLabel* description = new QLabel(this);
	if(ok)
		description->setText("Press button/move axis");
	else
		description->setText("Error initialising SDL");

	
	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		SDL_Joystick* joystick = SDL_JoystickOpen(i);
		if(joystick)
			mJoysticks.push_back(joystick);
	}

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(description);

	setMouseTracking(true);
	mMouseFirstPass = 1;

	QTimer* timer = new QTimer(this);
	timer->setInterval(10);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(CheckInput()));
	timer->start();
}

QSDLInputDialogue::~QSDLInputDialogue()
{
	SDL_Quit(); //Hopefully closes joysticks automatically
}

void QSDLInputDialogue::CheckInput()
{
	SDL_JoystickUpdate();
	for(QVector<SDL_Joystick*>::iterator it = mJoysticks.begin(); it != mJoysticks.end(); ++it)
	{
		for(int i = 0; i < SDL_JoystickNumAxes(*it); i++)
		{
			if(abs(SDL_JoystickGetAxis(*it, i)) > 10000)
			{
				mInputConfig->type = BindingType::JoystickAxisBinding;
				mInputConfig->binding.joystick_axis = JoystickAxis::Create(SDL_JoystickIndex(*it), i);
				done(QDialog::Accepted);
			}
		}
		for(int i = 0; i < SDL_JoystickNumButtons(*it); i++)
		{
			if(SDL_JoystickGetButton(*it, i))
			{
				mInputConfig->type = BindingType::JoystickButtonBinding;
				mInputConfig->binding.joystick_button = JoystickButton::Create(SDL_JoystickIndex(*it), i);
				done(QDialog::Accepted);
			}
		}
	}
}

const QString QSDLInputDialogue::GetKey()
{
	return mKey;
}

void QSDLInputDialogue::mouseReleaseEvent(QMouseEvent* event)
{
	switch(event->button())
	{
	case Qt::LeftButton:
		mInputConfig->type = BindingType::MouseButtonBinding;
		mInputConfig->binding.mouse_button = MouseButton::LeftMouseButton;
		done(QDialog::Accepted);
		break;
	case Qt::RightButton:
		mInputConfig->type = BindingType::MouseButtonBinding;
		mInputConfig->binding.mouse_button = MouseButton::RightMouseButton;
		done(QDialog::Accepted);
		break;
	case Qt::MidButton:
		mInputConfig->type = BindingType::MouseButtonBinding;
		mInputConfig->binding.mouse_button = MouseButton::MiddleMouseButton;
		done(QDialog::Accepted);
		break;
	}
}

void QSDLInputDialogue::mouseMoveEvent(QMouseEvent* event)
{
	if(mMouseFirstPass)
	{
		mMouseStartPosition.setX(event->x());
		mMouseStartPosition.setY(event->y());
		mMouseFirstPass = false;
	}
	if(abs(mMouseStartPosition.x() - event->x()) > 15)
	{
		mInputConfig->type = BindingType::MouseAxisBinding;
		mInputConfig->binding.mouse_axis = MouseAxis::MouseX;
		done(QDialog::Accepted);
	} else if(abs(mMouseStartPosition.y() - event->y()) > 15)
	{
		mInputConfig->type = BindingType::MouseAxisBinding;
		mInputConfig->binding.mouse_axis = MouseAxis::MouseY;
		done(QDialog::Accepted);
	}
}

void QSDLInputDialogue::keyReleaseEvent(QKeyEvent* event)
{
	int qtkey = event->key();
	SDLKey sdlkey = SDLK_FIRST;
	Logger::DiagnosticOut() << "Key pressed " << qtkey << "\n";
	//Qt:Key largely ascii, so can translate that bit easily
	if(qtkey >=Qt::Key_Space && qtkey <= Qt::Key_AsciiTilde)
	{
		if(qtkey >= Qt::Key_A && qtkey <= Qt::Key_Z)
			sdlkey = (SDLKey)(qtkey + 32);
		else
			sdlkey = (SDLKey)qtkey;
	}
	if(sdlkey != SDLK_FIRST) mKey = event->text();
	if(qtkey == Qt::Key_Shift) {sdlkey = SDLK_LSHIFT; mKey = "Shift";}
	if(qtkey == Qt::Key_Control) {sdlkey = SDLK_LCTRL; mKey = "Control";}
	if(qtkey == Qt::Key_Alt || qtkey == Qt::Key_AltGr) {sdlkey = SDLK_LALT; mKey = "Alt";}
	
	if(qtkey == Qt::Key_CapsLock) {sdlkey = SDLK_CAPSLOCK; mKey = "Capslock";}
	if(qtkey == Qt::Key_Tab) {sdlkey = SDLK_TAB; mKey = "Tab";}
	if(qtkey == Qt::Key_Return || qtkey == Qt::Key_Enter) {sdlkey = SDLK_RETURN; mKey = "Enter";}
	
	if(qtkey == Qt::Key_Insert) {sdlkey = SDLK_INSERT; mKey = "Insert";}
	if(qtkey == Qt::Key_Delete) {sdlkey = SDLK_DELETE; mKey = "Delete";}
	if(qtkey == Qt::Key_Home) {sdlkey = SDLK_HOME; mKey = "Home";}
	if(qtkey == Qt::Key_End) {sdlkey = SDLK_END; mKey = "End";}
	if(qtkey == Qt::Key_PageUp) {sdlkey = SDLK_PAGEUP; mKey = "Page Up";}
	if(qtkey == Qt::Key_PageDown) {sdlkey = SDLK_PAGEDOWN; mKey = "Page Down";}
	if(qtkey == Qt::Key_Left) {sdlkey = SDLK_LEFT; mKey = "Left Arrow";}
	if(qtkey == Qt::Key_Up) {sdlkey = SDLK_UP; mKey = "Up Arrow";}
	if(qtkey == Qt::Key_Right) {sdlkey = SDLK_RIGHT; mKey = "Right Arrow";}
	if(qtkey == Qt::Key_Down) {sdlkey = SDLK_DOWN; mKey = "Down Arrow";}
	
	if(sdlkey != SDLK_FIRST)
	{
		mInputConfig->type = BindingType::KeyboardBinding;
		mInputConfig->binding.key = sdlkey;
		done(QDialog::Accepted);
	}

}