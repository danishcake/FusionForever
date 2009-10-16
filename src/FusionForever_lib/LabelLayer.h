#pragma once
#include "Subscriber.h"

class Section;

class LabelLayer
{
private:
	Section* labelled_;
public:
	LabelLayer(Section* _root);
	virtual ~LabelLayer(void);

	virtual void EndSubscription(Subscriber*);
};
