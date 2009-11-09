#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
class Section;
class QGridLayout;
class Property;

class PropertiesWidget : public QWidget
{
	Q_OBJECT

public:
	PropertiesWidget(QWidget *parent);
	~PropertiesWidget();

public slots:
	void selectionChanged(Section* /*_section*/);

private:
	std::vector<QWidget*> controls_;
	std::vector<Property*> properties_;
	QGridLayout* layout_;
	
};

#endif // PROPERTIESWIDGET_H
