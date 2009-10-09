#ifndef SCENARIO_DIALOG_H
#define SCENARIO_DIALOG_H

#include <QtGui/QDialog>
#include "ui_ScenarioDialog.h"

class ScenarioDialog : public QDialog
{
	Q_OBJECT

public:
	ScenarioDialog(QWidget *parent);
	~ScenarioDialog();

	std::string const GetScenarioString();

private:
	Ui::ScenarioDialog ui;

	QString player_ai_;
	QString enemy_ai_;
	QString enemy_ship_;

	QString selected_player_ai_;
	QString selected_enemy_ai_;
	QString selected_enemy_ship_;


private slots:
	void pickEnemyShip();
	void pickEnemyAI();
	void pickPlayerAI();
	void useKeyboardAI();

	void refire_accept();

public slots:
	void showDialog();
};

#endif // SCENARIO_DIALOG_H
