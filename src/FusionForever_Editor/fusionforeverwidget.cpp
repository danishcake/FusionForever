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
	selection_ = NULL;
	drag_mode_ = EditorDragMode::NotDragging;
	drag_occurred_ = false;
	setMouseTracking(true);
}

FusionForeverWidget::~FusionForeverWidget()
{

}

void FusionForeverWidget::mouseReleaseEvent(QMouseEvent* me)
{
	Vector3f click_loc(me->x(), me->y(), 0);
	Vector3f world_loc = Camera::Instance().ScreenToWorld(click_loc);
	
	std::vector<Section_ptr> clicked_sections;
	core_->CheckCollisions(world_loc, clicked_sections);
	
	if(me->button() == Qt::LeftButton && !drag_occurred_)
	{
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
	} else if(me->button() == Qt::RightButton && !drag_occurred_)
	{
		emit selectionChanged(selection_, clicked_sections);
		emit rightClick();
	}

	drag_mode_ = EditorDragMode::NotDragging;
	drag_occurred_ = false;
}

void FusionForeverWidget::mousePressEvent(QMouseEvent* me)
{
	Vector3f click_loc(me->x(), me->y(), 0);
	Vector3f world_loc = Camera::Instance().ScreenToWorld(click_loc);
	if(me->button() == Qt::LeftButton && selection_)
	{
		Section_ptr clicked_section = NULL;
		selection_->CheckCollisions(world_loc, clicked_section);
		if(clicked_section == selection_)
		{
			drag_mode_ = EditorDragMode::MoveDrag;
			drag_occurred_ = false;
		} else if(clicked_section == NULL)
		{
			drag_mode_ = EditorDragMode::RotateDrag;
			drag_occurred_ = false;
		}
		accumulated_snap = Vector3f(0, 0, 0);
	} else if(me->button() == Qt::MidButton)
	{
		drag_mode_ = EditorDragMode::ScrollDrag;
		drag_occurred_ = false;
	}
}
void FusionForeverWidget::mouseDoubleClickEvent(QMouseEvent *){}
void FusionForeverWidget::mouseMoveEvent(QMouseEvent* me)
{
	Vector2f mouse_position = Vector2f(me->x(), me->y());
	Vector2f mouse_move = mouse_position - ltv_mouse_position_;
	
	Vector3f world_move = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(mouse_move.x, mouse_move.y, 0.0f));
	Vector3f world_position = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(mouse_position.x, mouse_position.y, 0.0f));

	if(drag_mode_ != EditorDragMode::NotDragging)
	{
		drag_occurred_ = true;
	}

	if(drag_mode_ == EditorDragMode::ScrollDrag)
	{
		Camera::Instance().SetCentre(Camera::Instance().GetCentreX() - world_move.x, Camera::Instance().GetCentreY() - world_move.y, CameraLevel::Human);
	} else if(drag_mode_ == EditorDragMode::MoveDrag && selection_ && selection_ != core_)
	{
		accumulated_snap += world_move;
		if(fabsf(accumulated_snap.x) >= 2.5f)
		{
			//Rotate the move vector into the parents space
			Vector3f delta = Vector3f(accumulated_snap.x, 0 , 0);
			delta.rotate(0, 0, selection_->GetParent()->GetGlobalAngle());
			Vector3f snap_to = (selection_->GetPosition() + delta).snap(2.5f);
			selection_->SetPosition(snap_to);
			accumulated_snap.x = 0;
		}
		if(fabsf(accumulated_snap.y) >= 2.5f)
		{
			//Rotate the move vector into the parents space
			Vector3f delta = Vector3f(0, accumulated_snap.y , 0);
			delta.rotate(0, 0, selection_->GetParent()->GetGlobalAngle());
			Vector3f snap_to = (selection_->GetPosition() + delta).snap(2.5f);
			selection_->SetPosition(snap_to);
			accumulated_snap.y = 0;
		}
	} else if(drag_mode_ == EditorDragMode::RotateDrag && selection_ && selection_ != core_)
	{
		accumulated_snap += world_move;
		//Rotate to face the mouse
		if(fabs(accumulated_snap.x) >= 15)
		{
			float angle = floorf((selection_->GetAngle() + accumulated_snap.x) / 15 + 0.5f) * 15.0f;
			selection_->SetAngle(angle);
			accumulated_snap.x = 0;
		}
	}
	ltv_mouse_position_ = Vector2f(me->x(), me->y());
}

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
	core_->SetAngle(0);
	core_->SetPosition(Vector3f(0,0,0));
	core_->SetVelocity(Vector3f(0,0,0));
	core_->SetSpin(0);
	//TODO delete spawned stuff
	Camera::Instance().TickCamera(timespan);


	updateGL();
}

void FusionForeverWidget::AddSection(std::string _item_name)
{
	if(selection_)
	{
		Section_ptr nSection = SectionTypes::GetSection(_item_name);
		if(nSection)
		{
			selection_->AddChild(nSection);
			SetSelection(nSection);
		} else
		{
			Logger::ErrorOut() << "Unable to create section of type '" << _item_name << "'\n";
		}

	}
}

void FusionForeverWidget::DeleteSelection()
{
	//std::vector<Section*> detached = selection_->GetParent()->DetachChildren();

	if(selection_ && core_ != selection_)
	{
		selection_->TakeDamage(selection_->GetMaxHealth() + 1, -1);	
	}
}

void FusionForeverWidget::SelectSection(int _section_id)
{
	Section_ptr select = core_->GetSectionByID(_section_id);
	SetSelection(select);
}

void FusionForeverWidget::Save(std::string _filename)
{
	core_->SaveToXML(_filename);
}

void FusionForeverWidget::New()
{
	delete core_;
	core_ = XMLCore::CreateXMLCore("SquareCore");
}

void FusionForeverWidget::Open(std::string _filename)
{
	delete core_;
	core_ = Core::CreateCore(_filename);
	if(!core_)
	{
		core_ = XMLCore::CreateXMLCore("SquareCore");
	}
}

void FusionForeverWidget::initializeGL()
{
	SectionTypes::RegisterSections();
	emit initialisedSections();
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
	Vector3f scaled_line = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(5,0,0));
	core_->DrawEditorSupport(scaled_line.x, selection_);
	glFlush();
}

void FusionForeverWidget::resizeGL(int width, int height)
{
	Camera::Instance().SetAspectRatio(width, height);
	glViewport(0, 0, width, height);	
}