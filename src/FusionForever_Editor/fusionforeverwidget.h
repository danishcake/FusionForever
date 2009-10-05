#ifndef FUSIONFOREVERWIDGET_H
#define FUSIONFOREVERWIDGET_H

#include <QGLWidget>
#include <vector>

class Section;

class FusionForeverWidget : public QGLWidget
{
	Q_OBJECT

public:
	FusionForeverWidget(QWidget *parent);
	~FusionForeverWidget();

public slots:
	void Tick();
	void AddSection(std::string _item_name);
	void DeleteSelection();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void wheelEvent(QWheelEvent *);

private:
	void SetSelection(Section*);
	Section* core_;
	Section* selection_;
	std::vector<Section*> sections_under_mouse_;
	size_t section_under_mouse_index_;
	
};

#endif // FUSIONFOREVERWIDGET_H
