#pragma once

/*
 * Represents a floating point value that can be affected by buffs, and limited in range
 * It cannot be negative, and can have a top limit set.
 * It is intended to add a buffs system
 */
class FlexFloat
{
private:
	float base_value_;
	float max_base_value_;
	bool use_max_base_value_;
	void ClampValue()
	{
		if(base_value_ < 0)
			base_value_ = 0;
		if(use_max_base_value_ && base_value_ > max_base_value_)
			base_value_ = max_base_value_;
	}
public:
	FlexFloat();
	explicit FlexFloat(float _base_value);
	FlexFloat(float _base_value, float _max_base_value);
	~FlexFloat(void);

	/*
	 * Sets the maximum value, and enables the max value limiter
	 */
	void SetMaxValue(float _max_base_value) {max_base_value_ = _max_base_value; use_max_base_value_ = true; ClampValue();}
	/*
	 * Gets the maximum value
	 */
	float GetMaxValue() {return max_base_value_;}
	/*
	 * Adds to the max_base_value
	 */
	void AddMaxValue(float _value_delta){max_base_value_ += _value_delta;}
	/*
	 * Enables or disables the upper value clamping
	 */
	void SetMaxValueEnabled(bool _enabled) {use_max_base_value_ = _enabled; ClampValue();}

	/*
	 * Gets the value
	 */
	float GetValue();

	/*
	 * Addition operator
	 * @param _n The value to be added
	 * @return previous value + _n
	 */
	 FlexFloat& operator+=(float _n)
	 {
		 base_value_ += _n;
		 ClampValue();
		 return *this;
	 }

	/*
	 * Addition operator
	 * @param _n The value to be added
	 * @return previous value + _n
	 */
	 FlexFloat operator+(const float _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ + _n, max_base_value_);
		 else
			return FlexFloat(base_value_ + _n);
	 }

	/*
	 * Addition operator
	 * @param _n The value to be added
	 * @return previous value + _n
	 * LHS maximum is preserved
	 */
	 FlexFloat& operator+=(FlexFloat& _n)
	 {
		base_value_ += _n.GetValue();
		ClampValue();
		return *this;
	 }

	/*
	 * Addition operator
	 * @param _n The value to be added
	 * @return previous value + _n
	 * LHS maximum is preserved
	 */
	 FlexFloat operator+(FlexFloat& _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ + _n.GetValue(), max_base_value_);
		 else
			return FlexFloat(base_value_ + _n.GetValue());
	 }
	 
	 /*
	 * Subtraction operator
	 * @param _n The value to be subtracted
	 * @return previous value - _n
	 */
	 FlexFloat& operator-=(float _n)
	 {
		 base_value_ -= _n;
		 ClampValue();
		 return *this;
	 }

	/*
	 * Subtraction operator
	 * @param _n The value to be subtracted
	 * @return previous value - _n
	 */
	 FlexFloat operator-(float _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ - _n, max_base_value_);
		 else
			return FlexFloat(base_value_ - _n);
	 }

	/*
	 * Subtraction operator
	 * @param _n The value to be subtracted
	 * @return previous value - _n
	 * LHS maximum is preserved
	 */
	 FlexFloat& operator-=(FlexFloat& _n)
	 {
		base_value_ -= _n.GetValue();
		ClampValue();
		return *this;
	 }

	/*
	 * Subtraction operator
	 * @param _n The value to be subtracted
	 * @return previous value - _n
	 * LHS maximum is preserved
	 */
	 FlexFloat operator-(FlexFloat& _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ - _n.GetValue(), max_base_value_);
		 else
			return FlexFloat(base_value_ - _n.GetValue());
	 }

	 /*
	 * Multiplication operator
	 * @param _n The value to be multiplied
	 * @return previous value * _n
	 */
	 FlexFloat& operator*=(float _n)
	 {
		 base_value_ *= _n;
		 ClampValue();
		 return *this;
	 }

	/*
	 * Multiplication operator
	 * @param _n The value to be multiplied
	 * @return previous value * _n
	 */
	 FlexFloat operator*(float _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ * _n, max_base_value_);
		 else
			return FlexFloat(base_value_ * _n);
	 }

	/*
	 * Multiplication operator
	 * @param _n The value to be multiplied
	 * @return previous value * _n
	 * LHS maximum is preserved
	 */
	 FlexFloat& operator*=(FlexFloat& _n)
	 {
		base_value_ *= _n.GetValue();
		ClampValue();
		return *this;
	 }

	/*
	 * Multiplication operator
	 * @param _n The value to be multiplied
	 * @return previous value * _n
	 * LHS maximum is preserved
	 */
	 FlexFloat operator*(FlexFloat& _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ * _n.GetValue(), max_base_value_);
		 else
			return FlexFloat(base_value_ * _n.GetValue());
	 }

 	 /*
	 * Division operator
	 * @param _n The value to be divided
	 * @return previous value / _n
	 */
	 FlexFloat& operator/=(float _n)
	 {
		 base_value_ /= _n;
		 ClampValue();
		 return *this;
	 }

	/*
	 * Division operator
	 * @param _n The value to be divided
	 * @return previous value / _n
	 */
	 FlexFloat operator/(float _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ / _n, max_base_value_);
		 else
			return FlexFloat(base_value_ / _n);
	 }

	/*
	 * Division operator
	 * @param _n The value to be divided
	 * @return previous value / _n
	 * LHS maximum is preserved
	 */
	 FlexFloat& operator/=(FlexFloat& _n)
	 {
		base_value_ /= _n.GetValue();
		ClampValue();
		return *this;
	 }

	/*
	 * Division operator
	 * @param _n The value to be divided
	 * @return previous value / _n
	 * LHS maximum is preserved
	 */
	 FlexFloat operator/(FlexFloat& _n)
	 {
		 if(use_max_base_value_)
			return FlexFloat(base_value_ / _n.GetValue(), max_base_value_);
		 else
			return FlexFloat(base_value_ / _n.GetValue());
	 }

	 float operator-()
	 {
		 if(use_max_base_value_)
			return -base_value_;
		 else
			return -base_value_;
	 }

	 /*
	  * Assignment operator
	  * @param _n The value being assigned
	  * @return The assigned value
	  */
	 FlexFloat operator=(float _n)
	 {
		 base_value_ = _n;
		 ClampValue();
		 return *this;
	 }

	 /*
	  * LToE
	  */
	 bool operator<=(float _n)
	 {
		 return GetValue() <= _n;
	 }
	 
	 /*
	  * GToE
	  */
	 bool operator>=(float _n)
	 {
		 return GetValue() >= _n;
	 }

	 /*
	  * LT
	  */
	 bool operator<(float _n)
	 {
		 return GetValue() < _n;
	 }
	 /*
	  * GT
	  */
	 bool operator>(float _n)
	 {
		 return GetValue() > _n;
	 }
	 /*
	  * EQ
	  */
	 bool operator==(float _n)
	 {
		 return GetValue() == _n;
	 }

};
