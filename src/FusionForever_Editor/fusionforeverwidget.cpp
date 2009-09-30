#include "fusionforeverwidget.h"
#include <XMLCore.h>
#include <XMLSection.h>
#include <SectionTypes.h>
#include <GridCollisionManager.h>

FusionForeverWidget::FusionForeverWidget(QWidget *parent)
	: QGLWidget(parent)
{
	
}

FusionForeverWidget::~FusionForeverWidget()
{

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
	glFlush();
}

void FusionForeverWidget::resizeGL(int width, int height)
{
	Camera::Instance().SetAspectRatio(width, height);
	glViewport(0, 0, width, height);
	
	
}