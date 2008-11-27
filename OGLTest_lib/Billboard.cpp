#include "StdAfx.h"
#include "Billboard.h"
#include "TextureLoader.h"
#include "Camera.h"

Billboard::Billboard(std::string _texture, BillboardType::Enum _billboard_space)
{
	billboard_space_ = _billboard_space;
	texture_id_ = TextureLoader::Instance().LoadTexture(_texture);
	Vector2<int> size_i = TextureLoader::Instance().GetTextureSize(texture_id_);
	size_.x = size_i.x;
	size_.y = size_i.y;
	centre_ = Vector2f(0.5f, 0.5f);
	color_ = GLColor(255,255,255);
}

Billboard::Billboard(unsigned int _texture_id, BillboardType::Enum _billboard_space)
{
	billboard_space_ = _billboard_space;
	TextureLoader::Instance().LoadTexture(_texture_id);
	texture_id_ = _texture_id;
	Vector2<int> size_i = TextureLoader::Instance().GetTextureSize(texture_id_);
	size_.x = size_i.x;
	size_.y = size_i.y;
	centre_ = Vector2f(0.5f, 0.5f);
	color_ = GLColor(255,255,255);
}

Billboard::~Billboard()
{
	TextureLoader::Instance().UnloadTexture(texture_id_);
}

void Billboard::Draw()
{
	glColor4ub(color_.r, color_.g, color_.b, color_.a);

	glPushMatrix();
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D , texture_id_);
	
	if(billboard_space_ == BillboardType::WorldSpace)
	{
		glTranslatef(position_.x - size_.x * centre_.x, position_.y - size_.y * centre_.y, 0);
		glBegin(GL_TRIANGLES);
		
		glTexCoord2i( 0, 1);
		glVertex3f(0,0, 0);
		glTexCoord2i( 1, 1 );
		glVertex3f(size_.x, 0, 0);
		glTexCoord2i( 1, 0 );
		glVertex3f(size_.x, size_.y, 0);

		glTexCoord2i( 1, 0 );
		glVertex3f(size_.x, size_.y, 0);
		glTexCoord2i( 0, 0 );
		glVertex3f(0, size_.y, 0);
		glTexCoord2i( 0, 1 );
		glVertex3f(0, 0, 0);
		
		glEnd();
	} else
	{
		Vector3f world_transform = Camera::Instance().ScreenToWorld(Vector3f(position_.x - size_.x * centre_.x, 
																			 position_.y - size_.y * centre_.y, 0));

		//Vector3f world_transform = Camera::Instance().ScreenToWorld(Vector3f(position_.x, -position_.y, 0));
		glTranslatef(world_transform.x, world_transform.y, 0);
		float scale = Camera::Instance().GetWidth() / Camera::Instance().GetWindowWidth();
		glScalef(scale, scale, scale);

		glBegin(GL_TRIANGLES);
		
		glTexCoord2i( 0, 0);
		glVertex3f(0,0, 0);
		glTexCoord2i( 1, 0 );
		glVertex3f(size_.x, 0, 0);
		glTexCoord2i( 1, 1 );
		glVertex3f(size_.x, -size_.y, 0);

		glTexCoord2i( 1, 1 );
		glVertex3f(size_.x, -size_.y, 0);
		glTexCoord2i( 0, 1 );
		glVertex3f(0, -size_.y, 0);
		glTexCoord2i( 0, 0 );
		glVertex3f(0, 0, 0);

		glEnd();
	}



	

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	glPopMatrix();
}
