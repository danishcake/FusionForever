#include "controlsconfig_bin.h"
#include <PlayerAI.h>
#include <QMessageBox>
#include <QCloseEvent>

ControlsConfig_bin::ControlsConfig_bin(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	PlayerAI::LoadBindings();

	ui.setupUi(this);
	
	ui.conf_MoveLeft->SetDetails("Left", "Move left", Action::MoveLeft);
	ui.conf_MoveRight->SetDetails("Right", "Move right", Action::MoveRight);
	ui.conf_MoveUp->SetDetails("Up", "Move up", Action::MoveUp);
	ui.conf_MoveDown->SetDetails("Down", "Move down", Action::MoveDown);
	

	ui.conf_LookXAxis->SetDetails("X look", "Set direction in X", Action::LookXAxis);
	ui.conf_LookYAxis->SetDetails("Y look", "Set direction in Y", Action::LookYAxis);
	ui.conf_MoveXAxis->SetDetails("X Move", "Analogue x movement", Action::XMovement);
	ui.conf_MoveYAxis->SetDetails("Y Move", "Analogue y movement", Action::YMovement);
	
	ui.conf_Fire->SetDetails("Fire", "Fires all weapons", Action::Fire);
	ui.conf_Boost->SetDetails("Boost", "Moves faster", Action::Boost);
	ui.conf_Target->SetDetails("Target", "Sets target", Action::Target);

	ui.conf_LookBack->SetDetails("Look back", "Looks backwards while held", Action::LookBackwards);
	ui.conf_LockAngle->SetDetails("Lock angle", "Prevents changes to angle while held", Action::LockAngle);
	ui.conf_LockMovement->SetDetails("Lock position", "Prevents changes to position while held", Action::LockMovement);

	QObject::connect(ui.cbox_player_id, SIGNAL(activated(int)), this, SLOT(player_id_changed(int)));
	QObject::connect(ui.actionExit_without_Saving, SIGNAL(triggered()), this, SLOT(exit_no_save()));
	QObject::connect(ui.actionSave_and_Exit, SIGNAL(triggered()), this, SLOT(exit_and_save()));
	QObject::connect(ui.actionReset_to_default, SIGNAL(triggered()), this, SLOT(reset_to_default()));

	old_player_id = -1;
	player_id_changed(0);
	menu_exit = false;
}

ControlsConfig_bin::~ControlsConfig_bin()
{
}

void ControlsConfig_bin::closeEvent(QCloseEvent *event)
{
	if(!menu_exit)
	{
		QMessageBox msgBox;
		msgBox.setText("The document may have been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if(ret == QMessageBox::Save)
		{
			player_id_changed(old_player_id);
			PlayerAI::SaveBindings();
			event->accept();
		} else if(ret == QMessageBox::Discard)
		{
			event->accept();
		} else
		{
			event->ignore();
		}
	}
}

void ControlsConfig_bin::player_id_changed(int player_id)
{
	std::map<int, std::vector<InputConfig> >& bindings = PlayerAI::GetBindings();

	if(old_player_id >= 0)
	{
		bindings[old_player_id].clear();	
		if(ui.conf_MoveUp->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveUp->GetBinding());
		if(ui.conf_MoveDown->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveDown->GetBinding());
		if(ui.conf_MoveLeft->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveLeft->GetBinding());
		if(ui.conf_MoveRight->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveRight->GetBinding());
		if(ui.conf_Fire->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_Fire->GetBinding());
		if(ui.conf_Boost->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_Boost->GetBinding());
		if(ui.conf_Target->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_Target->GetBinding());
		if(ui.conf_LookXAxis->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_LookXAxis->GetBinding());
		if(ui.conf_LookYAxis->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_LookYAxis->GetBinding());
		if(ui.conf_MoveXAxis->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveXAxis->GetBinding());
		if(ui.conf_MoveYAxis->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_MoveYAxis->GetBinding());

		if(ui.conf_LookBack->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_LookBack->GetBinding());
		if(ui.conf_LockAngle->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_LockAngle->GetBinding());
		if(ui.conf_LockMovement->GetBinding().type != BindingType::Unbound) bindings[old_player_id].push_back(ui.conf_LockMovement->GetBinding());
	}
	old_player_id = player_id;

	//Clear
	ui.conf_MoveUp->ClearBinding();
	ui.conf_MoveDown->ClearBinding();
	ui.conf_MoveLeft->ClearBinding();
	ui.conf_MoveRight->ClearBinding();
	ui.conf_Boost->ClearBinding();
	ui.conf_Fire->ClearBinding();
	ui.conf_Target->ClearBinding();
	ui.conf_LookXAxis->ClearBinding();
	ui.conf_LookYAxis->ClearBinding();
	ui.conf_MoveXAxis->ClearBinding();
	ui.conf_MoveYAxis->ClearBinding();
	ui.conf_LookBack->ClearBinding();
	ui.conf_LockAngle->ClearBinding();
	ui.conf_LockMovement->ClearBinding();

	//Load 
	if(bindings.find(player_id) != bindings.end())
	{
		std::vector<InputConfig> confs = bindings[player_id];
		for(std::vector<InputConfig>::iterator it = confs.begin(); it != confs.end(); ++it)
		{
			switch(it->action)
			{
			case Action::MoveUp:
				ui.conf_MoveUp->SetBinding(*it);
				break;
			case Action::MoveDown:
				ui.conf_MoveDown->SetBinding(*it);
				break;
			case Action::MoveLeft:
				ui.conf_MoveLeft->SetBinding(*it);
				break;
			case Action::MoveRight:
				ui.conf_MoveRight->SetBinding(*it);
				break;
			case Action::Fire:
				ui.conf_Fire->SetBinding(*it);
				break;
			case Action::Boost:
				ui.conf_Boost->SetBinding(*it);
				break;
			case Action::Target:
				ui.conf_Target->SetBinding(*it);
				break;
			case Action::XMovement:
				ui.conf_MoveXAxis->SetBinding(*it);
				break;
			case Action::YMovement:
				ui.conf_MoveYAxis->SetBinding(*it);
				break;
			case Action::LookXAxis:
				ui.conf_LookXAxis->SetBinding(*it);
				break;
			case Action::LookYAxis:
				ui.conf_LookYAxis->SetBinding(*it);
				break;
			case Action::LockAngle:
				ui.conf_LockAngle->SetBinding(*it);
				break;
			case Action::LockMovement:
				ui.conf_LockMovement->SetBinding(*it);
				break;
			case Action::LookBackwards:
				ui.conf_LookBack->SetBinding(*it);
				break;

			}
		}
	}
}


void ControlsConfig_bin::exit_no_save()
{
	menu_exit = 1;
	exit(0);
}

void ControlsConfig_bin::exit_and_save()
{
	menu_exit = 1;
	player_id_changed(old_player_id);
	PlayerAI::SaveBindings();
	exit(0);
}

void ControlsConfig_bin::reset_to_default()
{
	PlayerAI::ResetToDefault();
	int opid = old_player_id;
	old_player_id = -1; //Prevents player_id_changed from saving
	player_id_changed(opid== -1 ? 0: opid);
}