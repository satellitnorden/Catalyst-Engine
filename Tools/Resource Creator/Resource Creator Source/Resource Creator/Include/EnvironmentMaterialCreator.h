#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture4.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

//Environment material creator constants.
namespace EnvironmentMaterialCreatorConstants
{
	constexpr Vector2 INVERSE_ATAN{ 0.1591f, 0.3183f };
}

class EnvironmentMaterialCreator final
{

public:

	/*
	*	Creates an environment material resource file.
	*/
	static void CreateEnvironmentMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::EnvironmentMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const ResourceID resourceID{ std::strtoull(arguments[3], nullptr, 0) };
		file.Write(&resourceID, sizeof(ResourceID));

		//Load the texture.
		int32 width, height, numberOfChannels;
		float *const RESTRICT data{ stbi_loadf(arguments[4], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Wrap the data into a cpu texture for easier manipulation.
		CPUTexture4 hdrTexture{ static_cast<uint64>(width), static_cast<uint64>(height) };

		//Copy the data into the cpu texture.
		MemoryUtilities::CopyMemory(hdrTexture.Data(), data, width * height * 4 * sizeof(float));

		//Get the resolution of the output texture(s).
		const uint32 outputResolution{ std::strtoul(arguments[5], nullptr, 0) };

		//Write the output resolution to the file.
		file.Write(&outputResolution, sizeof(uint32));

		//Create the diffuse output textures.
		StaticArray<CPUTexture4, 6> diffuseOutputTextures
		{
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution)
		};

		for (uint8 i = 0; i < 6; ++i)
		{
			for (uint32 j = 0; j < outputResolution; ++j)
			{
				for (uint32 k = 0; k < outputResolution; ++k)
				{
					Vector3 position{ GetPositionVector(i, static_cast<float>(j) / static_cast<float>(outputResolution), static_cast<float>(k) / static_cast<float>(outputResolution)) };
					position.Normalize();

					Vector2 textureCoordinate{ CatalystMath::ArctangentRadians(position.Z, position.X), CatalystMath::ArcsineRadians(position.Y) };
					textureCoordinate *= EnvironmentMaterialCreatorConstants::INVERSE_ATAN;
					textureCoordinate += 0.5f;

					diffuseOutputTextures[i].At(j, k) = hdrTexture.At(textureCoordinate);
				}
			}
		}
		
		//Write the diffuse to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseOutputTextures[i].Data(), outputResolution * outputResolution * 4 * sizeof(float));
		}

		//Close the file.
		file.Close();
	}

private:

	/*
	*	Given an index and two weights, return the corresponding position vector.
	*/
	static Vector3 GetPositionVector(const uint8 index, const float xWeight, const float yWeight) NOEXCEPT
	{
		switch (index)
		{
			default: return Vector3();
			case 0: return Vector3(-1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight));
			case 1: return Vector3(1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight));
			case 2: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), -1.0f, CatalystMath::LinearlyInterpolate(1.0f, -1.0f, yWeight)); //Up.
			case 3: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), 1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight)); //Down.
			case 4: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), -1.0f);
			case 5: return Vector3(CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), 1.0f);
		}
	}

};