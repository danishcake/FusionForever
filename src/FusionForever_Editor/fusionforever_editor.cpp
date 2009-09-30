#include "fusionforever_editor.h"
#include <qtimer.h>

FusionForever_Editor::FusionForever_Editor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QTimer* timer = new QTimer(this);
	timer->setInterval(25);
	QObject::connect(timer, SIGNAL(timeout()), ui.fusionForeverWidget, SLOT(Tick()));
	timer->start();
}

FusionForever_Editor::~FusionForever_Editor()
{

}
