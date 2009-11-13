#include "sectionpopup.h"
#include <Section.h>
#include <boost/lexical_cast.hpp>

SectionPopup::SectionPopup(QWidget *parent, QList<QAction*> _actions)
: QMenu(parent)
{
	addActions(_actions);
	addSeparator();
	selections_ = new QMenu();
	selections_->setTitle("Selection");
	addMenu(selections_);
}

SectionPopup::~SectionPopup()
{

}

void SectionPopup::selectionChanged(Section* _section, std::vector<Section*> _sections)
{
	selections_->clear();
	for(std::vector<Section*>::iterator it = _sections.begin(); it != _sections.end(); ++it)
	{
		std::string name = boost::lexical_cast<std::string, int>((*it)->GetSectionID()) + " :" + (*it)->GetSectionType();
		QAction* select_action = new QAction(name.c_str(), this);
		select_action->setCheckable(true);
		select_action->setData((*it)->GetSectionID());
		if(*it == _section)
			select_action->setChecked(true);

		QObject::connect(select_action, SIGNAL(triggered()), this, SLOT(selection_item_triggered()));
		selections_->addAction(select_action);
	}
	if(!_section->IsCore())
	{
		Section* parent = _section->GetParent();

		std::string name = std::string("Parent: ") + parent->GetSectionType();
		QAction* select_action = new QAction(name.c_str(), this);
		select_action->setCheckable(true);
		select_action->setData(parent->GetSectionID());

		QObject::connect(select_action, SIGNAL(triggered()), this, SLOT(selection_item_triggered()));
		selections_->addSeparator();
		selections_->addAction(select_action);
	}
	std::vector<Section*>& subsections = _section->GetSubsections();
	if(subsections.size())
	{
		selections_->addSeparator();
	}
	for(std::vector<Section*>::iterator it = subsections.begin(); it != subsections.end(); ++it)
	{
		std::string name = std::string("Child: ") + (*it)->GetSectionType();
		QAction* select_action = new QAction(name.c_str(), this);
		select_action->setCheckable(true);
		select_action->setData((*it)->GetSectionID());

		QObject::connect(select_action, SIGNAL(triggered()), this, SLOT(selection_item_triggered()));
		selections_->addAction(select_action);
	}

}

void SectionPopup::show_popup()
{
	popup(QCursor::pos());
}

void SectionPopup::selection_item_triggered()
{
	QAction* action = (QAction*)QObject::sender();
	
	emit select_item(action->data().toInt());
}