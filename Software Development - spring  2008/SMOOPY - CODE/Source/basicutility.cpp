#include "basicutility.h"

float Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}