#ifndef CAMERA_H
#define CAMERA_H

#include "RtWeekend.h"
#include "Hittable.h"
#include "Material.h"

class Camera
{
public:
	double aspectRatio = 1.0;			// Ratio of image width over height
	int imageWidth = 100;				// Rendered image width in pixel count
	int samplesPerPixel = 10;			// Count of random samples for each pixel
	int maxDepth = 10;					// Maximum number of ray bounces into scene

	double vFov = 90;					// 수직 시야각(시야)
	Point3 lookFrom = Point3(0, 0, 0);	// 카메라가 바라보는 위치
	Point3 lookAt = Point3(0, 0, -1);	// 카메라가 바라보는 점
	Vec3 vUp = Vec3(0, 1, 0);			// 카메라 상대 "위쪽" 방향

	double defocus_angle = 0;			// Variation angle of rays through each pixel
	double focus_dist = 10;				// Distance from camera lookfrom ponit to plane of perfect focus

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

		mCenter = lookFrom;

		// Determine viewport dimensions
		auto focalLength = (lookFrom - lookAt).Length();
		auto theta = DegreesToRadians(vFov);
		auto h = std::tan(theta / 2);
		auto viewportHeight = 2 * h * focalLength;
		auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / mImageHeight);

		// 카메라 좌표 프레임에 대한 u,v,w 단위 기저 벡터 계산
		w = UnitVector(lookFrom - lookAt);
		u = UnitVector(Cross(vUp, w));
		v = Cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges
		auto viewportU = viewportWidth * u;
		auto viewportV = viewportHeight * -v;

		// Calculate the horizontal and vertical delta vectors from pixel to pixel
		mPixelDeltaU = viewportU / imageWidth;
		mPixelDeltaV = viewportV / mImageHeight;

		// Calculate the location of the upper left pixel
		auto viewportUpperLeft =
			mCenter
			- (focalLength * w)
			- viewportU / 2.0
			- viewportV / 2.0;

		mPixel00Location = viewportUpperLeft + 0.5 * (mPixelDeltaU + mPixelDeltaV);

		const double defocusRadius =
			focus_dist * std::tan(DegreesToRadians(defocus_angle * 0.5));

		mDefocusDiskU = u * defocusRadius;
		mDefocusDiskV = v * defocusRadius;
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

		auto rayOrigin = (defocus_angle <= 0.0) ? mCenter : DefocusDiskSample();
		auto rayDirection = pixelSample - rayOrigin;

		return Ray(rayOrigin, rayDirection);
	}

	Vec3 SampleSquare() const
	{
		// Returns the vector to a random point in the [-0.5, -0.5] - [+0.5, +0.5] unit square
		return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
	}

	Point3 DefocusDiskSample() const
	{
		const Vec3 point = RandomInUnitDisk();
		return mCenter + (point.X() * mDefocusDiskU) + (point.Y() * mDefocusDiskV);
	}

	Color RayColor(const Ray& ray, int depth, const Hittable& world) const
	{
		// If we've exceeded the ray bounce limit, no more light is gathered
		if (depth <= 0)
		{
			return Color(0.0, 0.0, 0.0);
		}

		HitRecord hitRecord;

		if (world.Hit(ray, Interval(0.001, Infinity), hitRecord))
		{
			Ray scattered;
			Color attenuation;

			if (hitRecord.material->Scatter(ray, hitRecord, attenuation, scattered))
			{
				return attenuation * RayColor(scattered, depth - 1, world);
			}

			return Color(0.0,0.0,0.0);
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
	Vec3 u, v, w;						// 카메라 프레임 기저 벡터

	Vec3 mDefocusDiskU;					// Defocus disk horizontal radius
	Vec3 mDefocusDiskV;					// Defocus disk vertical radius
};

#endif
