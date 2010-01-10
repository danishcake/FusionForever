#ifndef QSDLINPUTWIDGET_H
#define QSDLINPUTWIDGET_H

#include <QWidget>
#include <Binding.h>
class QLabel;
class QPushButton;
//class QComboBox;

class QSDLInputWidget : public QWidget
{
	Q_OBJECT

public:
	QSDLInputWidget(QWidget *parent/*, QString action*/);
	~QSDLInputWidget();

	void SetDetails(QString _action_str, QString _description, Action::Enum _action);
	QString GetAction();

	void SetBinding(InputConfig _input_config);
	void ClearBinding();
	InputConfig& GetBinding();

private:
	QLabel* mActionName;
	QLabel* mActionDescription;
	QPushButton* mConfigure;

	InputConfig mInputConfig;

	void UpdateInputConfig();
private slots:
	void Configure();
};

#endif // QSDLINPUTWIDGET_H
