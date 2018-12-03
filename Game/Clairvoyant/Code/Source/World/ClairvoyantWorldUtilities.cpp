//Header file.
#include <World/ClairvoyantWorldUtilities.h>

//Clairvoyant.
#include <World/ClairvoyantWorldConstants.h>

//Systems.
#include <Systems/TerrainSystem.h>

namespace ClairvoyantWorldUtilities
{

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const bool grass, const bool sand, const bool snow, const bool underwater, const float minimumAngle, const float height, const float minimumScale, const float maximumScale, const Vector3<float> &rotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
	{
		//Generate a random position.
		Vector3<float> position(CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X), 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z));

		//Get the terrain height.
		if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y))
		{
			return false;
		}

		//Determine where it is spawning.
		if (position._Y < 0.0f)
		{
			//Spawning underwater.
			if (!underwater)
			{
				return false;
			}
		}

		else if (position._Y < ClairvoyantWorldConstants::SAND_BLEND_END)
		{
			//Spawning on sand.
			if (!sand)
			{
				return false;
			}
		}

		else if (	position._Y > ClairvoyantWorldConstants::SAND_BLEND_END
					&& position._Y < ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
		{
			//Spawning on grass.
			if (!grass)
			{
				return false;
			}
		}

		else
		{
			//Spawning on snow.
			if (!snow)
			{
				return false;
			}
		}

		//Get the terrain normal.
		Vector3<float> terrainNormal;

		if (!TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &terrainNormal))
		{
			return false;
		}

		//Handle spawning based on angle.
		const float angle{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(terrainNormal, Vector3<float>::UP), 0.0f) };

		if (angle < minimumAngle)
		{
			return false;
		}

		const float scaledAngle{ CatalystBaseMath::Scale(angle, minimumAngle, 1.0f, 0.0f, 1.0f) };

		if (!CatalystBaseMath::RandomChance(scaledAngle))
		{
			return false;
		}

		//Adjust the height.
		position += terrainNormal * height;

		//Generate the transformation!
		const float scale{ CatalystBaseMath::RandomFloatInRange(minimumScale, maximumScale) };

		transformation->SetTranslation(position);
		transformation->SetScale(Vector3<float>(scale, scale, scale));

		Matrix4 rotationMatrix{ Matrix4::Orientation(terrainNormal, Vector3<float>::UP) };
		rotationMatrix.Rotate(rotation);

		*transformation = *transformation * rotationMatrix;

		return true;
	}

}