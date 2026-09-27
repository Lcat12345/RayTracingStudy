#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// Constants

constexpr double Infinity = std::numeric_limits<double>::infinity();
constexpr double Pi = 3.141592653589793285;

// Utility Functions

inline double DegreesToRadians(double degrees)
{
	return degrees * Pi / 180.0;
}

// random C style
/*
inline double RandomDouble()
{
	// Returns a random real in [0,1)
	// 0이상 1미만
	// rand() -> 0과 RAND_MAX 사이의 정수를 반환
	return std::rand() / (RAND_MAX + 1.0);
}

inline double RandomDouble(double minimum, double maximum)
{
	// Returns a random real in [minimum, maximum)
	return minimum + (maximum - minimum) * RandomDouble();
}
*/

// random C++ Style
inline double RandomDouble()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double RandomDouble(double minimum, double maximum)
{
	// Returns a random real in [minimum, maximum)
	return minimum + (maximum - minimum) * RandomDouble();
}

// Common Headers

#include "Color.h"
#include "Interval.h"
#include "Ray.h"
#include "Vec3.h"

#endif
