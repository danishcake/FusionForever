#ifndef QSDLINPUTDIALOGUE_H
#define QSDLINPUTDIALOGUE_H

#include <QDialog>
#include <SDL_joystick.h>
struct InputConfig;


class QSDLInputDialogue : public QDialog
{
	Q_OBJECT

public:
	QSDLInputDialogue(QWidget *parent, InputConfig* _input_config);
	~QSDLInputDialogue();

	const QString GetKey();
private:
	InputConfig* mInputConfig;
	QString mKey;
	QVector<SDL_Joystick*> mJoysticks;
private slots:
	void CheckInput();
protected:
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
};

#endif // QSDLINPUTDIALOGUE_H
