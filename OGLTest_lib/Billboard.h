#pragma once
#include <string>
#include "vmath.h"
#include "GLColor.h"

namespace BillboardType
{
	enum Enum
	{
		ScreenSpace,
		WorldSpace
	};
}

class Billboard
{
private:
	unsigned int texture_id_;
	Vector3f position_;
	Vector2f size_;
	Vector2f centre_;
	BillboardType::Enum billboard_space_;
	GLColor color_;

public:
	explicit Billboard(std::string _texture, BillboardType::Enum _billboard_space);
	explicit Billboard(unsigned int _texture_id, BillboardType::Enum _billboard_space);

	Vector3f GetPosition() {return position_;}
	void SetPosition(Vector3f _position){position_ = _position;}

	Vector2f GetSize() {return size_;}
	void SetSize(Vector2f _size){size_= _size;}

	Vector2f GetOffset() {return centre_;}
	void SetOffset(const Vector2f _centre){centre_ = _centre;}

	void SetColor(GLColor _color){color_ = _color;}

	void Draw();

	~Billboard(void);
};
