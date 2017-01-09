#include "Perspective.h"

// Justin Furtadon
// 7/10/2016
// Perspective.h
// Holds data for calculating perspective matrices

namespace Engine
{
	Perspective::Perspective()
		: m_aspectRatio(0.0f), m_nearPlane(0.0f), m_farPlane(0.0f), m_fieldOfViewY(0.0f)
	{
	}

	Perspective::~Perspective()
	{
	}

	void Perspective::SetPerspective(float aspectRatio, float fieldOfViewY, float nearPlane, float farPlane)
	{
		m_aspectRatio = aspectRatio;
		m_fieldOfViewY = fieldOfViewY;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
	}

	void Perspective::SetAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
	}

	void Perspective::SetScreenDimmensions(float width, float height)
	{
		m_width = width;
		m_height = height;
	}

	float Perspective::GetFOVY()
	{
		return m_fieldOfViewY;
	}

	float Perspective::GetNearDist()
	{
		return m_nearPlane;
	}

	float Perspective::GetWidth()
	{
		return m_width;
	}

	float Perspective::GetHeight()
	{
		return m_height;
	}

	Mat4 Perspective::GetPerspective()
	{
		return Mat4::Perspective(m_fieldOfViewY, m_aspectRatio, m_nearPlane, m_farPlane);
	}
}
