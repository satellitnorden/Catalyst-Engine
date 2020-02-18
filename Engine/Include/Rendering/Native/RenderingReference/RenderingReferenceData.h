#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

//User interface.
#include <UserInterface/TextUserInterfaceElement.h>

class RenderingReferenceData final
{

public:

	/*
	*	Asynchronous data class definition.
	*/
	class AsynchronousData final
	{

	public:

		//The task.
		Task _Task;

		//The start Y coordinate.
		uint32 _StartY;

		//The end Y coordinate.
		uint32 _EndY;

	};

	//The rendering reference texture.
	Texture2D<Vector4<float>> _RenderingReferenceTexture;

	//The rendering reference texture handle.
	Texture2DHandle _RenderingReferenceTextureHandle{ EMPTY_HANDLE };

	//The rendering reference texture index.
	uint32 _RenderingReferenceTextureIndex;

	//The number of iterations.
	uint32 _Iterations{ 0 };

	//Container for all the asynchronous data.
	DynamicArray<AsynchronousData> _AsynchronousData;

	//Keeps track of the number of texels calculated.
	std::atomic<uint64> _TexelsCalculated;

	//The progress information.
	TextUserInterfaceElement *RESTRICT _ProgressInformation;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE RenderingReferenceData() NOEXCEPT
	{
	
	}

};