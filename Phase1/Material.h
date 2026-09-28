#ifndef MATERIAL_H
#define MATERIAL_H

#include "Hittable.h"

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& rayIn, const HitRecord& hitRecord, Color& attenuation, Ray& scattered) const
	{
		return false;
	}
};

class Lambertian : public Material
{
public:
	explicit Lambertian(const Color& albedo)
		: mAlbedo(albedo)
	{
	}

	bool Scatter(const Ray& rayIn, const HitRecord& hitRecord, Color& attenuation,
		Ray& scattered) const override
	{
		Vec3 scatterDirection = hitRecord.Normal + RandomUnitVector();

		// Catch degenerate scatter direction
		if (scatterDirection.NearZero())
		{
			scatterDirection = hitRecord.Normal;
		}

		scattered = Ray(hitRecord.P, scatterDirection);
		attenuation = mAlbedo;

		return true;
	}

private:
	Color mAlbedo;
};

class Metal : public Material
{
public:
	explicit Metal(const Color& albedo, double fuzz)
		: mAlbedo(albedo)
		, mFuzz(fuzz < 1 ? fuzz : 1)
	{
	}

	bool Scatter(const Ray& rayIn, const HitRecord& hitRecord, Color& attenuation,
		Ray& scattered) const override
	{
		Vec3 reflected = Reflect(rayIn.Direction(), hitRecord.Normal);
		reflected = UnitVector(reflected) + (mFuzz * RandomUnitVector());

		scattered = Ray(hitRecord.P, reflected);
		attenuation = mAlbedo;

		return (Dot(scattered.Direction(), hitRecord.Normal) > 0);
	}

private:
	Color mAlbedo;
	double mFuzz;
};

#endif
