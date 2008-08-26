#include "FlexFloat.h"
#include <UnitTest++.h>

TEST(FlexFloatTests)
{
	/* Check +=, -=, /=, *= operators with floats */
	FlexFloat value = FlexFloat(10.0f);
	value -= 15.0f;
	CHECK_EQUAL(value.GetValue(), 0.0f);
	value += 15.0f;
	CHECK_EQUAL(value.GetValue(), 15.0f);
	value *= 3.0f;
	CHECK_EQUAL(value.GetValue(), 45.0f);
	value /= 5.0f;
	CHECK_EQUAL(value.GetValue(), 9.0f);

	/* Check +, -, *, / operators with floats */
	value = value + 5.0f;
	CHECK_EQUAL(value.GetValue(), 14.0f);
	value = value - 3.0f;
	CHECK_EQUAL(value.GetValue(), 11.0f);
	value = value * 4.0f;
	CHECK_EQUAL(value.GetValue(), 44.0f);
	value = value / 2.0f;
	CHECK_EQUAL(value.GetValue(), 22.0f);

	/* Check assignment operator */
	value = 9;

	/* Check +=, -=, /=, *= operators with other FlexFloats */
	FlexFloat value2 = FlexFloat(5);
	value += value2;
	CHECK_EQUAL(value.GetValue(), 14.0f);
	value *= value2;
	CHECK_EQUAL(value.GetValue(), 70.0f);
	value -= value2;
	CHECK_EQUAL(value.GetValue(), 65.0f);
	value /= value2;
	CHECK_EQUAL(value.GetValue(), 13.0f);

	/* Check +, -, *, / operators with other FlexFloats */
	value = 20;
	value = value + value2;
	CHECK_EQUAL(value.GetValue(), 25.0f);
	value = value * value2;
	CHECK_EQUAL(value.GetValue(), 125.0f);
	value = value - value2;
	CHECK_EQUAL(value.GetValue(), 120.0f);
	value = value / value2;
	CHECK_EQUAL(value.GetValue(), 24.0f);

	value.SetMaxValue(20);
	CHECK_EQUAL(value.GetValue(), 20.0f);
	value += 5;
	CHECK_EQUAL(value.GetValue(), 20.0f);
	value -= 30;
	CHECK_EQUAL(value.GetValue(), 0.0f);

	value = 5;
	CHECK(value < 10);
	CHECK(value > 4);
	CHECK(value >= 5);
	CHECK(value <= 5);

	CHECK(value >= 3);
	CHECK(value <= 8);
}