#include "propertieswidget.h"
#include <Section.h>
#include <Property.h>
#include <Logger.h>

#include <QLabel>
#include <QGridLayout>
#include <EnumProperty.h>
#include <ValueProperty.h>
#include <QLabel>

PropertiesWidget::PropertiesWidget(QWidget *parent)
	: QWidget(parent)
{
	layout_ = new QGridLayout();
	this->setLayout(layout_);
	item_label_ = new QLabel(this);
	item_label_->setScaledContents(true);
	item_label_->setMaximumSize(80, 80);
	item_label_->setStyleSheet("QLabel { border: 1px solid black }");
	layout_->addWidget(item_label_, 0, 0);

	item_text_ = new QLabel(this);
	layout_->addWidget(item_text_, 0, 1);
}

PropertiesWidget::~PropertiesWidget()
{

}

/*
 * There is a potential bug here - as sections and core lists are merged together 
 * there is no way to tell them apart, so if a core has the same name as a section
 * then the section icon will be drawn as the core
 */

void PropertiesWidget::receiveSectionPixmaps(std::vector<std::pair<std::string, QPixmap*> > _section_pixmaps, std::vector<std::pair<std::string, QPixmap*> > _core_pixmaps)
{
	for(std::vector<std::pair<std::string, QPixmap*> >::iterator it = _section_pixmaps.begin(); it != _section_pixmaps.end(); ++it)
	{
		pixmaps_[it->first] = it->second;
	}
	for(std::vector<std::pair<std::string, QPixmap*> >::iterator it = _core_pixmaps.begin(); it != _core_pixmaps.end(); ++it)
	{
		pixmaps_[it->first] = it->second;
	}
}

void PropertiesWidget::selectionChanged(Section* _section)
{
	for(std::vector<QWidget*>::iterator it = controls_.begin(); it != controls_.end(); ++it)
	{
		delete *it;
	}
	controls_.clear();
	if(pixmaps_.find(_section->GetSectionType()) != pixmaps_.end())
	{
		item_label_->setPixmap(*pixmaps_[_section->GetSectionType()]);
	}
	item_text_->setText(_section->GetSectionType().c_str());

	properties_.clear();
	_section->GetProperties(properties_);
	int row = 1;
	for(std::vector<Property*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
	{
		switch((*it)->Type())
		{
		case PropertyType::Float:
			{
				QLabel* text = new QLabel(this);
				text->setText((*it)->GetDescription().c_str());
				layout_->addWidget(text, row, 0);

				ValueProperty* entry = new ValueProperty(this, *it);
				
				layout_->addWidget(entry, row, 1);

				controls_.push_back(text);
				controls_.push_back(entry);
			}
			break;
		case PropertyType::EnumeratedList:
			{
				QLabel* text = new QLabel(this);
				text->setText((*it)->GetDescription().c_str());
				layout_->addWidget(text, row, 0);

				EnumProperty* entry = new EnumProperty(this, *it);
				layout_->addWidget(entry, row, 1);

				controls_.push_back(text);
				controls_.push_back(entry);
			}
			break;
		}
		row++;
	}
}