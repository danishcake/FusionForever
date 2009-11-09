#include "enumproperty.h"
#include <Property.h>

EnumProperty::EnumProperty(QWidget *parent, Property* _property)
	: QComboBox(parent), property_(_property)
{
	setEditable(false);
	Enumeration options = property_->GetEnumeration();
	for(Enumeration::iterator it = options.begin(); it != options.end(); ++it)
	{
		addItem(it->second.c_str(), QVariant(it->first));
	}
	if(property_->HasGetter())
	{
		int index = findText(property_->GetEnumerationItem().c_str());
		if(index != -1)
			setCurrentIndex(index);
	}
	
	QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));
}

EnumProperty::~EnumProperty()
{

}

void EnumProperty::onIndexChanged(int _index)
{
	if(property_->HasSetter())
	{
		property_->SetEnumerationValue(itemData(currentIndex()).toInt());
	}
}