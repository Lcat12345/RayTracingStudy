#include "Color.h"
#include "Ray.h"
#include "Vec3.h"

#include <iostream>

Color RayColor(const Ray& r)
{
	Vec3 unitDirection = UnitVector(r.Direction());
	auto a = 0.5 * (unitDirection.Y() + 1.0);

	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
	// Image
	auto aspectRatio = 16.0 / 9.0;
	int imageWidth = 400;

	// 이미지 높이를 계산하고 최소 1이 되도록 합니다.
	int imageHeight = int(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	// Camera
	auto focalLength = 1.0;
	auto viewportHeight = 2.0;
	auto viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
	auto cameraCenter = Point3(0, 0, 0);

	// 뷰포트의 수평 및 수직 가장자리를 가로지르는 벡터를 계산합니다.
	auto viewportU = Vec3(viewportWidth, 0, 0);
	auto viewportV = Vec3(0, -viewportHeight, 0);

	// 픽셀 간 수평 및 수직 델타 벡터를 계산합니다.
	auto pixelDeltaU = viewportU / imageWidth;
	auto pixelDeltaV = viewportV / imageHeight;

	// 왼쪽 위 픽셀의 위치를 계산합니다.
	auto viewportUpperLeft = cameraCenter - Vec3(0, 0, focalLength) - viewportU / 2 - viewportV / 2;
	auto pixxel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	// Render
	std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

	for (int j = 0; j < imageHeight; j++)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++)
		{
			auto pixelCenter = pixxel00Loc + (i * pixelDeltaU) + (j * pixelDeltaV);
			auto rayDirection = pixelCenter - cameraCenter;
			Ray r(cameraCenter, rayDirection);

			Color pixelColor = RayColor(r);
			WriteColor(std::cout, pixelColor);
		}
	}
}