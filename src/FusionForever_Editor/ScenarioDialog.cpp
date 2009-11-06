#include "ScenarioDialog.h"
#include <QFileDialog>

ScenarioDialog::ScenarioDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);

	enemy_ship_ = "Scripts/Ships/Lasership.xmlShip";
	enemy_ai_ = "Scripts/AI/SimpleAI.luaAI";
	player_ai_ = "PlayerAI0";
	
}

ScenarioDialog::~ScenarioDialog()
{

}

std::string const ScenarioDialog::GetScenarioString()
{
	std::string challenge;
	challenge += "challenge.use_absolute_paths = true\n";
	challenge += "challenge:WaitFor(1)\n";
	challenge += "challenge:SpawnShip(\"";
	challenge += enemy_ship_.toStdString();
	challenge += "\", 1, Vector:new(0, 500), 180, \"";
	challenge += enemy_ai_.toStdString();
	challenge += "\", 1)\n";
	challenge += "challenge:SpawnShip(\"Scripts/Ships/EditorTemp.xmlShip\", 0, Vector:new(0, -500), 0, \"";
	challenge += player_ai_.toStdString();
	challenge += "\", 1)\n";
	challenge += "challenge:WaitFor(0.1)\n";
	challenge += "while challenge.force_count[0] > 0 and challenge.force_count[1] > 0 do\n";
	challenge += "\tcoroutine.yield()\n";
	challenge += "end\n";
	challenge += "challenge:ReturnToEditor()\n";
	return challenge;
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
	selected_player_ai_ = "PlayerAI0";
	ui.lblPlayerAI->setText(selected_player_ai_);
}

void ScenarioDialog::refire_accept()
{
	player_ai_ = selected_player_ai_;
	enemy_ai_ = selected_enemy_ai_;
	enemy_ship_ = selected_enemy_ship_;

	accept();
}