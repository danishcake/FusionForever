#include "fusionforever_editor.h"
#include <QtGui/QApplication>
#include <Logger.h>


int main(int argc, char *argv[])
{
	Logger::SetPrefix("Editor");

	QApplication a(argc, argv);
	FusionForever_Editor w;
	w.show();
	return a.exec();
}
