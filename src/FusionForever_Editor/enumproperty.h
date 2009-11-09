#ifndef ENUMPROPERTY_H
#define ENUMPROPERTY_H

#include <QComboBox>

class Property;

class EnumProperty : public QComboBox
{
	Q_OBJECT

public:
	EnumProperty(QWidget *parent, Property* _property);
	~EnumProperty();

private:
	Property* property_;

private slots:
	void onIndexChanged(int _index);
};

#endif // ENUMPROPERTY_H
