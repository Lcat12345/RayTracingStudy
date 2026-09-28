#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include <memory>

class Sphere : public Hittable
{
public:
	Sphere(const Point3& center, double radius, const std::shared_ptr<Material>& material)
		: mCenter(center), mRadius(std::fmax(0.0, radius))
		, mMaterial(material)
	{
	}

	bool Hit(
		const Ray& ray,
		const Interval& rayT,
		HitRecord& hitRecord
	) const override
	{
		Vec3 originToCenter = mCenter - ray.Origin();

		auto a = ray.Direction().LengthSquared();
		auto h = Dot(ray.Direction(), originToCenter);
		auto c = originToCenter.LengthSquared() - mRadius * mRadius;

		auto discriminant = h * h - a * c;
		if (discriminant < 0.0)
		{
			return false;
		}

		auto squareRootDiscriminant = std::sqrt(discriminant);

		// Find the nearset root that lies in the acceptable range
		// 유효한 충돌 구간 추가
		auto root = (h - squareRootDiscriminant) / a;
		if (!rayT.Surrounds(root))
		{
			root = (h + squareRootDiscriminant) / a;
			if (!rayT.Surrounds(root))
			{
				return false;
			}
		}

		hitRecord.T = root;
		hitRecord.P = ray.At(hitRecord.T);

		// 구의 법선은 반지름으로 나누기만 하면 단위 길이로 만들 수 있어 제곱근을 
		// 완전히 피할 수 있습니다.
		Vec3 outwardNormal = (hitRecord.P - mCenter) / mRadius;
		hitRecord.SetFaceNormal(ray, outwardNormal);

		hitRecord.material = mMaterial;

		return true;
	}

private:
	Point3 mCenter;
	double mRadius = 0.0;
	std::shared_ptr<Material> mMaterial;
};

#endif