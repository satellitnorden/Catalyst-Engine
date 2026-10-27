#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Path tracing.
#include <PathTracing/Material.h>

namespace PathTracing
{

	/*
	*	Class holding data about a hit surface.
	*/
	class SurfaceInformation final
	{

	public:

		///////////////////
		// GEOMETRY DATA //
		///////////////////

		//The hit distance.
		float32 _HitDistance{ FLOAT32_MAXIMUM };

		//The hit position.
		Vector3<float32> _HitPosition;

		//The geometry normal.
		Vector3<float32> _GeometryNormal;

		//The tangent.
		Vector3<float32> _Tangent;

		//The texture coordinate.
		Vector2<float32> _TextureCoordinate;

		///////////////////
		// MATERIAL DATA //
		///////////////////

		//The material.
		const PathTracing::Material *RESTRICT _Material;

		//The albedo.
		Vector3<float32> _Albedo;

		//The shading normal.
		Vector3<float32> _ShadingNormal;

		//The roughness.
		float32 _Roughness;

		//The metallic.
		float32 _Metallic;

		/*
		*	Fills in the material data from the given material.
		*/
		FORCE_INLINE void FillInMaterialData(const PathTracing::Material *const RESTRICT material) NOEXCEPT
		{
			//Set the material.
			_Material = material;

			//Fill in the albedo.
			_Albedo = material->SampleAlbedo(_TextureCoordinate);

			//Calculate the shading normal.
			{
				//Construct the tangent space matrix.
				const Matrix3x3 tangent_space_matrix
				{
					_Tangent,
					Vector3<float32>::CrossProduct(_GeometryNormal, _Tangent),
					_GeometryNormal
				};

				//Sample the normal map.
				const Vector3<float32> normal_map{ material->SampleNormalMap(_TextureCoordinate) * 2.0f - 1.0f };

				//Fill in the shading normal.
				_ShadingNormal = tangent_space_matrix * normal_map;
			}

			//Fill in the roughness.
			_Roughness = material->SampleRoughness(_TextureCoordinate);

			//Fill in the metallic.
			_Metallic = material->SampleMetallic(_TextureCoordinate);
		}

	};
}