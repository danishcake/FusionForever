#include "fusionforever_editor.h"
#include <qtimer.h>
#include <SectionTypes.h>
#include <SectionMetadata.h>
#include "SectionButton.h"
#include "SectionPopup.h"
#include "ToolboxPopup.h"
#include <set>
#include <QFileDialog>
#include "ScenarioDialog.h"
#include <stdlib.h>
#include <fstream>

FusionForever_Editor::FusionForever_Editor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	
	scenario_dialog_ = new ScenarioDialog(this);
	scenario_dialog_->setModal(true);

	QTimer* timer = new QTimer(this);
	timer->setInterval(25);
	QObject::connect(timer, SIGNAL(timeout()), ui.fusionForeverWidget, SLOT(Tick()));
	timer->start();

	QList<QAction*> popup_actions;
	popup_actions.push_back(ui.actionDelete_selection);
	popup_actions.push_back(ui.actionRemove);
	selection_menu_ = new SectionPopup(this, popup_actions);

	toolbox_menu_ = new ToolboxPopup(this, ui.actionReplace, ui.actionAdd_as_parent, ui.actionAdd_as_child);

	QObject::connect(ui.fusionForeverWidget, SIGNAL(selectionChanged(Section*, std::vector<Section*>)), selection_menu_, SLOT(selectionChanged(Section*, std::vector<Section*>))); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(selectionChanged(Section*)), ui.propertiesList, SLOT(selectionChanged(Section*))); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(selectionChanged(Section*)), toolbox_menu_, SLOT(selectionChanged(Section*))); 

	QObject::connect(ui.fusionForeverWidget, SIGNAL(rightClick()), selection_menu_, SLOT(show_popup())); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(initialisedSections(std::vector<std::pair<std::string, QPixmap*> >)), this, SLOT(reloadSectionList(std::vector<std::pair<std::string, QPixmap*> >))); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(initialisedSections(std::vector<std::pair<std::string, QPixmap*> >)), ui.propertiesList, SLOT(receiveSectionPixmaps(std::vector<std::pair<std::string, QPixmap*> >))); 
	
	QObject::connect(selection_menu_, SIGNAL(select_item(int)), ui.fusionForeverWidget, SLOT(SelectSection(int))); 
	QObject::connect(toolbox_menu_, SIGNAL(sigInsert_after(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));
	QObject::connect(toolbox_menu_, SIGNAL(sigInsert_before(std::string)), ui.fusionForeverWidget, SLOT(InsertSection(std::string)));
	QObject::connect(toolbox_menu_, SIGNAL(sigReplace(std::string)), ui.fusionForeverWidget, SLOT(ReplaceSection(std::string)));
	
	QObject::connect(ui.actionDelete_selection, SIGNAL(triggered()), ui.fusionForeverWidget, SLOT(DeleteSelection()));
	QObject::connect(ui.actionRemove, SIGNAL(triggered()), ui.fusionForeverWidget, SLOT(RemoveSelection()));

	QObject::connect(ui.actionSet_snap_to_0_5, SIGNAL(triggered()), this, SLOT(setGridSize0_5()));
	QObject::connect(ui.actionSet_snap_to_1, SIGNAL(triggered()), this, SLOT(setGridSize1()));
	QObject::connect(ui.actionSet_snap_to_2_5, SIGNAL(triggered()), this, SLOT(setGridSize2_5()));
	QObject::connect(ui.actionIncrease_snap, SIGNAL(triggered()), ui.fusionForeverWidget, SLOT(IncreaseGridSize()));
	QObject::connect(ui.actionDecrease_snap, SIGNAL(triggered()), ui.fusionForeverWidget, SLOT(DecreaseGridSize()));



	QObject::connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveShip()));
	QObject::connect(ui.actionSave_As, SIGNAL(triggered()), this, SLOT(saveShipAs()));
	QObject::connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newShip()));
	QObject::connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openShip()));
	QObject::connect(ui.actionConfigure, SIGNAL(triggered()), scenario_dialog_, SLOT(showDialog()));
	QObject::connect(ui.actionTry, SIGNAL(triggered()), this, SLOT(tryShip()));



	
}

FusionForever_Editor::~FusionForever_Editor()
{

}

void FusionForever_Editor::reloadSectionList(std::vector<std::pair<std::string, QPixmap*> > _icons)
{
	std::set<std::string> tag_list;
	std::map<std::string, std::vector<std::pair<std::string, QPixmap*> > > tag_entries;
	

	for(std::vector<std::pair<std::string, QPixmap*> >::iterator it = _icons.begin(); it != _icons.end(); ++it)
	{
		
		SectionButton* itemButton = new SectionButton(ui.pageAll, it->first.c_str(), it->first.c_str(), *it->second);
		ui.gridLayoutAll->addWidget(itemButton);

		QObject::connect(itemButton, SIGNAL(sectionClicked(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));
		QObject::connect(itemButton, SIGNAL(sectionRightClick(std::string)), toolbox_menu_, SLOT(show_popup(std::string)));

		std::vector<std::string> tags = SectionMetadata::GetTags(it->first);
		for(std::vector<std::string>::iterator tag = tags.begin(); tag != tags.end(); ++tag)
		{
			tag_list.insert(*tag);
			tag_entries[*tag].push_back(*it);
		}
	}

	for(std::set<std::string>::iterator it = tag_list.begin(); it != tag_list.end(); ++it)
	{
		QWidget* toolbox_page = new QWidget();
		QVBoxLayout* toolbox_page_layout = new QVBoxLayout(toolbox_page);
		toolbox_page_layout->setSpacing(3);
		toolbox_page_layout->setMargin(0);
		ui.toolBoxSections->addItem(toolbox_page, it->c_str());

		for(std::vector<std::pair<std::string, QPixmap*> >::iterator it2 = tag_entries[*it].begin(); it2 != tag_entries[*it].end(); ++it2)
		{
			QPushButton* itemButton = new SectionButton(toolbox_page, it2->first.c_str(), it2->first.c_str(), *it2->second);
			toolbox_page_layout->addWidget(itemButton);

			QObject::connect(itemButton, SIGNAL(sectionClicked(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));
			QObject::connect(itemButton, SIGNAL(sectionRightClick(std::string)), toolbox_menu_, SLOT(show_popup(std::string)));
		}

		QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		toolbox_page_layout->addItem(spacer);
	}
}

/* Action handlers */

void FusionForever_Editor::saveShip()
{
	if(filename_.length() == 0)
	{
		QString filename = QFileDialog::getSaveFileName(this, "Save Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
		if(filename.length() != 0)
		{
			filename_ = filename.toStdString();
			ui.fusionForeverWidget->Save(filename_);
		}
	} else
	{
		ui.fusionForeverWidget->Save(filename_);
	}
}

void FusionForever_Editor::saveShipAs()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
	if(filename.length() != 0)
	{
		filename_ = filename.toStdString();
		ui.fusionForeverWidget->Save(filename_);
	}
}

void FusionForever_Editor::newShip()
{
	ui.fusionForeverWidget->New();
	filename_ = "";
}

void FusionForever_Editor::openShip()
{
	QString filename = QFileDialog::getOpenFileName(this, "Save Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
	filename_ = filename.toStdString();
	if(filename_.length() > 0)
		ui.fusionForeverWidget->Open(filename_);
}

void FusionForever_Editor::tryShip()
{
	ui.fusionForeverWidget->Save("Scripts/Ships/EditorTemp.xmlShip");

	std::ofstream challenge = std::ofstream("Scripts/Challenges/EditorTemp.luaChallenge");
	challenge << scenario_dialog_->GetScenarioString();
	challenge.close();

	std::string launch_cmd = "FusionForever_bin.exe -s Scripts/Challenges/EditorTemp.luaChallenge";
	system(launch_cmd.c_str());
}

void FusionForever_Editor::setGridSize0_5()
{
	ui.fusionForeverWidget->SetGridSize(0.5f);
}

void FusionForever_Editor::setGridSize1()
{
	ui.fusionForeverWidget->SetGridSize(1.0f);
}

void FusionForever_Editor::setGridSize2_5()
{
	ui.fusionForeverWidget->SetGridSize(2.5f);
}