#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

/*
*	A cube texture that can be manipulated on the CPU.
*/
class CPUTextureCube final
{

public:

	/*
	*	Default constructor.
	*/
	CPUTextureCube() NOEXCEPT
	{

	}

	/*
	*	Constructor taking a static array of all the data.
	*/
	CPUTextureCube(const StaticArray<CPUTexture2D, 6> &initialData) NOEXCEPT
		:
		_Data(initialData)
	{

	}

	/*
	*	Returns the value at the specified direction.
	*/
	Vector4 At(const Vector3 &direction) const NOEXCEPT
	{
		//Determine the texture coordinate and the face index.
		uint8 faceIndex;
		Vector2 textureCoordinate;

		CubeSampling(direction, faceIndex, textureCoordinate);

		//Return the sampled texture.
		return _Data[faceIndex].At(textureCoordinate);
	}

private:

	//The underlying data.
	StaticArray<CPUTexture2D, 6> _Data;

	/*
	*	Function to determine the face index as well as the texture coordinate from a direction vector.
	*/
	void CubeSampling(const Vector3 &direction, uint8 &faceIndex, Vector2 &textureCoordinate) const NOEXCEPT
	{
		const Vector3 absoluteDirection{ Vector3::Absolute(direction) };

		float ma;

		if (absoluteDirection._Z >= absoluteDirection._X && absoluteDirection._Z >= absoluteDirection._Y)
		{
			faceIndex = direction._Z < 0.0f ? 5 : 4;
			ma = 0.5f / absoluteDirection._Z;
			textureCoordinate = Vector2(direction._Z < 0.0f ? -direction._X : direction._X, -direction._Y);
		}

		else if (absoluteDirection._Y >= absoluteDirection._X)
		{
			faceIndex = direction._Y < 0.0f ? 3 : 2;
			ma = 0.5f / absoluteDirection._Y;
			textureCoordinate = Vector2(direction._X, direction._Y < 0.0f ? -direction._Z : direction._Z);
		}

		else
		{
			faceIndex = direction._X < 0.0f ? 1 : 0;
			ma = 0.5f / absoluteDirection._X;
			textureCoordinate = Vector2(direction._X < 0.0f ? direction._Z : -direction._Z, -direction._Y);
		}

		textureCoordinate = textureCoordinate * ma + 0.5f;
	}

};