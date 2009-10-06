#include "ScenarioDialog.h"
#include <QFileDialog>

ScenarioDialog::ScenarioDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);

	enemy_ship_ = "Lasership";
	player_ai_ = "KeyboardAI";
	enemy_ai_ = "SimpleAI";
}

ScenarioDialog::~ScenarioDialog()
{

}

void ScenarioDialog::showDialog()
{
	selected_enemy_ship_ = enemy_ship_;
	selected_enemy_ai_ = enemy_ai_;
	selected_player_ai_ = player_ai_;

	ui.lblEnemy->setText(selected_enemy_ship_);
	ui.lblEnemyAI->setText(selected_enemy_ai_);
	ui.lblPlayerAI->setText(selected_player_ai_);
	exec();
}

/* Private slots */
void ScenarioDialog::pickEnemyShip()
{
	QString filename = QFileDialog::getOpenFileName(this, "Enemy Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
	if(filename.length() > 0)
	{
		selected_enemy_ship_ = filename;
		ui.lblEnemy->setText(selected_enemy_ship_);
	}
}

void ScenarioDialog::pickEnemyAI()
{
	QString filename = QFileDialog::getOpenFileName(this, "Enemy AI", "./Scripts/AI/", "luaAIformat (*.luaAI)");
	if(filename.length() > 0)
	{
		selected_enemy_ai_ = filename;
		ui.lblEnemyAI->setText(selected_enemy_ai_);
	}
}

void ScenarioDialog::pickPlayerAI()
{
	QString filename = QFileDialog::getOpenFileName(this, "Player AI", "./Scripts/AI/", "luaAIformat (*.luaAI)");
	if(filename.length() > 0)
	{
		selected_player_ai_ = filename;
		ui.lblPlayerAI->setText(selected_player_ai_);
	}
}

void ScenarioDialog::useKeyboardAI()
{
	selected_player_ai_ = "KeyboardAI";
	ui.lblPlayerAI->setText(selected_player_ai_);
}

void ScenarioDialog::refire_accept()
{
	player_ai_ = selected_player_ai_;
	enemy_ai_ = selected_enemy_ai_;
	enemy_ship_ = selected_enemy_ship_;

	accept();
}