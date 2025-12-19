#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	/*
	*	Render commands for UI.
	*	This is the struct that gets sent to the GPU.
	*/
	class ALIGN(16) RenderCommand final
	{

	public:

		//Enumeration covering all modes.
		enum class Mode : uint32
		{
			NONE,

			COLOR,
			TEXTURE,

			TEXT
		};

		/*
		*	The positions.
		*	Not a simple bounding box to better handle rotated boxes and such.
		*/
		StaticArray<Vector4<float32>, 4> _Positions;

		//The texture coordinates.
		StaticArray<Vector2<float32>, 4> _TextureCoordinates;

		//The mode.
		Mode _Mode;

		//The color/texture.
		uint32 _ColorOrTexture;

		//The color/opacity.
		uint32 _ColorOpacity;

		/*
		*	Parameter 1.
		*	Radius (rounded corners) for image.
		*	None for texture.
		*	Smoothing factor for text.
		*/
		union
		{
			uint32 _Parameter1_uint32;
			float32 _Parameter1_float32;
		};

	};

}