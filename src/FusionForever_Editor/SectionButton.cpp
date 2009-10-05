#include "SectionButton.h"

SectionButton::SectionButton(QWidget *parent, const QString& _text, const std::string _data)
	: QPushButton(_text, parent), data_(_data)
{
	QObject::connect(this, SIGNAL(clicked()), this, SLOT(reemit_clicked()));
}

SectionButton::~SectionButton()
{

}

void SectionButton::reemit_clicked()
{
	emit sectionClicked(data_);
}