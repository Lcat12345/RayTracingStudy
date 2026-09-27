#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"

class HitRecord
{
public:
	void SetFaceNormal(const Ray& r, const Vec3& outwardNormal)
	{
		// 히트 레코드 법선 벡터를 설정합니다.
		// 참고: 매개변수 'outwardNormal'은 단위 길이를 가진다고 가정합니다.
		 
		bFrontFace = Dot(r.Direction(), outwardNormal) < 0;
		Normal = bFrontFace ? outwardNormal : -outwardNormal;
	}	

	// 법선은 항상 밖을 향한다.
	// 1. 법선이 항상 밖을 향한다. ( 지금까지 구한 법선들 구의 중심에서 교참점 방향 P - C -> 여기서 크기로 나누면 외향법선)
	// 광선이 구의 외부에서 교차하면 그 방향이 법선과 반대임
	// 광선이 구의 내부에서 교차하면 그 방향이 법선과 똑같음
	// 2. 법선이 항상 광선의 반대 방향이다.
	// 광선이 구의 외부에 있으면 법선은 밖을 향한다. 내부에 있으면 안쪽을 향한다.

	// 하고 싶은건 광선이 어느 쪽 표면에서 오는지 결정하고 싶다.
	// 법선이 항상 밖을 향한다고 가정하면 법선과 광선을 내적해서 양수면 광선도 법선이랑
	// 방향이 같다는 거니까 안쪽에서 오는 광선이고
	// 음수면 바깥쪽에서 오는 광선이라고 할 수 있다.

	// 그러나 법선이 항상 광선의 반대방향이라고 가정을 하면 내적으로 결정할 수가 없다.
	// 먼저 광선이 밖에서 온건지 안에서 온건지 확인한다. 법선과 내적해서 원래의 가정대로
	// 내적이 양수면 방향이 법선과 광선이 같다는거고 법선은 항상 밖을 향한다고 했으니까
	// 안에서 밖으로 나가는 광선이다. frontface = false
	// 그런데 여기선 항상 광선의 반대 방향을 가정했으니 법선의 부호를 반대로 해준다.
	// 내적이 음수면 방향이 법선과 광선이 반대라는 거고 법선은 항상 밖을 향한다고 했으니
	// 광선은 밖에서 안으로 향하는 거다. frontface = true

	Point3 P;
	Vec3 Normal;
	double T;
	bool bFrontFace;
};

class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual bool Hit(
		const Ray& r, 
		const Interval& rayT,
		HitRecord& hitRecord
	) const = 0;
};

#endif
