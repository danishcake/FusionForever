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
	cut_section_ = NULL;
	drag_mode_ = EditorDragMode::NotDragging;
	drag_occurred_ = false;
	icon_render_mode = true;
	setMouseTracking(true);
	grid_snap_ = 2.5f;
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
		Vector3f rot_move = world_move;
		rot_move.rotate(0, 0, selection_->GetParent()->GetGlobalAngle());
		accumulated_snap += rot_move;

		if(fabsf(accumulated_snap.x) >= grid_snap_)
		{
			Vector3f delta = Vector3f(accumulated_snap.x, 0 , 0);
			Vector3f snap_to = (selection_->GetPosition() + delta).snap(grid_snap_);
			selection_->SetPosition(snap_to);
			accumulated_snap.x = 0;
		}
		if(fabsf(accumulated_snap.y) >= grid_snap_)
		{
			Vector3f delta = Vector3f(0, accumulated_snap.y , 0);
			Vector3f snap_to = (selection_->GetPosition() + delta).snap(grid_snap_);
			selection_->SetPosition(snap_to);
			accumulated_snap.y = 0;
		}
	} else if(drag_mode_ == EditorDragMode::RotateDrag && selection_ && selection_ != core_)
	{
		accumulated_snap.x += mouse_move.x;
		//Rotate to face the mouse
		if(fabs(accumulated_snap.x) >= 30)
		{
			float angle = floorf((selection_->GetAngle() + (accumulated_snap.x < 0 ? -15 : 15)) / 15 + 0.5f) * 15.0f;
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
	if(selection_ != _selection)
		emit selectionChanged(_selection);
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

void FusionForeverWidget::InsertSection(std::string _item_name)
{
	if(selection_ && core_ != selection_)
	{
		Section_ptr nSection = SectionTypes::GetSection(_item_name);
		if(nSection)
		{
			//Detach children, remove selection from them. Add new section and attach selection to it.
			Section_ptr parent = selection_->GetParent();
			std::vector<Section_ptr> children = parent->DetachChildren();
			children.erase(std::remove(children.begin(), children.end(), selection_), children.end());
			children.push_back(nSection);
			nSection->AddChild(selection_);
			parent->AttachChildren(children);
			SetSelection(nSection);
		} else
		{
			Logger::ErrorOut() << "Unable to create section of type '" << _item_name << "'\n";
		}
	}
}

void FusionForeverWidget::ReplaceSection(std::string _item_name)
{
	if(selection_ && selection_ != core_)
	{
		Section_ptr nSection = SectionTypes::GetSection(_item_name);
		if(nSection)
		{
			float angle = selection_->GetAngle();
			Vector3f position = selection_->GetPosition();
			//Detach children from parent, remove selection from them
			//Detach children from selection.

			Section_ptr parent = selection_->GetParent();
			std::vector<Section_ptr> children = parent->DetachChildren();
			std::vector<Section_ptr> sel_children = selection_->DetachChildren();
			children.erase(std::remove(children.begin(), children.end(), selection_), children.end());
			parent->AttachChildren(children);
			nSection->AttachChildren(sel_children);
			nSection->SetAngle(angle);
			nSection->SetPosition(position);
			parent->AddChild(nSection);
			delete selection_;
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

void FusionForeverWidget::RemoveSelection()
{
	if(selection_ && core_ != selection_)
	{
		Section_ptr parent = selection_->GetParent();
		std::vector<Section_ptr> children = parent->DetachChildren();
		std::vector<Section_ptr> selection_children = selection_->DetachChildren();
		children.erase(std::remove(children.begin(), children.end(), selection_), children.end());
		children.insert(children.end(), selection_children.begin(), selection_children.end());
		parent->AttachChildren(children);
		delete selection_;
		SetSelection(parent);
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
	std::vector<Decoration_ptr> deathspawn;
	core_->GetDeathSpawn(deathspawn);
	for(std::vector<Decoration_ptr>::iterator it = deathspawn.begin(); it != deathspawn.end(); ++it)
	{
		delete *it;
	}
	core_ = XMLCore::CreateXMLCore("SquareCore");
	SetSelection(core_);
}

void FusionForeverWidget::Open(std::string _filename)
{
	std::vector<Decoration_ptr> deathspawn;
	core_->GetDeathSpawn(deathspawn);
	for(std::vector<Decoration_ptr>::iterator it = deathspawn.begin(); it != deathspawn.end(); ++it)
	{
		delete *it;
	}
	core_ = Core::CreateCore(_filename);
	if(!core_)
	{
		core_ = XMLCore::CreateXMLCore("SquareCore");
	}
	SetSelection(core_);
}


void FusionForeverWidget::CutSection()
{
	if(cut_section_)
	{
		delete cut_section_;
		cut_section_ = NULL;
	}
	if(selection_ && core_ != selection_)
	{
		Section_ptr parent = selection_->GetParent();
		std::vector<Section_ptr> children = parent->DetachChildren();
		children.erase(std::remove(children.begin(), children.end(), selection_), children.end());
		parent->AttachChildren(children);
		cut_section_ = selection_;
		SetSelection(parent);
	}
}
void FusionForeverWidget::PasteSection()
{
	if(selection_ && cut_section_)
	{
		selection_->AddChild(cut_section_);
		SetSelection(cut_section_);
		cut_section_ = NULL;
	}
}
void FusionForeverWidget::SetGridSize(float _snap)
{
	grid_snap_ = _snap;
}

void FusionForeverWidget::IncreaseGridSize()
{
	grid_snap_ *= sqrt(2.0);
}

void FusionForeverWidget::DecreaseGridSize()
{
	grid_snap_ *= sqrt(0.5);
}

void FusionForeverWidget::initializeGL()
{
	SectionTypes::RegisterSections();
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FusionForeverWidget::paintGL()
{
	if(icon_render_mode) 
	{ //On first pass render all sections to an icon
		glClearColor(1.0f,1.0f,1.0f,0.0f);
		std::vector<std::string> section_types = SectionTypes::GetNames();
		std::vector<std::pair<std::string, QPixmap*> > section_icons;

		for(std::vector<std::string>::iterator it = section_types.begin(); it != section_types.end(); ++it)
		{
			Section_ptr section = SectionTypes::GetSection(*it);
			if(section)
			{
				float radius = section->GetRadius();
				
				Camera::Instance().SetSmallestDimension(40);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Camera::Instance().SetupCamera();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				//Draw Section
				section->DeathTick();
				section->SetOutlineColor(GLColor(0,0,0));
				section->DrawSelf();
				
				glFlush();


				QImage icon = grabFrameBuffer();
				if(height() > width())
				{
					int top = (height() - width()) / 2;
					QImage icon2 = icon.copy(0, top, width(), width());
					QPixmap pm = QPixmap::fromImage(icon2);
					section_icons.push_back(std::pair<std::string, QPixmap*>(*it, new QPixmap(pm.pixmapData())));
				} else
				{
					int left = (width() - height()) / 2;
					QImage icon2 = icon.copy(left, 0, height(), height());
					QPixmap pm = QPixmap::fromImage(icon2);
					section_icons.push_back(std::pair<std::string, QPixmap*>(*it, new QPixmap(pm.pixmapData())));
				}

				
				delete section;
			}
		}

		core_ = XMLCore::CreateXMLCore("SquareCore");
		SetSelection(core_);
		icon_render_mode = false;

		emit initialisedSections(section_icons);
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Camera::Instance().SetupCamera();
	glMatrixMode(GL_MODELVIEW);

	if(selection_)
	{
		glColor3ub(50, 50, 50);
		Matrix4f transform;
		if(!selection_->IsCore())
			transform = selection_->GetParent()->GetTransform();
		glLoadMatrixf(&transform.data[0]);
		//Draw grid
		glBegin(GL_LINES);
		float worst_width = sqrt(Camera::Instance().GetWidth() * Camera::Instance().GetWidth() +
								 Camera::Instance().GetHeight() * Camera::Instance().GetHeight());
		
		//Calculate grid size to ensure 5px minimum size
		float drawn_grid_size = grid_snap_;
		const float min_screen_size = 5;
		while(drawn_grid_size < min_screen_size * Camera::Instance().GetWidth() / Camera::Instance().GetWindowWidth())
		{
			drawn_grid_size *= 2.0f;
		}	

		for(float x = 0; x < worst_width; x+=drawn_grid_size)
		{
			glVertex3f(x, -10000, 0);
			glVertex3f(x, 10000, 0);
			glVertex3f(-x, -10000, 0);
			glVertex3f(-x, 10000, 0);
			
			glVertex3f(-10000, x, 0);
			glVertex3f(10000, x, 0);
			glVertex3f(-10000, -x, 0);
			glVertex3f(10000, -x, 0);
		}
		glEnd();
	}

	glLoadIdentity();
	core_->DrawSelf();

	//Calculate world space length so that constant 5px screenspace is achieved.
	Vector3f scaled_line = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(5,0,0));
	core_->DrawEditorSupport(scaled_line.x, selection_);
	glFlush();

}

void FusionForeverWidget::resizeGL(int width, int height)
{
	Camera::Instance().SetAspectRatio(width, height);
	glViewport(0, 0, width, height);
}