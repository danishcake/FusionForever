#ifndef TOOLBOXPOPUP_H
#define TOOLBOXPOPUP_H

#include <QMenu>
#include <vector>

class Section;

class ToolboxPopup : public QMenu
{
	Q_OBJECT

public:
	ToolboxPopup(QWidget *parent, QAction* _replace, QAction* _insert_before, QAction* _insert_after);
	~ToolboxPopup();

public slots:
	void selectionChanged(Section* /*_section*/);
	void show_popup(std::string /*_section_type*/);
private slots:
	void slotReplace();
	void slotInsert_after();
	void slotInsert_before();
signals:
	void sigReplace(std::string /*_section_type*/);
	void sigInsert_after(std::string /*_section_type*/);
	void sigInsert_before(std::string /*_section_type*/);

private:
	QAction* replace_;
	QAction* insert_after_;
	QAction* insert_before_;
	std::string section_type_;
};

#endif // TOOLBOXPOPUP_H
