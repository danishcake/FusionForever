#include "StdAfx.h"
#include "FlexFloat.h"

FlexFloat::FlexFloat(float _base_value)
: base_value_(_base_value), max_base_value_(0), use_max_base_value_(false)
{
	ClampValue();
}

FlexFloat::FlexFloat(float _base_value, float _max_base_value)
: base_value_(_base_value), max_base_value_(_max_base_value), use_max_base_value_(true)
{
	ClampValue();
}

FlexFloat::FlexFloat()
: base_value_(10), max_base_value_(10), use_max_base_value_(true)
{

}

FlexFloat::~FlexFloat(void)
{
}

float FlexFloat::GetValue() const 
{
	return base_value_;
}
