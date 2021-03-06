#ifndef SECTION_BUTTON_H
#define SECTION_BUTTON_H

#include <QPushButton>

class SectionButton : public QPushButton
{
	Q_OBJECT

public:
	SectionButton(QWidget *parent, const QString& _text, const std::string _data, const QPixmap& _icon);
	~SectionButton();

signals:
	void sectionClicked(std::string);
	void sectionRightClick(std::string);
private:
	std::string data_;
protected:
	virtual void contextMenuEvent(QContextMenuEvent*);
private slots:
	void reemit_clicked();
};

#endif // SECTION_BUTTON_H
