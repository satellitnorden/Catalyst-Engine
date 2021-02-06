#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/Resource.h>

class VideoResource final : public Resource
{

public:

	/*
	*	Frame class definition.
	*/
	class Frame final
	{

	public:

		//The data X.
		Texture2D<uint8> _DataX;

		//The data Y.
		Texture2D<uint8> _DataY;

		//The data Z.
		Texture2D<uint8> _DataZ;

	};

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The frames per second.
	float32 _FramesPerSecond;

	//The frames per second reciprocal.
	float32 _FramesPerSecondReciprocal;

	//The frames.
	DynamicArray<Frame> _Frames;


};