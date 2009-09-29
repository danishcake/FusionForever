#ifndef FUSIONFOREVERWIDGET_H
#define FUSIONFOREVERWIDGET_H

#include <QGLWidget>

class Section;

class FusionForeverWidget : public QGLWidget
{
	Q_OBJECT

public:
	FusionForeverWidget(QWidget *parent);
	~FusionForeverWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

private:
	Section* core_;
	
};

#endif // FUSIONFOREVERWIDGET_H
