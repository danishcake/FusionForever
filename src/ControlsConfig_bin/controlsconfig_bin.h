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
protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::ControlsConfig_binClass ui;
	int old_player_id;
	bool menu_exit;

private slots:
	void player_id_changed(int player_id);
	void exit_no_save();
	void exit_and_save();
	void reset_to_default();
};

#endif // CONTROLSCONFIG_BIN_H
