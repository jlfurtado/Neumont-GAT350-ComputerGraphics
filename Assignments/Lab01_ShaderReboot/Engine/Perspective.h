#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

// Justin Furtado
// 7/10/2016
// Perspective.h
// Holds data for calculating perspective matrices

#include "ExportHeader.h"
#include "Mat4.h"

namespace Engine
{
	class ENGINE_SHARED Perspective
	{
	public:
		// ctor/dtor
		Perspective();
		~Perspective();

		// methods
		void SetPerspective(float aspectRatio, float fieldOfViewY, float nearPlane, float farPlane);
		void SetAspectRatio(float aspectRatio);
		void SetScreenDimmensions(float width, float height);
		float GetFOVY();
		float GetNearDist();
		float GetWidth();
		float GetHeight();
		Mat4 GetPerspective();

	private:
		// data
		float m_width;
		float m_height;
		float m_aspectRatio;
		float m_fieldOfViewY;
		float m_nearPlane;
		float m_farPlane;
	};
}

#endif // ifndef PERSPECTIVE_H