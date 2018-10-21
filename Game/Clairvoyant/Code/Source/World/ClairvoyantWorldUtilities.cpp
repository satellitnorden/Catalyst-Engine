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
	bool GenerateTransformation(const bool grass, const bool sand, const bool snow, const bool underwater, const float minimumAngle, const float height, const float minimumScale, const float maximumScale, const Vector3 &rotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
	{
		//Generate a random position.
		Vector3 position(CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X), 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z));

		//Get the terrain height.
		if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y))
		{
			return false;
		}

		//Handle spawning on grass.
		if (!grass)
		{
			if (position._Y > ClairvoyantWorldConstants::SAND_BLEND_END
				&& position._Y < ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
			{
				return false;
			}

			else if (position._Y < ClairvoyantWorldConstants::SAND_BLEND_END)
			{
				if (!sand)
				{
					return false;
				}

				else if (position._Y > ClairvoyantWorldConstants::SAND_BLEND_BEGIN)
				{
					if (CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SAND_BLEND_BEGIN) / (ClairvoyantWorldConstants::SAND_BLEND_END - ClairvoyantWorldConstants::SAND_BLEND_BEGIN)))
					{
						return false;
					}
				}
			}

			else if (position._Y > ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
			{
				if (!snow)
				{
					return false;
				}

				else if (position._Y < ClairvoyantWorldConstants::SNOW_BLEND_END)
				{
					if (1.0f - CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) / (ClairvoyantWorldConstants::SNOW_BLEND_END - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)))
					{
						return false;
					}
				}
			}
		}

		//Handle spawning on sand.
		if (!sand)
		{
			if (position._Y < ClairvoyantWorldConstants::SAND_BLEND_BEGIN)
			{
				return false;
			}

			else if (position._Y < ClairvoyantWorldConstants::SAND_BLEND_END)
			{
				if (CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SAND_BLEND_BEGIN) / (ClairvoyantWorldConstants::SAND_BLEND_END - ClairvoyantWorldConstants::SAND_BLEND_BEGIN)))
				{
					return false;
				}
			}
		}

		//Handle spawning on snow.
		if (!snow)
		{
			if (position._Y > ClairvoyantWorldConstants::SNOW_BLEND_END)
			{
				return false;
			}

			else if (position._Y > ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)
			{
				if (CatalystBaseMath::RandomChance((position._Y - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN) / (ClairvoyantWorldConstants::SNOW_BLEND_END - ClairvoyantWorldConstants::SNOW_BLEND_BEGIN)))
				{
					return false;
				}
			}
		}

		//Handle spawning underwater.
		if (!underwater)
		{
			if (position._Y < 0.0f)
			{
				return false;
			}
		}

		//Get the terrain normal.
		Vector3 terrainNormal;

		if (!TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &terrainNormal))
		{
			return false;
		}

		//Handle spawning based on angle.
		const float angle{ CatalystBaseMath::Maximum<float>(Vector3::DotProduct(terrainNormal, Vector3::UP), 0.0f) };

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
		transformation->SetScale(Vector3(scale, scale, scale));

		Matrix4 rotationMatrix{ Matrix4::Orientation(terrainNormal, Vector3::UP) };
		rotationMatrix.Rotate(rotation);

		*transformation = *transformation * rotationMatrix;

		return true;
	}

}