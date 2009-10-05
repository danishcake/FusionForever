#include "fusionforever_editor.h"
#include <qtimer.h>
#include <SectionTypes.h>
#include <SectionMetadata.h>
#include <SectionButton.h>
#include <set>

FusionForever_Editor::FusionForever_Editor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QTimer* timer = new QTimer(this);
	timer->setInterval(25);
	QObject::connect(timer, SIGNAL(timeout()), ui.fusionForeverWidget, SLOT(Tick()));
	timer->start();


	QObject::connect(ui.actionRefresh_Sections, SIGNAL(triggered()), this, SLOT(actionReloadSectionList()));
}

FusionForever_Editor::~FusionForever_Editor()
{

}

void FusionForever_Editor::actionReloadSectionList()
{
	
	std::vector<std::string> names = SectionMetadata::GetSections();
	std::set<std::string> tag_list;
	std::map<std::string, std::vector<std::string> > tag_entries;
	

	for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
	{
		SectionButton* itemButton = new SectionButton(ui.pageAll, it->c_str(), it->c_str());
		ui.gridLayoutAll->addWidget(itemButton);

		QObject::connect(itemButton, SIGNAL(sectionClicked(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));

		std::vector<std::string> tags = SectionMetadata::GetTags(*it);
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

		for(std::vector<std::string>::iterator it2 = tag_entries[*it].begin(); it2 != tag_entries[*it].end(); ++it2)
		{
			QPushButton* itemButton = new QPushButton(it2->c_str(), toolbox_page);
			QObject::connect(itemButton, SIGNAL(sectionClicked(std::string)), ui.fusionForeverWidget, SLOT(AddSection(std::string)));
			toolbox_page_layout->addWidget(itemButton);
		}

		QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		toolbox_page_layout->addItem(spacer);
	}

}