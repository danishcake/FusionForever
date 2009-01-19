#include "StdAfx.h"
#include "TextureLoader.h"
#include <SDL.h>
#include <SDL_opengl.h>

TextureLoader::TextureLoader(void)
{
}

TextureLoader::~TextureLoader(void)
{
	for(std::map<std::string, boost::shared_ptr<TextureData>>::iterator it = textures_by_name_.begin();
		it != textures_by_name_.end(); ++it)
	{
		glDeleteTextures(1, &it->second->texture_id);
	}
}

void TextureLoader::PreloadTexture(std::string _filename)
{
	LoadTexture(_filename);
	textures_by_name_[_filename]->usages--;
}

GLuint TextureLoader::LoadTexture(std::string _filename)
{
	if(textures_by_name_.find(_filename) != textures_by_name_.end())
	{
		textures_by_name_[_filename]->usages++;
		return textures_by_name_[_filename]->texture_id;
	}
	/* Lifted wholesale from 
	http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL
	*/
	GLuint texture;
	SDL_Surface* surface;
	GLenum texture_format;
	GLint nOfColors;
	if(surface = SDL_LoadBMP((std::string("textures/") + _filename ).c_str()))
	{
		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
				if (surface->format->Rmask == 0x000000ff)
						texture_format = GL_RGBA;
				else
						texture_format = GL_BGRA;
		} else if (nOfColors == 3)     // no alpha channel
		{
				if (surface->format->Rmask == 0x000000ff)
						texture_format = GL_RGB;
				else
						texture_format = GL_BGR;
		} else
		{
			Logger::ErrorOut() << "Not a truecolor image\n";
			return 0;
		}
			
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
	 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
	 
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	 
		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
						  texture_format, GL_UNSIGNED_BYTE, surface->pixels );


		boost::shared_ptr<TextureData> td = boost::shared_ptr<TextureData>(new TextureData(texture));
		td->texture_size.x = surface->w;
		td->texture_size.y = surface->h;
		textures_by_name_[_filename] = td;
		textures_by_id_[texture] = td;
		SDL_FreeSurface(surface);

		return texture;
	} else
	{
		Logger::ErrorOut() << "Unable to open texture " << _filename << "\n";
	}
	return 0;
}

void TextureLoader::UnloadTexture(std::string _filename)
{
	if(textures_by_name_.find(_filename) != textures_by_name_.end())
	{
		textures_by_name_[_filename]->usages--;
	/*	if(textures_by_name_[_filename].usages <= 0)
		{
			glDeleteTextures(1, &textures_by_name_[_filename].texture_id);
		}*/
	} else
	{
		Logger::ErrorOut() << "Texture not loaded - why are you trying to unload it? :" << _filename << "\n";
	}
	//textures_by_name_.erase(textures_by_name_.;
}

void TextureLoader::LoadTexture(unsigned int _texture_id)
{
	if(textures_by_id_.find(_texture_id) != textures_by_id_.end())
	{
		textures_by_id_[_texture_id]->usages++;
	} else
	{
		Logger::ErrorOut() << "Texture not loaded :" << boost::lexical_cast<std::string, unsigned int>(_texture_id) << "\n";
	}
}

void TextureLoader::UnloadTexture(unsigned int _texture_id)
{
	if(textures_by_id_.find(_texture_id) != textures_by_id_.end())
	{
		textures_by_id_[_texture_id]->usages--;
	} else
	{
		Logger::ErrorOut() << "Texture not loaded :" << boost::lexical_cast<std::string, unsigned int>(_texture_id) << "\n";
	}
}

Vector2<int> TextureLoader::GetTextureSize(unsigned int _texture_id)
{
	if(textures_by_id_.find(_texture_id) != textures_by_id_.end())
	{
		return Vector2<int>(textures_by_id_[_texture_id]->texture_size.x, textures_by_id_[_texture_id]->texture_size.y);
	} else
	{
		Logger::ErrorOut() << "Texture not loaded :" << boost::lexical_cast<std::string, unsigned int>(_texture_id) << "\n";
		return Vector2<int>();
	}
}

TextureLoader& TextureLoader::Instance()
{
	static TextureLoader loader;
	return loader;
}