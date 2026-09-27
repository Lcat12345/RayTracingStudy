#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// Constants

constexpr double Infinity = std::numeric_limits<double>::infinity();
constexpr double Pi = 3.141592653589793285;

// Utility Functions

inline double DegreesToRadians(double degrees)
{
	return degrees * Pi / 180.0;
}

// Common Headers

#include "Color.h"
#include "Interval.h"
#include "Ray.h"
#include "Vec3.h"

#endif
