#pragma once
#include "Decoration.h"
#include "Billboard.h"


namespace BillboardDecoType
{
	enum Enum
	{
		FadesOut,
		Explodes,
		Warp
	};
}

class BillboardDeco :
	public Decoration
{
protected:
	virtual void InitialiseGraphics();
	Billboard* billboard_;
	BillboardDecoType::Enum billboard_type_;
	Vector2f initial_size_;
	float total_lifetime_;
	float scale_;
	float transparancy_;
public:
	BillboardDeco(std::string _filename, float _lifetime, BillboardDecoType::Enum _billboard_type);
	virtual ~BillboardDeco(void);

	virtual void Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn);
	virtual void DrawSelf();

	static BillboardDeco* RandomExplosion();
};
