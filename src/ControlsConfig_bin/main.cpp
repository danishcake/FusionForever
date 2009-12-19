#include "controlsconfig_bin.h"
#include <QtGui/QApplication>
#include <Logger.h>

int main(int argc, char *argv[])
{
	Logger::SetPrefix("ControlsConfig");
	QApplication a(argc, argv);
	ControlsConfig_bin w;
	w.show();
	return a.exec();
}
