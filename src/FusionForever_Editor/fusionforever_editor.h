#ifndef FUSIONFOREVER_EDITOR_H
#define FUSIONFOREVER_EDITOR_H

#include <QtGui/QMainWindow>
#include "ui_fusionforever_editor.h"

class ScenarioDialog;

class FusionForever_Editor : public QMainWindow
{
	Q_OBJECT

public:
	FusionForever_Editor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~FusionForever_Editor();

private:
	Ui::FusionForever_EditorClass ui;
	QMenu* selection_menu_;
	ScenarioDialog* scenario_dialog_;

private slots:
	void reloadSectionList(std::vector<std::pair<std::string, QPixmap*> >);
	void saveShip();
	void newShip();
	void openShip();
	void tryShip();
};

#endif // FUSIONFOREVER_EDITOR_H
