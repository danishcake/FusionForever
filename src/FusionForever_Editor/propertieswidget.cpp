#include "propertieswidget.h"
#include <Section.h>
#include <Property.h>
#include <Logger.h>

#include <QLabel>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <ValueProperty.h>

PropertiesWidget::PropertiesWidget(QWidget *parent)
	: QWidget(parent)
{
	layout_ = new QGridLayout();
	this->setLayout(layout_);
}

PropertiesWidget::~PropertiesWidget()
{

}

void PropertiesWidget::selectionChanged(Section* _section)
{
	for(std::vector<QWidget*>::iterator it = controls_.begin(); it != controls_.end(); ++it)
	{
		delete *it;
	}
	controls_.clear();
	//delete layout_;

	properties_.clear();
	_section->GetProperties(properties_);
	int row = 0;
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

				QComboBox* entry = new QComboBox(this);
				entry->setEditable(false);
				Enumeration options = (*it)->GetEnumeration();
				for(Enumeration::iterator it = options.begin(); it != options.end(); ++it)
				{
					entry->addItem(it->second.c_str(), QVariant(it->first));
				}
				layout_->addWidget(entry, row, 1);

				controls_.push_back(text);
				controls_.push_back(entry);
			}
			break;
		}
		row++;
	}
}