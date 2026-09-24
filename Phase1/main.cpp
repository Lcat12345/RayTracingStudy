#include "Color.h"
#include "Vec3.h"

#include <iostream>

int main()
{
	// Image
	int ImageWidth = 256;
	int ImageHeight = 256;

	// Render
	std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

	for (int j = 0; j < ImageHeight; j++)
	{
		std::clog << "\rScanlines remaining: " << (ImageHeight - j) << ' ' << std::flush;
		for (int i = 0; i < ImageWidth; i++)
		{
			auto PixelColor = Color(double(i) / (ImageWidth - 1), double(j) / (ImageHeight - 1), 0);
			WriteColor(std::cout, PixelColor);
		}
	}

	std::clog << "\rDone.			\n";
}