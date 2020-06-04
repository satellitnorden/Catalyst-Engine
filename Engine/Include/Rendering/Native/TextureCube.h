#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

class TextureCube final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TextureCube() NOEXCEPT
	{

	}

	/*
	*	Constructor taking a static array of all the faces.
	*/
	FORCE_INLINE TextureCube(const StaticArray<Texture2D<Vector4<float>>, 6> &initialFaces) NOEXCEPT
		:
		_Faces(initialFaces)
	{

	}

	/*
	*	Returns the texel at the given face index and coordinate.
	*/
	FORCE_INLINE NO_DISCARD Vector4<float32> &At(const uint8 face_index, const uint32 X, const uint32 Y) NOEXCEPT
	{
		return _Faces[face_index].At(X, Y);
	}

	/*
	*	Returns the face with the given index.
	*/
	FORCE_INLINE NO_DISCARD Texture2D<Vector4<float32>> &Face(const uint8 face_index) NOEXCEPT
	{
		return _Faces[face_index];
	}

	/*
	*	Initializes this texture cube.
	*/
	FORCE_INLINE void Initialize(const uint32 resolution) NOEXCEPT
	{
		_Resolution = resolution;

		for (uint8 i{ 0 }; i < _Faces.Size(); ++i)
		{
			_Faces[i].Initialize(resolution);
		}
	}

	/*
	*	Initializes this texture cube.
	*/
	FORCE_INLINE void Initialize(const uint32 resolution, const Vector4<float> *const RESTRICT data) NOEXCEPT
	{
		_Resolution = resolution;

		for (uint8 i{ 0 }; i < _Faces.Size(); ++i)
		{
			_Faces[i].Initialize(resolution, resolution, data + (_Resolution * _Resolution * i));
		}
	}

	/*
	*	Samples the value at the specified direction.
	*/
	FORCE_INLINE NO_DISCARD Vector4<float> Sample(const Vector3<float> &direction) const NOEXCEPT
	{
		//Determine the texture coordinate and the face index.
		Vector2<float> textureCoordinate;
		uint8 faceIndex;

		CubeSampling(direction, &textureCoordinate, &faceIndex);

		//Return the sampled face.
		return _Faces[faceIndex].Sample(textureCoordinate, AddressMode::CLAMP_TO_EDGE);
	}

private:

	/*
	*	The underlying faces.
	*
	*	Face 0: Front.
	*	Face 1: Back.
	*	Face 2: Up.
	*	Face 3: Down.
	*	Face 4: Right.
	*	Face 5: Left;
	*/
	StaticArray<Texture2D<Vector4<float32>>, 6> _Faces;

	//The resolution.
	uint32 _Resolution;

	/*
	*	Function to determine the face index as well as the texture coordinate from a direction vector.
	*/
	FORCE_INLINE void CubeSampling(const Vector3<float> &direction, Vector2<float> *const RESTRICT textureCoordinate, uint8 *const RESTRICT faceIndex) const NOEXCEPT
	{
		const Vector3<float> absoluteDirection{ Vector3<float>::Absolute(direction) };

		float ma;

		if (absoluteDirection._Z >= absoluteDirection._X && absoluteDirection._Z >= absoluteDirection._Y)
		{
			*faceIndex = direction._Z < 0.0f ? 5 : 4;
			ma = 0.5f / absoluteDirection._Z;
			*textureCoordinate = Vector2<float>(direction._Z < 0.0f ? -direction._X : direction._X, -direction._Y);
		}

		else if (absoluteDirection._Y >= absoluteDirection._X)
		{
			*faceIndex = direction._Y < 0.0f ? 3 : 2;
			ma = 0.5f / absoluteDirection._Y;
			*textureCoordinate = Vector2<float>(direction._X, direction._Y < 0.0f ? -direction._Z : direction._Z);
		}

		else
		{
			*faceIndex = direction._X < 0.0f ? 1 : 0;
			ma = 0.5f / absoluteDirection._X;
			*textureCoordinate = Vector2<float>(direction._X < 0.0f ? direction._Z : -direction._Z, -direction._Y);
		}

		*textureCoordinate = *textureCoordinate * ma + 0.5f;
	}

};