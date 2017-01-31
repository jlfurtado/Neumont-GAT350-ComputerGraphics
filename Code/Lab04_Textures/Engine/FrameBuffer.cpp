#include "FrameBuffer.h"
#include "MyGL.h"

// Justin Furtado
// 1/30/2017
// FrameBuffer.h
// Class to hold framebuffer code

namespace Engine
{
	FrameBuffer::FrameBuffer() 
		: width(0), height(0), depthBufferId(0), fboHandle(0), renderTextureId(0)
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	const GLsizei numTextureLevels = 1;
	const GLenum  internalFormat = GL_RGBA8;
	const GLint   xOffset = 0; // offsets into texture
	const GLint   yOffset = 0;
	const GLint   mipMapLevel = 0;
	const GLuint  unbindFbo = 0; // 0 unbinds fbo
	bool FrameBuffer::Initialize(int w, int h)
	{
		width = w;
		height = h;

		// make the texture
		glGenTextures(1, &renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize GenTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Texture in FrameBufffer Initialize!\n");
			return false;
		}

		// bind the texture we just created
		glBindTexture(GL_TEXTURE_2D, renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize BindTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Texture in FrameBufffer Initialize!\n");
			return false;
		}

		// send some data about the texture to opengl
		glTexStorage2D(GL_TEXTURE_2D, numTextureLevels, internalFormat, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize TexStorage2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexStorage2D in failedFrameBufffer Initialize!\n");
			return false;
		}

		// set some parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize TexParameteri Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexParameteri failed in FrameBufffer Initialize!\n");
			return false;
		}

		// Create framebuffer object
		glGenFramebuffers(1, &fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize GenFramebuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Framebuffer in FrameBufffer Initialize!\n");
			return false;
		}

		// bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize BindFramebuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to BindFramebuffer in FrameBufffer Initialize!\n");
			return false;
		}

		// Bind the texture to the fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureId, mipMapLevel);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize FramebufferTexture2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Hook framebuffer to texture in FrameBufffer Initialize!\n");
			return false;
		}

		// Create the depth buffer and bind it
		glGenRenderbuffers(1, &depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize GenRenderbuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Renderbuffers in FrameBufffer Initialize!\n");
			return false;
		}

		// bind the render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize BindRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Renderbuffer in FrameBufffer Initialize!\n");
			return false;
		}

		// send data to opengl
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize RenderbufferStorage Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to send info about renderbuffer to opengl in FrameBufffer Initialize!\n");
			return false;
		}

		// Bind the depth buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize FramebufferRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to hook up the framebuffer to the renderbuffer in FrameBufffer Initialize!\n");
			return false;
		}

		// Set the target for the fragment shader outputs
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize DrawBuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to set target for fragment shader outputs in FrameBufffer Initialize!\n");
			return false;
		}

		// Check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			GameLogger::Log(MessageType::cError, "FrameBuffer Initialize Failed! Framebuffer status is not complete!\n");
			return false;
		}

		// Unbind FBO and revert to default
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		return true;
	}
	
	void FrameBuffer::Bind()
	{
		// Bind to texture's FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glViewport(xOffset, yOffset, width, height);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	void FrameBuffer::UnBind(int viewX, int viewY, int viewWidth, int viewHeight)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		glViewport(viewX, viewY, viewWidth, viewHeight);
	}

	GLuint * FrameBuffer::GetTexIdPtr()
	{
		return &renderTextureId;
	}
}
