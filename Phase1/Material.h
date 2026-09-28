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

class Dielectric : public Material
{
public:
	explicit Dielectric(double refractionIndex)
		: mRefractionIndex(refractionIndex)
	{
	}

	bool Scatter(
		const Ray& rayIn,
		const HitRecord& hitRecord,
		Color& attenuation,
		Ray& scattered
	) const override
	{
		attenuation = Color(1.0, 1.0, 1.0);

		const double refractionRatio =
			hitRecord.bFrontFace ? (1.0 / mRefractionIndex) : mRefractionIndex;

		const Vector3 unitDirection = UnitVector(rayIn.Direction());
		const double cosTheta =
			std::fmin(Dot(-unitDirection, hitRecord.Normal), 1.0);
		const double sinTheta =
			std::sqrt(1.0 - cosTheta * cosTheta);

		const bool cannotRefract =
			refractionRatio * sinTheta > 1.0;

		Vector3 direction;

		if (cannotRefract || Reflectance(cosTheta, refractionRatio) > RandomDouble())
		{
			direction = Reflect(unitDirection, hitRecord.Normal);
		}
		else
		{
			direction = Refract(unitDirection, hitRecord.Normal, refractionRatio);
		}

		scattered = Ray(hitRecord.P, direction);
		return true;
	}

private:
	// 진공 또는 공기 중 굴절률, 또는 재질의 굴절률을
	// 둘러싼 매질의 굴절률로 나눈 비율
	double mRefractionIndex;

	static double Reflectance(double cosine, double refractionIndex)
	{
		// Schlick의 반사율 근사 사용
		auto r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
		r0 = r0 * r0;
		return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5);
	}
};

#endif
