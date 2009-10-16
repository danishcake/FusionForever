#include "StdAfx.h"
#include "LabelLayer.h"
#include "Section.h"
#include <vector>
#include "SortFunctors.h"

void buildSectionList(std::vector<Section*>& _sections, Section* _section)
{
	std::vector<Section*> ss = _section->GetSubsections();
	for(std::vector<Section*>::iterator it = ss.begin(); it != ss.end(); ++it)
	{
	   buildSectionList(_sections, *it);
	}
	_sections.insert(_sections.end(), ss.begin(), ss.end());
}

LabelLayer::LabelLayer(Section* _section) 
	: labelled_(_section)
{
	std::vector<Section*> sections;
	
	buildSectionList(sections, labelled_);
	std::vector<Section*> left_sections;
	std::vector<Section*> right_sections;
	for(std::vector<Section*>::iterator it = sections.begin(); it != sections.end(); ++it)
	{
		if((*it)->GetGlobalPosition().x - _section->GetGlobalPosition().x < 0)
			left_sections.push_back(*it);
		else
			right_sections.push_back(*it);
	}
	std::sort(left_sections.begin(), left_sections.end(), YPositionSort<Section_ptr>());
	std::sort(right_sections.begin(), right_sections.end(), YPositionSort<Section_ptr>());

}

LabelLayer::~LabelLayer(void)
{
}

void LabelLayer::EndSubscription(Subscriber* _source)
{
	if(labelled_ == _source)
	{
		labelled_ = NULL;
	}
}
