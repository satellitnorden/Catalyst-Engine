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

		//The data.
		Texture2D<Vector4<uint8>> _Data;

	};

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The frames.
	DynamicArray<Frame> _Frames;


};