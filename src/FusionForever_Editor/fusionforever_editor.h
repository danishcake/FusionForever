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
	std::string filename_;

private slots:
	void reloadSectionList(std::vector<std::pair<std::string, QPixmap*> >);
	void saveShip();
	void saveShipAs();
	void newShip();
	void openShip();
	void tryShip();

	void setGridSize0_5();
	void setGridSize1();
	void setGridSize2_5();
};

#endif // FUSIONFOREVER_EDITOR_H
