#ifndef VALUEPROPERTY_H
#define VALUEPROPERTY_H

#include <QDoubleSpinBox>

class Property;

class ValueProperty : public QDoubleSpinBox
{
	Q_OBJECT

public:
	ValueProperty(QWidget *parent, Property* _property);
	~ValueProperty();

private:
	Property* property_;

private slots:
	void onValueChanged(double _value);
};

#endif // VALUEPROPERTY_H
