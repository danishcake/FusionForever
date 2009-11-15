#include "SectionButton.h"

SectionButton::SectionButton(QWidget *parent, const QString& _text, const std::string _data, const QPixmap& _icon)
	: QPushButton(_text, parent), data_(_data)
{
	QSize s(64,64);
	setIconSize(s);
	QIcon ic(_icon);
	setIcon(ic);
	QObject::connect(this, SIGNAL(clicked()), this, SLOT(reemit_clicked()));
}

SectionButton::~SectionButton()
{

}

void SectionButton::reemit_clicked()
{
	emit sectionClicked(data_);
}

void SectionButton::contextMenuEvent(QContextMenuEvent*)
{
	emit sectionRightClick(data_);
}