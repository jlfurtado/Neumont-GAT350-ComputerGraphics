#ifndef GRAPHICALOBJECT_H
#define GRAPHICALOBJECT_H

// Justin Furtado
// 7/6/2016
// GraphicalObject.h
// Represents something drawn to the screen

#include "GraphicalObject.h"
#include "GL\glew.h"
#include "Vec3.h"
#include "Mat4.h"
#include "UniformData.h"

namespace Engine
{
	class Mesh;
	class ENGINE_SHARED GraphicalObject
	{
	public:
		typedef void(*GraphicalObjectCallback)(GraphicalObject *gobInstance, void *classInstance);

		// ctor/dtor
		GraphicalObject();
		~GraphicalObject();

		// methods
		Mat4 GetScaleMat();
		Mat4 GetTransMat();
		Vec3 GetPos();
		Mat4 GetRotMat();
		Vec3 GetRotationAxis();
		Vec3 GetVelocity();
		Vec3 GetTintColor();
		Vec3 GetBaseColor();
		float GetTintIntensity();
		GLfloat GetRotation();
		GLfloat GetRotationRate();
		Mesh *GetMeshPointer();
		GraphicalObject *GetNextDrawingNode();
		GraphicalObject *GetNextCollidingNode();
		bool IsEnabled();
		void CallCallback();
		void SetCallback(GraphicalObjectCallback callback, void *classInstance);
		void PassUniforms();
		void AddUniformData(UniformData dataToAdd);
		void SetTintColor(Vec3 color);
		void SetBaseColor(Vec3 color);
		void SetTintIntensity(float newIntensity);
		void SetScaleMat(Mat4 scale);
		void SetTransMat(Mat4 trans);
		void SetRotMat(Mat4 rot);
		void SetRotationAxis(Vec3 axis);
		void SetVelocity(Vec3 vel);
		void SetRotation(GLfloat rot);
		void SetRotationRate(GLfloat rate);
		void SetMeshPointer(Mesh *pMesh);
		void PointToDrawingNode(GraphicalObject *nextNode);
		void PointToCollidingNode(GraphicalObject *nextNode);
		void SetEnabled(bool visible);

	private:
		// data
		static const int MAX_UNFIFORM_DATA = 1;
		UniformData m_uniformData[MAX_UNFIFORM_DATA];
		int m_numUniformDatas{ 0 };

		GraphicalObject *m_pNextDrawingNode;
		GraphicalObject *m_pNextCollidingNode;
		Vec3 m_tintColor;
		Vec3 m_baseColor;
		float m_tintIntensity;
		Mat4 m_scaleMatrix;
		Mat4 m_translationMatrix;
		Mat4 m_rotationMatrix;
		Vec3 m_rotationAxis;
		Vec3 m_velocity;
		GLfloat m_rotation;
		GLfloat m_rotationRate;
		GLfloat m_scaleRate;
		Mesh *m_pMesh;
		bool m_enabled;
		void *m_classInstance{ nullptr };
		GraphicalObjectCallback m_callback{ nullptr };
	};
}

#endif // ifndef GRAPHICALOBJECT_H