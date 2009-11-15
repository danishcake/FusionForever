#include "toolboxpopup.h"
#include <Section.h>

ToolboxPopup::ToolboxPopup(QWidget *parent, QAction* _replace, QAction* _insert_before, QAction* _insert_after)
: QMenu(parent), replace_(_replace), insert_before_(_insert_before), insert_after_(_insert_after)
{
	addAction(_insert_after);
	addAction(_replace);
	addAction(_insert_before);
	QObject::connect(_insert_after, SIGNAL(triggered()), this, SLOT(slotInsert_after()));
	QObject::connect(_insert_before, SIGNAL(triggered()), this, SLOT(slotInsert_before()));
	QObject::connect(_replace, SIGNAL(triggered()), this, SLOT(slotReplace()));
}

ToolboxPopup::~ToolboxPopup()
{

}

void ToolboxPopup::slotReplace()
{
	emit sigReplace(section_type_);
}

void ToolboxPopup::slotInsert_after()
{
	emit sigInsert_after(section_type_);
}

void ToolboxPopup::slotInsert_before()
{
	emit sigInsert_before(section_type_);
}

void ToolboxPopup::show_popup(std::string _section_type)
{
	section_type_ = _section_type;
	popup(QCursor::pos());
}

void ToolboxPopup::selectionChanged(Section* _section)
{
	if(_section->IsCore())
	{
		replace_->setEnabled(false);
		insert_before_->setEnabled(false);
	} else
	{
		replace_->setEnabled(true);
		insert_before_->setEnabled(true);
	}
}