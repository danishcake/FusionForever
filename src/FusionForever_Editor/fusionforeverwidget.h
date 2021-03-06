#ifndef FUSIONFOREVERWIDGET_H
#define FUSIONFOREVERWIDGET_H

#include <QGLWidget>
#include <vector>
#include <vmath.h>

class Section;

namespace EditorDragMode
{
	enum Enum
	{
		NotDragging, RotateDrag, MoveDrag, ScrollDrag
	};
}

class FusionForeverWidget : public QGLWidget
{
	Q_OBJECT

public:
	FusionForeverWidget(QWidget *parent);
	~FusionForeverWidget();

	void Save(std::string /*_filename*/);
	void New();
	void Open(std::string /*_filename*/);

public slots:
	void Tick();
	/* Adds as child */
	void AddSection(std::string _item_name);
	/* Inserts before selection */
	void InsertSection(std::string _item_name);
	/* Replaces the current selection */
	void ReplaceSection(std::string _item_name);
	/* Replaces the current core */
	void ReplaceCore(std::string _core_name);

	void RemoveSelection();
	void DeleteSelection();
	void SelectSection(int /*_section_id*/);
	void SetGridSize(float /*_snap*/);
	void IncreaseGridSize();
	void DecreaseGridSize();
	void CutSection();
	void PasteSection();

signals:
	void selectionChanged(Section* /*_current_selection*/, std::vector<Section*> /*_all_possible_selections*/);
	void selectionChanged(Section* /*_new_selection*/);
	void rightClick();
	void initialisedSections(std::vector<std::pair<std::string, QPixmap*> > /*_section_icons*/, std::vector<std::pair<std::string, QPixmap*> > /*_core_icons*/);

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
	void DrawSection(Section* section, std::string name, std::vector<std::pair<std::string, QPixmap*> >& icons);
	Section* core_;
	Section* selection_;
	Section* cut_section_;
	std::vector<Section*> sections_under_mouse_;
	size_t section_under_mouse_index_;

	EditorDragMode::Enum drag_mode_;
	bool drag_occurred_;
	Vector2f ltv_mouse_position_;
	Vector3f accumulated_snap;
	bool icon_render_mode;
	float grid_snap_;
	
};

#endif // FUSIONFOREVERWIDGET_H
