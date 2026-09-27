#ifndef CAMERA_H
#define CAMERA_H

#include "Hittable.h"

class Camera
{
public:
	double aspectRatio = 1.0;			// Ratio of image width over height
	int imageWidth = 100;				// Rendered image width in pixel count
	int samplesPerPixel = 10;			// Count of random samples for each pixel
	int maxDepth = 10;					// Maximum number of ray bounces into scene

	void Render(const Hittable& world)
	{
		Initialize();

		std::cout << "P3\n" << imageWidth << ' ' << mImageHeight << "\n255\n";

		for (int scanlineIndex = 0; scanlineIndex < mImageHeight; scanlineIndex++)
		{
			std::clog
				<< "\rScanlines remaining: "
				<< (mImageHeight - scanlineIndex)
				<< ' '
				<< std::flush;

			for (int pixelIndex = 0; pixelIndex < imageWidth; pixelIndex++)
			{
				Color pixelColor(0.0, 0.0, 0.0);

				for (int sampleIndex = 0; sampleIndex < samplesPerPixel; sampleIndex++)
				{
					Ray ray = GetRay(pixelIndex, scanlineIndex);
					pixelColor += RayColor(ray, maxDepth, world);
				}

				WriteColor(std::cout, mPixelSamplesScale * pixelColor);
			}
		}

		std::clog << "\rDone.			\n";
	}

private:
	void Initialize()
	{
		mImageHeight = static_cast<int>(imageWidth / aspectRatio);
		mImageHeight = (mImageHeight < 1) ? 1 : mImageHeight;

		mPixelSamplesScale = 1.0 / static_cast<double>(samplesPerPixel);

		mCenter = Point3(0.0, 0.0, 0.0);

		// Determine viewport dimensions
		auto focalLength = 1.0;
		auto viewportHeight = 2.0;
		auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / mImageHeight);

		// Calculate the vectors across the horizontal and down the vertical viewport edges
		auto viewportU = Vec3(viewportWidth, 0.0, 0.0);
		auto viewportV = Vec3(0.0, -viewportHeight, 0.0);

		// Calculate the horizontal and vertical delta vectors from pixel to pixel
		mPixelDeltaU = viewportU / imageWidth;
		mPixelDeltaV = viewportV / mImageHeight;

		// Calculate the location of the upper left pixel
		auto viewportUpperLeft =
			mCenter
			- Vec3(0.0, 0.0, focalLength)
			- viewportU / 2.0
			- viewportV / 2.0;

		mPixel00Location = viewportUpperLeft + 0.5 * (mPixelDeltaU + mPixelDeltaV);
	}

	Ray GetRay(int pixelIndex, int scanlineIndex) const
	{
		// Construct a camera ray originating from the origin and directed at randomly sampled
		// point around the pixel location pixelIndex, scanlineIndex.

		auto offset = SampleSquare();

		// 범위를 사각형으로 그려보면 크기 1짜리 정사각형이 나옴
		auto pixelSample =
			mPixel00Location
			+ ((pixelIndex + offset.X()) * mPixelDeltaU)
			+ ((scanlineIndex + offset.Y()) * mPixelDeltaV);

		auto rayOrigin = mCenter;
		auto rayDirection = pixelSample - rayOrigin;

		return Ray(rayOrigin, rayDirection);
	}

	Vec3 SampleSquare() const
	{
		// Returns the vector to a random point in the [-0.5, -0.5] - [+0.5, +0.5] unit square
		return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
	}

	Color RayColor(const Ray& ray, int depth, const Hittable& world) const
	{
		// If we've exceeded the ray bounce limit, no more light is gathered
		if (depth <= 0)
		{
			return Color(0.0, 0.0, 0.0);
		}

		HitRecord hitRecord;

		// Default
		// if (world.Hit(ray, Interval(0, Infinity), hitRecord))
		// Hit 범위의 최소값을 줌 부동소수점 오차로 인해 똑같은 위치로 
		// 반사되는 것을 방지함
		if (world.Hit(ray, Interval(0.001, Infinity), hitRecord))
		{
			// 랜덤하게 광선을 보낸다.
			//Vec3 direction = RandomOnHemisphere(hitRecord.Normal);
			// 충돌 지점 P에서 무작위 점 S로 광선을 보낸다.
			Vec3 direction = hitRecord.Normal + RandomUnitVector();

			return 0.5 * RayColor(Ray(hitRecord.P, direction), depth - 1, world);
		}

		Vector3 unitDirection = UnitVector(ray.Direction());
		auto a = 0.5 * (unitDirection.Y() + 1.0);

		return  (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}

private:
	int mImageHeight = 0;				// Rendered image height
	double mPixelSamplesScale = 1.0;	// Color scale factor for a sum of pixel samples

	Point3 mCenter;						// Camera center
	Point3 mPixel00Location;			// Location of pixel 0,0
	Vec3 mPixelDeltaU;					// Offset to pixel to the right;
	Vec3 mPixelDeltaV;					// Offset to pixel below
};

#endif
