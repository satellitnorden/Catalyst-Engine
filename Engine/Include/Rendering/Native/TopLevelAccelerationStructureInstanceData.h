#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TopLevelAccelerationStructureInstanceData final
{

public:

	//The transform.
	Matrix4 _Transform{ MatrixConstants::IDENTITY };

	//The bottom level acceleration structure.
	AccelerationStructureHandle _BottomLevelAccelerationStructure{ EMPTY_HANDLE };

	//The index.
	uint64 _Index{ UINT64_MAXIMUM };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData(const Matrix4 &initialTransform, const AccelerationStructureHandle &initialBottomLevelAccelerationStructure, const uint64 initialIndex) NOEXCEPT
		:
		_Transform(initialTransform),
		_BottomLevelAccelerationStructure(initialBottomLevelAccelerationStructure),
		_Index(initialIndex)
	{

	}

};