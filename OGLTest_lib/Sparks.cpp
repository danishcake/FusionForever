#include "StdAfx.h"
#include "Sparks.h"

#define SPARKS_LIFETIME 0.1f
#define SPARK_SIZE 2.0f

bool Sparks::initialised_ = false;
int Sparks::fill_dl_ = 0;
int Sparks::fill_verts_index_ = 0;

Sparks::Sparks(void)
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255, static_cast<unsigned char>(Random::RandomRange(200,255)), static_cast<unsigned char>(Random::RandomRange(200,255))));

	lifetime_ = SPARKS_LIFETIME;
	ltv_transform_ = Matrix4f::createScale(0);
}

Sparks::~Sparks(void)
{
}

void Sparks::Tick(float _timespan, Matrix4f _transform)
{
	Matrix4f scaleM  = Matrix4f::createScale(SPARK_SIZE* (sqrtf((SPARKS_LIFETIME - lifetime_) / SPARKS_LIFETIME)));
	Decoration::Tick(_timespan, _transform);
	_transform = ltv_transform_ * scaleM;
	ltv_transform_ = _transform;
}

void Sparks::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0.5f,0)); //0
	temp_outline->push_back(Vector3f(1,4,0)); //1
	temp_outline->push_back(Vector3f(2.5f,6,0)); //2
	temp_outline->push_back(Vector3f(4.5f,8,0)); //3
	temp_outline->push_back(Vector3f(5,7,0)); //4
	temp_outline->push_back(Vector3f(3.5,4,0)); //5
	temp_outline->push_back(Vector3f(1.5f,2,0)); //6

	temp_outline->push_back(Vector3f(-1,4,0)); //7
	temp_outline->push_back(Vector3f(-2.5f,6,0)); //8
	temp_outline->push_back(Vector3f(-4.5f,7,0)); //9
	temp_outline->push_back(Vector3f(-5,7,0)); //10
	temp_outline->push_back(Vector3f(-3.5,4,0)); //11
	temp_outline->push_back(Vector3f(-1.5f,2,0)); //12

	//Right hand
	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[1]);
	
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[6]);
	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[3]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[5]);
	temp_fill->push_back((*temp_outline)[4]);
	temp_fill->push_back((*temp_outline)[3]);


	//Left hand
	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[7]);
	temp_fill->push_back((*temp_outline)[12]);
	
	temp_fill->push_back((*temp_outline)[7]);
	temp_fill->push_back((*temp_outline)[8]);
	temp_fill->push_back((*temp_outline)[12]);

	temp_fill->push_back((*temp_outline)[8]);
	temp_fill->push_back((*temp_outline)[11]);
	temp_fill->push_back((*temp_outline)[12]);	

	temp_fill->push_back((*temp_outline)[8]);
	temp_fill->push_back((*temp_outline)[9]);
	temp_fill->push_back((*temp_outline)[11]);

	temp_fill->push_back((*temp_outline)[9]);
	temp_fill->push_back((*temp_outline)[10]);
	temp_fill->push_back((*temp_outline)[11]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

