#ifndef CONTROLSCONFIG_BIN_H
#define CONTROLSCONFIG_BIN_H

#include <QtGui/QMainWindow>
#include "ui_controlsconfig_bin.h"

class ControlsConfig_bin : public QMainWindow
{
	Q_OBJECT

public:
	ControlsConfig_bin(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ControlsConfig_bin();

private:
	Ui::ControlsConfig_binClass ui;
};

#endif // CONTROLSCONFIG_BIN_H
