#include "valueproperty.h"
#include <Property.h>

ValueProperty::ValueProperty(QWidget *parent, Property* _property)
	: QDoubleSpinBox(parent), property_(_property)
{
	setRange(0, 100000);
	setSingleStep(0.1);
	if(property_->HasGetter())
	{
		setValue(property_->GetFloat());
	}
	QObject::connect(this, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
}

ValueProperty::~ValueProperty()
{

}

void ValueProperty::onValueChanged(double _value)
{
	if(property_->HasSetter())
	{
		property_->SetFloat(_value);
	}
}