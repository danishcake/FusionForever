#ifndef FUSIONFOREVER_EDITOR_H
#define FUSIONFOREVER_EDITOR_H

#include <QtGui/QMainWindow>
#include "ui_fusionforever_editor.h"

class FusionForever_Editor : public QMainWindow
{
	Q_OBJECT

public:
	FusionForever_Editor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~FusionForever_Editor();

private:
	Ui::FusionForever_EditorClass ui;
	QMenu* selection_menu_;

private slots:
	void reloadSectionList();
	void saveShip();
	void newShip();
	void openShip();
};

#endif // FUSIONFOREVER_EDITOR_H
