#include "fusionforeverwidget.h"
#include <XMLCore.h>

FusionForeverWidget::FusionForeverWidget(QWidget *parent)
	: QGLWidget(parent)
{
	core_ = XMLCore::CreateXMLCore("SquareCore.xmlCore");
}

FusionForeverWidget::~FusionForeverWidget()
{

}


void FusionForeverWidget::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void FusionForeverWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FusionForeverWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width /(float)height, 0.1f, 100000);
	glMatrixMode(GL_MODELVIEW);
	
}