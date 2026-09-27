#include "RtWeekend.h"

#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

#include "Color.h"
#include "Ray.h"
#include "Vec3.h"

#include <iostream>

double HitSphere(const Point3& center, double radius, const Ray& ray)
{
	Vec3 originToCenter = center - ray.Origin();
	auto a = ray.Direction().LengthSquared();
	auto h = Dot(ray.Direction(), originToCenter);
	auto c = originToCenter.LengthSquared() - radius * radius;
	auto discriminant = h * h - a * c;

	if (discriminant < 0.0)
	{
		return -1.0;
	}

	return ((h - std::sqrt(discriminant)) / a);
}

Color RayColor(const Ray& ray, const Hittable& world)
{
	HitRecord hitRecord;
	if (world.Hit(ray, 0.0, Infinity, hitRecord))
	{
		return 0.5 * (hitRecord.Normal + Color(1.0, 1.0, 1.0));
	}

	Vector3 unitDirection = UnitVector(ray.Direction());
	auto a = 0.5 * (unitDirection.Y() + 1.0);

	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
	// Image
	auto aspectRatio = 16.0 / 9.0;
	int imageWidth = 400;

	// 이미지 높이를 계산하고 최소 1이 되도록 합니다.
	int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	// World
	HittableList world;
	world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5));
	world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0));

	// Camera
	auto focalLength = 1.0;
	auto viewportHeight = 2.0;
	auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / imageHeight);
	auto cameraCenter = Point3(0, 0, 0);

	// 뷰포트의 수평 및 수직 가장자리를 가로지르는 벡터를 계산합니다.
	auto viewportU = Vec3(viewportWidth, 0, 0);
	auto viewportV = Vec3(0, -viewportHeight, 0);

	// 픽셀 간 수평 및 수직 델타 벡터를 계산합니다.
	auto pixelDeltaU = viewportU / imageWidth;
	auto pixelDeltaV = viewportV / imageHeight;

	// 왼쪽 위 픽셀의 위치를 계산합니다.
	auto viewportUpperLeft = 
		cameraCenter 
		- Vec3(0, 0, focalLength) 
		- viewportU / 2 
		- viewportV / 2;

	auto pixxel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	// Render
	std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

	for (int scanlineIndex = 0; scanlineIndex < imageHeight; scanlineIndex++)
	{
		std::clog
			<< "\rScanlines remaining: "
			<< (imageHeight - scanlineIndex)
			<< ' '
			<< std::flush;

		for (int pixelIndex = 0; pixelIndex < imageWidth; pixelIndex++)
		{
			auto pixelCenter =
				pixxel00Loc
				+ (pixelIndex * pixelDeltaU)
				+ (scanlineIndex * pixelDeltaV);

			auto rayDirection = pixelCenter - cameraCenter;
			Ray ray(cameraCenter, rayDirection);

			Color pixelColor = RayColor(ray, world);
			WriteColor(std::cout, pixelColor);
		}
	}

	std::clog << "\rDone.			\n";
}