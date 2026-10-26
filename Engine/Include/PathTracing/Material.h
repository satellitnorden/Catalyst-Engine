#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

namespace PathTracing
{

	/*
	*	Class holding material data.
	*/
	class Material final
	{

	public:

		/*
		*	Sets the albedo value.
		*/
		FORCE_INLINE void SetAlbedoValue(const Vector3<float32> &value) NOEXCEPT
		{
			_AlbedoValue = value;
		}

		/*
		*	Sets the albedo texture.
		*/
		FORCE_INLINE void SetAlbedoTexture(const Texture2D<Vector3<byte>> &value) NOEXCEPT
		{
			_AlbedoTexture = value;
		}

		/*
		*	Sets the normal map value.
		*/
		FORCE_INLINE void SetNormalMapValue(const Vector3<float32> &value) NOEXCEPT
		{
			_NormalMapValue = value;
		}

		/*
		*	Sets the normal map texture.
		*/
		FORCE_INLINE void SetNormalMapTexture(const Texture2D<Vector3<byte>> &value) NOEXCEPT
		{
			_NormalMapTexture = value;
		}

		/*
		*	Sets the roughness value.
		*/
		FORCE_INLINE void SetRoughnessValue(const float32 value) NOEXCEPT
		{
			_RoughnessValue = value;
		}

		/*
		*	Sets the roughness texture.
		*/
		FORCE_INLINE void SetRoughnessTexture(const Texture2D<byte>& value) NOEXCEPT
		{
			_RoughnessTexture = value;
		}

		/*
		*	Sets the metallic value.
		*/
		FORCE_INLINE void SetMetallicValue(const float32 value) NOEXCEPT
		{
			_MetallicValue = value;
		}

		/*
		*	Sets the metallic texture.
		*/
		FORCE_INLINE void SetMetallicTexture(const Texture2D<byte> &value) NOEXCEPT
		{
			_MetallicTexture = value;
		}

		/*
		*	Samples the albedo.
		*/
		FORCE_INLINE NO_DISCARD Vector3<float32> SampleAlbedo(const Vector2<float32> texture_coordinate) const NOEXCEPT
		{
			//If we have a texture, use that.
			if (_AlbedoTexture.GetWidth() > 0 && _AlbedoTexture.GetHeight() > 0)
			{
				return SampleConvert(_AlbedoTexture, texture_coordinate);
			}

			//Otherwise, use the constant value.
			else
			{
				return _AlbedoValue;
			}
		}

		/*
		*	Samples the normal map.
		*/
		FORCE_INLINE NO_DISCARD Vector3<float32> SampleNormalMap(const Vector2<float32> texture_coordinate) const NOEXCEPT
		{
			//If we have a texture, use that.
			if (_NormalMapTexture.GetWidth() > 0 && _NormalMapTexture.GetHeight() > 0)
			{
				return SampleConvert(_NormalMapTexture, texture_coordinate);
			}

			//Otherwise, use the constant value.
			else
			{
				return _NormalMapValue;
			}
		}

		/*
		*	Samples the roughness.
		*/
		FORCE_INLINE NO_DISCARD float32 SampleRoughness(const Vector2<float32> texture_coordinate) const NOEXCEPT
		{
			//If we have a texture, use that.
			if (_RoughnessTexture.GetWidth() > 0 && _RoughnessTexture.GetHeight() > 0)
			{
				return SampleConvert(_RoughnessTexture, texture_coordinate);
			}

			//Otherwise, use the constant value.
			else
			{
				return _RoughnessValue;
			}
		}

		/*
		*	Samples the metallic.
		*/
		FORCE_INLINE NO_DISCARD float32 SampleMetallic(const Vector2<float32> texture_coordinate) const NOEXCEPT
		{
			//If we have a texture, use that.
			if (_MetallicTexture.GetWidth() > 0 && _MetallicTexture.GetHeight() > 0)
			{
				return SampleConvert(_MetallicTexture, texture_coordinate);
			}

			//Otherwise, use the constant value.
			else
			{
				return _MetallicValue;
			}
		}

	private:

		//The albedo value.
		Vector3<float32> _AlbedoValue{ 0.5f, 0.5f, 0.5f };

		//The albedo texture.
		Texture2D<Vector3<byte>> _AlbedoTexture;

		//The normal map value.
		Vector3<float32> _NormalMapValue{ 0.5f, 0.5f, 1.0f };

		//The normal map texture.
		Texture2D<Vector3<byte>> _NormalMapTexture;

		//The roughness value.
		float32 _RoughnessValue{ 1.0f };

		//The roughness texture.
		Texture2D<byte> _RoughnessTexture;

		//The metallic value.
		float32 _MetallicValue{ 0.0f };

		//The metallic texture.
		Texture2D<byte> _MetallicTexture;

	};

}