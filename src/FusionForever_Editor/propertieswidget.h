#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include <map>
class Section;
class QGridLayout;
class QLabel;
class Property;

class PropertiesWidget : public QWidget
{
	Q_OBJECT

public:
	PropertiesWidget(QWidget *parent);
	~PropertiesWidget();

public slots:
	void selectionChanged(Section* /*_section*/);
	void receiveSectionPixmaps(std::vector<std::pair<std::string, QPixmap*> > /*_pixmaps*/);

private:
	std::vector<QWidget*> controls_;
	std::vector<Property*> properties_;
	QGridLayout* layout_;
	std::map<std::string, QPixmap*> pixmaps_;
	QLabel* item_label_;
	QLabel* item_text_;
	
};

#endif // PROPERTIESWIDGET_H
