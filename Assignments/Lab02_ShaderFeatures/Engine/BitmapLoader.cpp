#include "BitmapLoader.h"
#include "GameLogger.h"
#include "MyGL.h"
#include <fstream>

namespace Engine
{
	bool BitmapLoader::Initialize()
	{
		GameLogger::Log(MessageType::Process, "BitmapLoader successfully initialized!\n");
		return true;
	}

	bool BitmapLoader::Shutdown()
	{
		GameLogger::Log(MessageType::Process, "BitmapLoader successfully shutdown!\n");
		return true;
	}

	const int HEADER_BYTES = 54;
	const int DATA_POSITION_LOCATION = 10;
	const int IMAGE_SIZE_LOCATION = 34;
	const int WIDTH_LOCATION = 18;
	const int HEIGHT_LOCATION = 22;
	GLuint BitmapLoader::LoadTexture(const char * const relativePath)
	{
		// variables to hold data to be read in
		GLubyte header[HEADER_BYTES];
		GLuint dataPosition;
		GLuint width;
		GLuint height;
		GLuint imageSize;
		GLubyte *data;

		// binary reader to read in file
		std::ifstream inputStream(relativePath, std::ios::binary | std::ios::in);

		if (!inputStream)
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to read file [%s]! Input file stream is no good!\n", relativePath);
			return GL_FALSE;
		}

		// read in header
		inputStream.read(reinterpret_cast<char*>(header), HEADER_BYTES);

		// validate file
		if (!(header[0] == 'B' && header[1] == 'M'))
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to load file [%s]! File is not a correct bitmap file!\n", relativePath);
			return GL_FALSE;
		}

		// extract data from header 
		dataPosition = *(reinterpret_cast<int*>(&header[DATA_POSITION_LOCATION]));
		imageSize = *(reinterpret_cast<int*>(&header[IMAGE_SIZE_LOCATION]));
		width = *(reinterpret_cast<int*>(&header[WIDTH_LOCATION]));
		height = *(reinterpret_cast<int*>(&header[HEIGHT_LOCATION]));

		// fill-in missing info if present
		if (imageSize == 0) { imageSize = width*height * 3; } // one byte per each of RGB
		if (dataPosition == 0) { dataPosition = HEADER_BYTES; } // assume data follows header

		// allocate memory for image
		data = new GLubyte[imageSize];

		// validate memory was allocated
		if (!data)
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to allocate [%d] bytes of memory for bitmap [%s]!\n", imageSize, relativePath);
			return GL_FALSE;
		}

		// read in the data
		inputStream.read(reinterpret_cast<char*>(data), imageSize);

		// close the file stream because it is no longer needed
		inputStream.close();

		// create the opengl texture
		GLuint textureID;
		glGenTextures(1, &textureID);

		// check if the texture was created
		if (MyGL::TestForError(MessageType::cError, "BitmapLoader Gen Texture Errors") || !textureID)
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to generate texture id for bitmap [%s] after reading it in!\n", relativePath);
			return GL_FALSE;
		}

		// bind the newly created texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		// check for errors in binding the texture
		if (MyGL::TestForError(MessageType::cError, "BitmapLoader Bind Texture Errors"))
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to bind texture with id [%d] for bitmap [%s]!\n", textureID, relativePath);
			return GL_FALSE;
		}

		// send the texture data to opengl
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		// do more error checking
		if (MyGL::TestForError(MessageType::cError, "BitmapLoader Texture Data Errors"))
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to send texture data for texture with id [%d] to opengl!\n", textureID);
			return GL_FALSE;
		}

		// set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// more error checking
		if (MyGL::TestForError(MessageType::cError, "BitmapLoader Texture Parameter Errors"))
		{
			GameLogger::Log(MessageType::cError, "BitmapLoader failed to set parameters for the texture with id [%d]!\n", textureID);
			return GL_FALSE;
		}

		// success
		return textureID;
	}
}