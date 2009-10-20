#pragma once
#include "vmath.h"

template <class ref_t, class list_t> 
class RelativeRangeSort: public std::binary_function<list_t, list_t, bool> 
{
	const ref_t reference_;
public:
	RelativeRangeSort( const ref_t _reference) : reference_(_reference) {}

	bool operator()(list_t a, list_t b) const
	{
		return Collisions2f::DistanceSqr(a->GetGlobalPosition(), reference_->GetGlobalPosition()) <
			   Collisions2f::DistanceSqr(b->GetGlobalPosition(), reference_->GetGlobalPosition());
	}
};

template <class list_t> 
class YPositionSort: public std::binary_function<list_t, list_t, bool> 
{
public:
	YPositionSort(){}

	bool operator()(list_t a, list_t b) const
	{
		return a->GetGlobalPosition().y > b->GetGlobalPosition().y;
	}
};

