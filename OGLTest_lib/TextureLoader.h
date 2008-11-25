#pragma once
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

struct TextureData
{
	unsigned int texture_id;
	int usages;
	std::string texture_name;
	Vector2<int> texture_size;
	
	TextureData()
	{
		usages = 0;
		texture_id = 0;
	}

	TextureData(unsigned int _texture_id)
	{
		usages = 1;
		texture_id = _texture_id;
	}
};

class TextureLoader
{
public:
	static TextureLoader& Instance();
	~TextureLoader(void);

	unsigned int LoadTexture(std::string _filename);
	void UnloadTexture(std::string _filename);
	void LoadTexture(unsigned int _texture_id);
	void UnloadTexture(unsigned int _texture_id);
	Vector2<int> GetTextureSize(unsigned int _texture_id);

private:
	TextureLoader(void);
	std::map<std::string, boost::shared_ptr<TextureData>> textures_by_name_;
	std::map<unsigned int, boost::shared_ptr<TextureData>> textures_by_id_;
};
