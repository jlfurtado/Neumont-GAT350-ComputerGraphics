#ifndef UNIFORMTYPE_H
#define UNIFORMTYPE_H

// Justin Furtado
// An enum that stores which method a uniform should call
// 10/28/2016
// UniformType.h

#include "ExportHeader.h"

namespace Engine
{
	enum class ENGINE_SHARED UniformType
	{
		UNIFORM1F,
		UNIFORM2F,
		UNIFORM3F,
		UNIFORMMAT4FV,
		UNIFORM1I,
		MATRIXARRAY,
		MATRIXGETTER,
		MATRIXGETTERARRAY
	};
}

#endif // ifndef UNIFORMTYPE_H