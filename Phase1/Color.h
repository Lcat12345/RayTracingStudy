#ifndef COLOR_H
#define COLOR_H

#include "Interval.h"
#include "Vec3.h"

using Color = Vec3;

void WriteColor(std::ostream& out, const Color& pixelColor)
{
	auto r = pixelColor.X();
	auto g = pixelColor.Y();
	auto b = pixelColor.Z();

	// [0,1] 범위의 컴포넌트 값을 바이트 범위 [0,255]로 변환합니다.
	static const Interval intensity(0.000, 0.999);

	int rByte = static_cast<int>(256 * intensity.Clamp(r));
	int gByte = static_cast<int>(256 * intensity.Clamp(g));
	int bByte = static_cast<int>(256 * intensity.Clamp(b));

	// 픽셀 색상 컴포넌트를 출력합니다.
	out << rByte << ' ' << gByte << ' ' << bByte << '\n';
}

#endif
