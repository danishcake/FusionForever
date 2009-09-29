#include "fusionforever_editor.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FusionForever_Editor w;
	w.show();
	return a.exec();
}
