#ifndef MATHUTILITY_H
#define MATHUTILITY_H

// Justin Furtado
// 10/13/2016
// MathUtility.h
// Degrees to radians, random, etc

#include "Vec3.h"
#include "ExportHeader.h"
#include "CollisionTester.h"

namespace Engine
{
	class MathUtility
	{
	public:
		static ENGINE_SHARED float ToRadians(float degrees);
		static ENGINE_SHARED float ToDegrees(float radians);
		static ENGINE_SHARED const float PI;
		static ENGINE_SHARED float Clamp(float value, float min, float max);
		static ENGINE_SHARED int Clamp(int value, int min, int max);
		static ENGINE_SHARED float Rand(float minValue, float maxValue);
		static ENGINE_SHARED Vec3 Rand(const Vec3& minVec, const Vec3& maxVec);
		static ENGINE_SHARED Vec3 GetNormalFromRayCastingOutput(RayCastingOutput output);
	};
}

#endif // ifndef MATHUTILITY_H