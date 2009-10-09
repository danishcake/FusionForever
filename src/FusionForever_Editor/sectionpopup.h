#ifndef SECTIONPOPUP_H
#define SECTIONPOPUP_H

#include <QMenu>

class Section;

class SectionPopup : public QMenu
{
	Q_OBJECT

public:
	SectionPopup(QWidget *parent, QList<QAction*> _actions);
	~SectionPopup();

public slots:
	void selectionChanged(Section* /*_section*/, std::vector<Section*> /*_section*/);
	void show_popup();
signals:
	void select_item(int /*Item unique id*/);

private:
	QMenu* selections_;

private slots:
	void selection_item_triggered();
};

#endif // SECTIONPOPUP_H
