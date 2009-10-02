#include "fusionforeverwidget.h"
#include <XMLCore.h>
#include <XMLSection.h>
#include <SectionTypes.h>
#include <GridCollisionManager.h>
#include <QtGui/qevent.h>
#include <Logger.h>
#include <algorithm>

FusionForeverWidget::FusionForeverWidget(QWidget *parent)
	: QGLWidget(parent)
{
	section_under_mouse_index_ = 0;	
}

FusionForeverWidget::~FusionForeverWidget()
{

}

void FusionForeverWidget::mouseReleaseEvent(QMouseEvent* me)
{
	me->accept();
	Vector3f click_loc(me->x(), me->y(), 0);
	Vector3f world_loc = Camera::Instance().ScreenToWorld(click_loc);

	if(me->button() == Qt::LeftButton)
	{
		std::vector<Section_ptr> clicked_sections;
		core_->CheckCollisions(world_loc, clicked_sections);

		if(clicked_sections.size() > 0)
		{
			if(clicked_sections == sections_under_mouse_)
			{
				section_under_mouse_index_++;
				section_under_mouse_index_ %= sections_under_mouse_.size();
				SetSelection(sections_under_mouse_[section_under_mouse_index_]);
			} else 
			{
				sections_under_mouse_ = clicked_sections;
				section_under_mouse_index_ = 0;
				SetSelection(sections_under_mouse_[section_under_mouse_index_]);
			}
		}
	}
}

void FusionForeverWidget::mousePressEvent(QMouseEvent *){}
void FusionForeverWidget::mouseDoubleClickEvent(QMouseEvent *){}
void FusionForeverWidget::mouseMoveEvent(QMouseEvent *){}
void FusionForeverWidget::wheelEvent(QWheelEvent* we)
{
	if(we->delta() > 0)
		Camera::Instance().ZoomIn();
	else if(we->delta() < 0)
		Camera::Instance().ZoomOut();
}

void FusionForeverWidget::SetSelection(Section* _selection)
{
	selection_ = _selection;
}


void FusionForeverWidget::Tick()
{
	float timespan = 0.025f;
	std::vector<Decoration_ptr> decoration_spawn;
	std::vector<Core_ptr> enemies;
	std::vector<Core_ptr> friends;
	std::vector<Projectile_ptr> projectile_spawn;
	GridCollisionManager collision_manager;

	Matrix4f identity;
	
	core_->Tick(timespan, projectile_spawn, decoration_spawn, identity, enemies, &collision_manager);
	//TODO delete spawned stuff
	Camera::Instance().TickCamera(timespan);


	updateGL();
}


void FusionForeverWidget::initializeGL()
{
	SectionTypes::RegisterSections();
	core_ = XMLCore::CreateXMLCore("SquareCore");
	Section_ptr section = SectionTypes::GetSection("RigidArm");
	section->SetPosition(Vector3f(7, 4, 0));
	core_->AddChild(section);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FusionForeverWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Camera::Instance().SetupCamera();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	core_->DrawSelf();
	core_->DrawEditorSupport(2.5f, selection_);
	glFlush();
}

void FusionForeverWidget::resizeGL(int width, int height)
{
	Camera::Instance().SetAspectRatio(width, height);
	glViewport(0, 0, width, height);	
}