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

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData(const Matrix4 &initialTransform, const AccelerationStructureHandle &initialBottomLevelAccelerationStructure) NOEXCEPT
		:
		_Transform(initialTransform),
		_BottomLevelAccelerationStructure(initialBottomLevelAccelerationStructure)
	{

	}

};