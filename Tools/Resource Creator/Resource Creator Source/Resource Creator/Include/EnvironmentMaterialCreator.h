#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Math.
#include <Math/CatalystMath.h>
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
		const HashString resourceID{ arguments[3] };
		file.Write(&resourceID, sizeof(HashString));

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

		//Create the output textures.
		StaticArray<CPUTexture4, 6> outputTextures
		{
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution)
		};

		//Calculate the diffuse.
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

					outputTextures[i].At(j, k) = hdrTexture.At(textureCoordinate);
				}
			}
		}
		
		//Write the diffuse to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(outputTextures[i].Data(), outputResolution * outputResolution * 4 * sizeof(float));
		}

		//Calculate the diffuse irradiance.
		for (uint8 i = 0; i < 6; ++i)
		{
			for (uint32 j = 0; j < outputResolution; ++j)
			{
				for (uint32 k = 0; k < outputResolution; ++k)
				{
					Vector3 finalIrradiance{ 0.0f, 0.0f, 0.0f };
					Vector3 position{ GetPositionVector(i, static_cast<float>(j) / static_cast<float>(outputResolution), static_cast<float>(k) / static_cast<float>(outputResolution)) };
					Vector3 normalDirection{ position };

					Vector3 rightVector{ Vector3::CrossProduct(Vector3(0.0f, 1.0f, 0.0f), normalDirection) };
					Vector3 upVector{ Vector3::CrossProduct(normalDirection, rightVector) };

					float sampleDelta{ 0.025f };
					float numberOfSamples{ 0.0f };

					for (float phi = 0.0f; phi < (2.0f * CatalystMathConstants::PI); phi += sampleDelta)
					{
						for (float theta = 0.0f; theta < (0.5f * CatalystMathConstants::PI); theta += sampleDelta)
						{
							Vector3 tangentSample{ Vector3(CatalystMath::SineRadians(theta) * CatalystMath::CosineRadians(phi), CatalystMath::SineRadians(theta) * CatalystMath::SineRadians(phi), CatalystMath::CosineRadians(theta)) };
							Vector3 sampleVector{ tangentSample.X * rightVector + tangentSample.Y * upVector + tangentSample.Z * normalDirection };

							Vector2 textureCoordinate{ CatalystMath::ArctangentRadians(sampleVector.Z, sampleVector.X), CatalystMath::ArcsineRadians(sampleVector.Y) };
							textureCoordinate *= EnvironmentMaterialCreatorConstants::INVERSE_ATAN;
							textureCoordinate += 0.5f;

							Vector4 sampledValue{ hdrTexture.At(textureCoordinate) };

							finalIrradiance += Vector3(sampledValue.X, sampledValue.Y, sampledValue.Z) * CatalystMath::CosineRadians(theta) * CatalystMath::SineRadians(theta);

							numberOfSamples += 1.0f;
						}
					}

					outputTextures[i].At(j, k) = CatalystMathConstants::PI * finalIrradiance * (1.0f / numberOfSamples);
				}
			}
		}

		//Write the diffuse irradiance to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(outputTextures[i].Data(), outputResolution * outputResolution * 4 * sizeof(float));
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