#include "fusionforever_editor.h"
#include <qtimer.h>
#include <SectionTypes.h>
#include <SectionMetadata.h>
#include "SectionButton.h"
#include "SectionPopup.h"
#include <set>
#include <QFileDialog>

FusionForever_Editor::FusionForever_Editor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	
	
	QTimer* timer = new QTimer(this);
	timer->setInterval(25);
	QObject::connect(timer, SIGNAL(timeout()), ui.fusionForeverWidget, SLOT(Tick()));
	timer->start();

	//QObject::connect(ui.actionRefresh_Sections, SIGNAL(triggered()), this, SLOT(actionReloadSectionList()));

	QList<QAction*> popup_actions;
	popup_actions.push_back(ui.actionDelete_selection);

	selection_menu_ = new SectionPopup(this, popup_actions);
	QObject::connect(ui.fusionForeverWidget, SIGNAL(selectionChanged(Section* , std::vector<Section*>)), selection_menu_, SLOT(selectionChanged(Section*, std::vector<Section*>))); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(rightClick()), selection_menu_, SLOT(show_popup())); 
	QObject::connect(ui.fusionForeverWidget, SIGNAL(initialisedSections(std::vector<std::pair<std::string, QPixmap*> >)), this, SLOT(reloadSectionList(std::vector<std::pair<std::string, QPixmap*> >))); 
	QObject::connect(selection_menu_, SIGNAL(select_item(int)), ui.fusionForeverWidget, SLOT(SelectSection(int))); 
	
	
	QObject::connect(ui.actionDelete_selection, SIGNAL(triggered()), ui.fusionForeverWidget, SLOT(DeleteSelection()));
	QObject::connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveShip()));
	QObject::connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newShip()));
	QObject::connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openShip()));
}

FusionForever_Editor::~FusionForever_Editor()
{

}

void FusionForever_Editor::reloadSectionList(std::vector<std::pair<std::string, QPixmap*> > _icons)
{
	//std::vector<std::string> names = SectionMetadata::GetSections();
	std::set<std::string> tag_list;
	std::map<std::string, std::vector<std::pair<std::string, QPixmap*> > > tag_entries;
	

	for(std::vector<std::pair<std::string, QPixmap*> >::iterator it = _icons.begin(); it != _icons.end(); ++it)
	{
		
		SectionButton* itemButton = new SectionButton(ui.pageAll, it->first.c_str(), it->first.c_str(), *it->second);
		ui.gridLayoutAll->addWidget(itemButton);

		QObject::connect(itemButton, SIGNAL(sectionClicked(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));

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
		}

		QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		toolbox_page_layout->addItem(spacer);
	}

}

void FusionForever_Editor::saveShip()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
	ui.fusionForeverWidget->Save(filename.toStdString());
}

void FusionForever_Editor::newShip()
{
	ui.fusionForeverWidget->New();
}

void FusionForever_Editor::openShip()
{
	QString filename = QFileDialog::getOpenFileName(this, "Save Ship", "./Scripts/Ships/", "XMLShip format (*.xmlShip)");
//	filename.remove(".xmlShip", Qt::CaseInsensitive);
	ui.fusionForeverWidget->Open(filename.toStdString());
}