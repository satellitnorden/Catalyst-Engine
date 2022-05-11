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
	*	Initializes this texture cube.
	*/
	FORCE_INLINE void Initialize(const uint32 resolution, const DynamicArray<float32> &data) NOEXCEPT
	{
		_Resolution = resolution;

		const byte *RESTRICT byte_pointer{ reinterpret_cast<const byte *RESTRICT>(data.Data()) };

		for (uint8 i{ 0 }; i < _Faces.Size(); ++i)
		{
			_Faces[i].Initialize(resolution, resolution, byte_pointer);

			byte_pointer += resolution * resolution * sizeof(Vector4<float32>);
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

	/*
	*	Returns the direction from the given face index and normalized coordinate.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float32> GetDirection(const uint8 face_index, const Vector2<float32> normalized_coordinate) NOEXCEPT
	{
		const Vector2<float32> converted_coordinate{ normalized_coordinate * Vector2<float32>(2.0f) - Vector2<float32>(1.0f) };

		switch (face_index)
		{
			case 0:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(1.0f, -converted_coordinate._Y, -converted_coordinate._X));
			}

			case 1:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(-1.0f, -converted_coordinate._Y, converted_coordinate._X));
			}

			case 2:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(converted_coordinate._X, 1.0f, converted_coordinate._Y));
			}

			case 3:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(converted_coordinate._X, -1.0f, -converted_coordinate._Y));
			}

			case 4:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(converted_coordinate._X, -converted_coordinate._Y, 1.0f));
			}

			case 5:
			{
				return Vector3<float32>::Normalize(Vector3<float32>(-converted_coordinate._X, -converted_coordinate._Y, -1.0f));
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return Vector3<float32>();
			}
		}
	}

private:

	/*
	*	The underlying faces.
	*
	*	Face 0: Right.
	*	Face 1: Left.
	*	Face 2: Up.
	*	Face 3: Down.
	*	Face 4: Forward.
	*	Face 5: Backward;
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