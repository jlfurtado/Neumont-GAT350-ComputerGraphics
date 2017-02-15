#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "ExportHeader.h"
#include "MyGL.h"

// Justin Furtado
// 1/30/2017
// FrameBuffer.h
// Class to hold framebuffer code

namespace Engine
{
	class ENGINE_SHARED FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		bool InitializeForTexture(int width, int height);
		bool InitializeForDepth(int width, int height);
		void Bind();
		void UnBind(int viewX, int viewY, int viewWidth, int viewHeight);
		GLuint * GetTexIdPtr();

	private:
		int width;
		int height;
		GLuint renderTextureId;
		GLuint fboHandle;
		GLuint depthBufferId;
	};
}

#endif // ifndef FRAMEBUFFER_H