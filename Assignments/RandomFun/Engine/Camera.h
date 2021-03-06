#ifndef CAMERA_H
#define CAMERA_H

// Justin Furtado
// 7/10/2016
// Camera.h
// Shows the world!

#include "Mat4.h"
#include "Vec3.h"

namespace Engine
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		Mat4 GetWorldToViewMatrix();

		bool Initialize();
		bool Shutdown();

		void StrafeLeft(float dt);
		void StrafeRight(float dt);
		void Ascend(float dt);
		void Descend(float dt);
		void MoveForward(float dt);
		void MoveBackward(float dt);
		void SetSpeed(float unitsPerSecond);
		void SetRotateSpeed(float speed);
		float GetRotateSpeed();
		float GetSpeed();
		float GetYaw();
		float GetPitch();
		void MouseRotate(int dx, int dy);
		Vec3 GetPosition();
		void SetPosition(Vec3 newPosition);


	private:
		// methods
		float WrapAngle(float angleToWrap);

		// data
		bool m_following;
		Vec3 m_up;
		Vec3 m_viewDirection;
		Vec3 m_position;
		Vec3 m_positionTranslate;
		float m_yaw;
		float m_pitch;
		float m_cameraSpeed;
		float m_rotateSpeed;
	};
}

#endif // ifndef CAMERA_H