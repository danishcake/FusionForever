#pragma once
#include "core.h"

class SquareCore :
	public Core
{
protected:
	virtual void InitialiseGraphics();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

public:
	SquareCore(BaseAI*);
	virtual ~SquareCore(void);
	virtual void ToXML(TiXmlElement* _node);
};
