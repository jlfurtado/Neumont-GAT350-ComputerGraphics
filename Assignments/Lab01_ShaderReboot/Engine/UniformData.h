#ifndef UNIFORMDATA_H
#define UNIFORMDATA_H

// Justin Furtado
// UniformData.h
// 10/28/2016
// Stores data needed to make uniform calls!

#include "UniformType.h"
#include "Mat4.h"
#include "ExportHeader.h"

namespace Engine
{
	typedef Mat4(*MatrixGetterCallback)();

	class ENGINE_SHARED UniformData
	{
	public:
		UniformData();
		UniformData(UniformType type, void *dataAddress, int dataLoc, int numElements);
		bool PassUniform();

	private:
		Mat4 MultiplyFromMatArray();
		Mat4 MultiplyFromMatGetterArray();
		int m_numMatrices;
		int m_uniformDataLoc;
		void *m_pUniformData;
		UniformType m_uniformType;
	};
}

#endif // ifndef UNIFORMDATA_H