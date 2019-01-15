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
	bool GenerateTransformation(const TransformationGenerationProperties &properties) NOEXCEPT
	{
		//Generate a random position.
		Vector3<float> position{	CatalystBaseMath::RandomFloatInRange(properties._AxisAlignedBoundingBox->_Minimum._X, properties._AxisAlignedBoundingBox->_Maximum._X),
									0.0f,
									CatalystBaseMath::RandomFloatInRange(properties._AxisAlignedBoundingBox->_Minimum._Z, properties._AxisAlignedBoundingBox->_Maximum._Z) };

		//Get the terrain height.
		if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y))
		{
			return false;
		}

		//Get the terrain normal.
		Vector3<float> terrainNormal;

		if (!TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &terrainNormal))
		{
			return false;
		}

		//Handle spawning based on angle.
		const float angle{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(terrainNormal, Vector3<float>::UP), 0.0f) };

		if (angle < properties._MinimumAngle)
		{
			return false;
		}

		const float scaledAngle{ CatalystBaseMath::Scale(angle, properties._MinimumAngle, 1.0f, 0.0f, 1.0f) };

		if (!CatalystBaseMath::RandomChance(scaledAngle))
		{
			return false;
		}

		//Generate the transformation!
		const float scale{ CatalystBaseMath::RandomFloatInRange(properties._MinimumScale, properties._MaximumScale) };

		properties._Transformation->SetTranslation(position);
		properties._Transformation->SetScale(Vector3<float>(scale, scale, scale));

		Matrix4 rotationMatrix;

		if (terrainNormal != Vector3<float>::UP)
		{
			rotationMatrix = Matrix4::Orientation(terrainNormal, Vector3<float>::UP);
		}

		rotationMatrix.Rotate(properties._Rotation);

		*properties._Transformation = *properties._Transformation * rotationMatrix;

		return true;
	}

}