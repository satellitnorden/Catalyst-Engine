//Header file.
#include <World/ClairvoyantVegetationPlacement.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

//Systems.
#include <Systems/TerrainSystem.h>

namespace ClairvoyantVegetationPlacement
{

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const TransformationGenerationProperties &properties, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
	{
		//Generate a random position.
		Vector3<float> position{	CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X),
									0.0f,
									CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z) };

		//Retrieve the height, normal and material of the terrain.
		float height;
		Vector3<float> normal;
		uint8 material;

		if (!TerrainSystem::Instance->GetTerrainMaterialAtPosition(position, &material, &height, &normal))
		{
			return false;
		}

		//Test the material.
		float materialWeight{ 0.0f };

		for (const TerrainMaterialWeight terrainMaterialWeight : properties._MaterialWeights)
		{
			if (terrainMaterialWeight._Material == static_cast<ClairvoyantTerrainMaterial>(material))
			{
				materialWeight = terrainMaterialWeight._Weight;
			}
		}

		if (materialWeight == 0.0f)
		{
			return false;
		}

		if (materialWeight < 1.0f && !CatalystBaseMath::RandomChance(materialWeight))
		{
			return false;
		}

		//Set the height.
		position._Y = height;

		//Generate the transformation!
		const float scale{ CatalystBaseMath::RandomFloatInRange(properties._MinimumScale, properties._MaximumScale) };

		transformation->SetTranslation(position);
		transformation->SetScale(Vector3<float>(scale, scale, scale));

		Matrix4 rotationMatrix{ Matrix4::Orientation(normal, Vector3<float>::UP) };

		rotationMatrix.Rotate(properties._Rotation);

		*transformation = *transformation * rotationMatrix;

		return true;
	}

}