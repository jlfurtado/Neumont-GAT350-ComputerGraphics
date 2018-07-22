#include "UniformData.h"
#include "GL\glew.h"
#include "GameLogger.h"

namespace Engine
{
	UniformData::UniformData()
		: m_uniformType(UniformType::UNIFORM3F), m_pUniformData(nullptr), m_uniformDataLoc(0)
	{
	}

	UniformData::UniformData(UniformType type, void * dataAddress, int dataLoc, int numElements)
		: m_uniformType(type), m_pUniformData(dataAddress), m_uniformDataLoc(dataLoc), m_numMatrices(numElements)
	{
	}

	bool UniformData::PassUniform()
	{
		if (!m_pUniformData) { GameLogger::Log(MessageType::cError, "PassUniform called but data address is nullptr!\n"); return false; }
		if (m_uniformDataLoc <= 0) { GameLogger::Log(MessageType::cError, "PassUniform called but data location was less than or equal to zero!\n"); return false; }

		// TODO: INVESTIGATE SUPERIOR WAY OF ACHIEVING THIS!!!
		Mat4 result;

		switch (m_uniformType)
		{
		case UniformType::UNIFORM1I:
			glUniform1i(m_uniformDataLoc, *reinterpret_cast<int*>(m_pUniformData));
			break;

		case UniformType::UNIFORM1F:
			glUniform1f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData));
			break;

		case UniformType::UNIFORM2F:
			glUniform2f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1));
			break;

		case UniformType::UNIFORM3F:
			glUniform3f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), *(reinterpret_cast<float*>(m_pUniformData) + 2));
			break;

		case UniformType::UNIFORMMAT4FV:
			glUniformMatrix4fv(m_uniformDataLoc, 1, GL_FALSE, reinterpret_cast<float*>(m_pUniformData));
			break;

		case UniformType::MATRIXARRAY:
			result = MultiplyFromMatArray();
			glUniformMatrix4fv(m_uniformDataLoc, 1, GL_FALSE, result.GetAddress());
			break;

		case UniformType::MATRIXGETTER:
			result = reinterpret_cast<MatrixGetterCallback>(m_pUniformData)();
			glUniformMatrix4fv(m_uniformDataLoc, 1, GL_FALSE, result.GetAddress());
			break;

		case UniformType::MATRIXGETTERARRAY:
			result = MultiplyFromMatGetterArray();
			glUniformMatrix4fv(m_uniformDataLoc, 1, GL_FALSE, result.GetAddress());
			break;

		default:
			GameLogger::Log(MessageType::cError, "Unknown uniform data type [%d]!\n", m_uniformType);
			return false;
		}

		return true;
	}

	Mat4 UniformData::MultiplyFromMatArray()
	{
		if (m_numMatrices <= 0) { GameLogger::Log(MessageType::cError, "Invalid number of matrices passed!\n"); return Mat4(); }
		Mat4 **pMatrices = reinterpret_cast<Mat4**>(m_pUniformData);
		Mat4 result;

		for (int i = 0; i < m_numMatrices; ++i)
		{
			result = result * **(pMatrices + i); //TODO: IF EVERYTHING EXPLODES REVERSE ORDER HERE!!!!!!!!!!!!!
		}

		return result;
	}

	Mat4 UniformData::MultiplyFromMatGetterArray()
	{
		if (m_numMatrices <= 0) { GameLogger::Log(MessageType::cError, "Invalid number of matrix getters passed!\n"); return Mat4(); }
		MatrixGetterCallback *pMatrices = (MatrixGetterCallback*)(m_pUniformData);
		Mat4 result;

		for (int i = 0; i < m_numMatrices; ++i)
		{
			MatrixGetterCallback currentCallback = (*(pMatrices + i));
			if (!currentCallback) { GameLogger::Log(MessageType::cError, "Callback for matrix was nullptr!\n"); return Mat4(); }
			result = result * (currentCallback()); //TODO: IF EVERYTHING EXPLODES REVERSE ORDER HERE!!!!!!!!!!!!!
		}

		return result;
	}
}


