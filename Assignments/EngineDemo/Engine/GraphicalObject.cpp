#include "GraphicalObject.h"
#include "GameLogger.h"

// Justin Furtado
// 7/6/2016
// GraphicalObject.cpp
// Represents something drawn to the screen

namespace Engine
{
	GraphicalObject::GraphicalObject()
		: m_pMesh(nullptr), m_rotation(0.0f), m_rotationAxis(Vec3(0.0f, 1.0f, 0.0f)), m_rotationMatrix(Mat4()),
		m_scaleMatrix(Mat4()), m_rotationRate(0.0f), m_scaleRate(0.0f), m_translationMatrix(Mat4()), m_velocity(Vec3()),
		m_enabled(true), m_pNextCollidingNode(nullptr), m_pNextDrawingNode(nullptr), m_tintColor(Vec3()), m_tintIntensity(0.0f)
	{
	}

	GraphicalObject::~GraphicalObject()
	{
	}

	Mat4 GraphicalObject::GetScaleMat()
	{
		return m_scaleMatrix;
	}

	Mat4 GraphicalObject::GetTransMat()
	{
		return m_translationMatrix;
	}

	Vec3 GraphicalObject::GetPos()
	{
		return Vec3(m_translationMatrix.GetAddress()[12], m_translationMatrix.GetAddress()[13], m_translationMatrix.GetAddress()[14]);
	}

	Mat4 GraphicalObject::GetRotMat()
	{
		return m_rotationMatrix;
	}

	Vec3 GraphicalObject::GetRotationAxis()
	{
		return m_rotationAxis;
	}

	Vec3 GraphicalObject::GetVelocity()
	{
		return m_velocity;
	}

	Vec3 GraphicalObject::GetTintColor()
	{
		return m_tintColor;
	}

	Vec3 GraphicalObject::GetBaseColor()
	{
		return m_baseColor;
	}

	float GraphicalObject::GetTintIntensity()
	{
		return m_tintIntensity;
	}

	GLfloat GraphicalObject::GetRotation()
	{
		return m_rotation;
	}

	GLfloat GraphicalObject::GetRotationRate()
	{
		return m_rotationRate;
	}

	Mesh * GraphicalObject::GetMeshPointer()
	{
		return m_pMesh;
	}

	GraphicalObject * GraphicalObject::GetNextDrawingNode()
	{
		return m_pNextDrawingNode;
	}

	GraphicalObject * GraphicalObject::GetNextCollidingNode()
	{
		return m_pNextCollidingNode;
	}

	bool GraphicalObject::IsEnabled()
	{
		return m_enabled;
	}

	void GraphicalObject::CallCallback()
	{
		// only call the callback if it has been set
		if (m_callback)
		{
			m_callback(this, m_classInstance);
		}
	}

	void GraphicalObject::SetCallback(GraphicalObjectCallback callback, void *classInstance)
	{
		m_callback = callback;
		m_classInstance = classInstance;
	}

	void GraphicalObject::PassUniforms()
	{
		for (int i = 0; i < m_numUniformDatas; ++i)
		{
			m_uniformData[i].PassUniform();
		}
	}

	void GraphicalObject::AddUniformData(UniformData dataToAdd)
	{
		if (m_numUniformDatas >= MAX_UNFIFORM_DATA) { GameLogger::Log(MessageType::cError, "Ran out of space for uniform data!\n"); return; }
		m_uniformData[m_numUniformDatas++] = dataToAdd;
	}

	void GraphicalObject::SetTintColor(Vec3 color)
	{
		m_tintColor = color;
	}

	void GraphicalObject::SetBaseColor(Vec3 color)
	{
		m_baseColor = color;
	}

	void GraphicalObject::SetTintIntensity(float newIntensity)
	{
		m_tintIntensity = newIntensity;
	}

	void GraphicalObject::SetScaleMat(Mat4 scale)
	{
		m_scaleMatrix = scale;
	}

	void GraphicalObject::SetTransMat(Mat4 trans)
	{
		m_translationMatrix = trans;
	}

	void GraphicalObject::SetRotMat(Mat4 rot)
	{
		m_rotationMatrix = rot;
	}

	void GraphicalObject::SetRotationAxis(Vec3 axis)
	{
		m_rotationAxis = axis;
	}

	void GraphicalObject::SetVelocity(Vec3 vel)
	{
		m_velocity = vel;
	}

	void GraphicalObject::SetRotation(GLfloat rot)
	{
		m_rotation = rot;
	}

	void GraphicalObject::SetRotationRate(GLfloat rate)
	{
		m_rotationRate = rate;
	}

	void GraphicalObject::SetMeshPointer(Mesh * pMesh)
	{
		if (!pMesh) { GameLogger::Log(MessageType::cWarning, "Invalid mesh pointer passed in for graphical object, ignoring!\n"); return; }
		m_pMesh = pMesh;
	}

	void GraphicalObject::PointToCollidingNode(GraphicalObject * nextNode)
	{
		m_pNextCollidingNode = nextNode;
	}

	void GraphicalObject::PointToDrawingNode(GraphicalObject * nextNode)
	{
		m_pNextDrawingNode = nextNode;
	}

	void GraphicalObject::SetEnabled(bool visible)
	{
		m_enabled = visible;
	}
}

