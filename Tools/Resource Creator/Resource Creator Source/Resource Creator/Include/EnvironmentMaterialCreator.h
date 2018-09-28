#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/CPUTextureCube.h>

//Resources
#include <Resources/ResourcesCore.h>

//Systems.
#include <Systems/TaskSystem.h>

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

	class EnvironmentMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The file path.
		const char *RESTRICT _File;

		//The diffuse resolution.
		uint32 _DiffuseResolution;

		//The diffuse irradiance resolution.
		uint32 _DiffuseIrradianceResolution;

	};

	/*
	*	Creates an environment material resource file.
	*/
	static void CreateEnvironmentMaterial(const EnvironmentMaterialCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::EnvironmentMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Load the texture.
		int32 width, height, numberOfChannels;
		float *const RESTRICT data{ stbi_loadf(parameters._File, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Wrap the data into a cpu texture for easier manipulation.
		CPUTexture2D hdrTexture{ static_cast<uint64>(width), static_cast<uint64>(height) };

		//Copy the data into the cpu texture.
		MemoryUtilities::CopyMemory(hdrTexture.Data(), data, width * height * 4 * sizeof(float));

		//Create the diffuse output textures.
		StaticArray<CPUTexture2D, 6> diffuseOutputTextures
		{
			CPUTexture2D(parameters._DiffuseResolution),
			CPUTexture2D(parameters._DiffuseResolution),
			CPUTexture2D(parameters._DiffuseResolution),
			CPUTexture2D(parameters._DiffuseResolution),
			CPUTexture2D(parameters._DiffuseResolution),
			CPUTexture2D(parameters._DiffuseResolution)
		};

		StaticArray<DiffuseLayerCreationParameters, 6> diffuseLayerCreationParameters
		{
			DiffuseLayerCreationParameters(0, parameters._DiffuseResolution, diffuseOutputTextures[0], hdrTexture),
			DiffuseLayerCreationParameters(1, parameters._DiffuseResolution, diffuseOutputTextures[1], hdrTexture),
			DiffuseLayerCreationParameters(2, parameters._DiffuseResolution, diffuseOutputTextures[2], hdrTexture),
			DiffuseLayerCreationParameters(3, parameters._DiffuseResolution, diffuseOutputTextures[3], hdrTexture),
			DiffuseLayerCreationParameters(4, parameters._DiffuseResolution, diffuseOutputTextures[4], hdrTexture),
			DiffuseLayerCreationParameters(5, parameters._DiffuseResolution, diffuseOutputTextures[5], hdrTexture),
		};

		//Calculate the diffuse.
		for (uint8 i = 0; i < 6; ++i)
		{
			CreateDiffuseLayer(diffuseLayerCreationParameters[i]);
		}

		//Create the diffuse texture.
		CPUTextureCube diffuseTexture{ diffuseOutputTextures };

		//Create the diffuse irradiance output textures.
		StaticArray<CPUTexture2D, 6> diffuseIrradianceOutputTextures
		{
			CPUTexture2D(parameters._DiffuseIrradianceResolution),
			CPUTexture2D(parameters._DiffuseIrradianceResolution),
			CPUTexture2D(parameters._DiffuseIrradianceResolution),
			CPUTexture2D(parameters._DiffuseIrradianceResolution),
			CPUTexture2D(parameters._DiffuseIrradianceResolution),
			CPUTexture2D(parameters._DiffuseIrradianceResolution)
		};

		StaticArray<IrradianceLayerCreationParameters, 6> diffuseIrradianceLayerCreationParameters
		{
			IrradianceLayerCreationParameters(0, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[0], diffuseTexture),
			IrradianceLayerCreationParameters(1, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[1], diffuseTexture),
			IrradianceLayerCreationParameters(2, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[2], diffuseTexture),
			IrradianceLayerCreationParameters(3, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[3], diffuseTexture),
			IrradianceLayerCreationParameters(4, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[4], diffuseTexture),
			IrradianceLayerCreationParameters(5, parameters._DiffuseIrradianceResolution, diffuseIrradianceOutputTextures[5], diffuseTexture),
		};

		//Calculate the diffuse.
		for (uint8 i = 0; i < 6; ++i)
		{
			CreateDiffuseIrradianceLayer(diffuseIrradianceLayerCreationParameters[i]);
		}

		//Write the diffuse output resolution to the file.
		file.Write(&parameters._DiffuseResolution, sizeof(uint32));

		//Write the diffuse to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseOutputTextures[i].Data(), parameters._DiffuseResolution * parameters._DiffuseResolution * 4 * sizeof(float));
		}

		//Write the diffuse irradiance output resolution to the file.
		file.Write(&parameters._DiffuseIrradianceResolution, sizeof(uint32));

		//Write the diffuse irradiance to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseIrradianceOutputTextures[i].Data(), parameters._DiffuseIrradianceResolution * parameters._DiffuseIrradianceResolution * 4 * sizeof(float));
		}

		//Close the file.
		file.Close();
	}

private:

	/*
	*	Definition for the diffuse layer creation parameters.
	*/
	class DiffuseLayerCreationParameters final
	{

	public:

		DiffuseLayerCreationParameters(const uint8 initialIndex, const uint32 initialOutputResolution, CPUTexture2D &initialOutputTexture, const CPUTexture2D &initialHdrTexture) NOEXCEPT
			:
			index(initialIndex),
			outputResolution(initialOutputResolution),
			outputTexture(initialOutputTexture),
			hdrTexture(initialHdrTexture)
		{

		}

		const uint8 index;
		const uint32 outputResolution;
		CPUTexture2D &outputTexture;
		const CPUTexture2D &hdrTexture;

	};

	/*
	*	Definition for the irradiance layer creation parameters.
	*/
	class IrradianceLayerCreationParameters final
	{

	public:

		IrradianceLayerCreationParameters(const uint8 initialIndex, const uint32 initialOutputResolution, CPUTexture2D &initialOutputTexture, const CPUTextureCube &initialDiffuseTexture) NOEXCEPT
			:
			index(initialIndex),
			outputResolution(initialOutputResolution),
			outputTexture(initialOutputTexture),
			diffuseTexture(initialDiffuseTexture)
		{

		}

		const uint8 index;
		const uint32 outputResolution;
		CPUTexture2D &outputTexture;
		const CPUTextureCube &diffuseTexture;

	};

	/*
	*	Given an index and two weights, return the corresponding position vector.
	*/
	static Vector3 GetPositionVector(const uint8 index, const float xWeight, const float yWeight) NOEXCEPT
	{
		switch (index)
		{
			default: return Vector3();
			case 0: return Vector3(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight));
			case 1: return Vector3(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight));
			case 2: return Vector3(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, yWeight));
			case 3: return Vector3(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight));
			case 4: return Vector3(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), -1.0f);
			case 5: return Vector3(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), 1.0f);
		}
	}

	/*
	*	Creates one layer of the diffuse textures.
	*/
	static void CreateDiffuseLayer(const DiffuseLayerCreationParameters &parameters) NOEXCEPT
	{
		for (uint32 j = 0; j < parameters.outputResolution; ++j)
		{
			for (uint32 k = 0; k < parameters.outputResolution; ++k)
			{
				Vector3 position{ GetPositionVector(parameters.index, static_cast<float>(j) / static_cast<float>(parameters.outputResolution), static_cast<float>(k) / static_cast<float>(parameters.outputResolution)) };
				position.Normalize();

				Vector2 textureCoordinate{ CatalystBaseMath::ArctangentRadians(position._Z, position._X), CatalystBaseMath::ArcsineRadians(position._Y) };
				textureCoordinate *= EnvironmentMaterialCreatorConstants::INVERSE_ATAN;
				textureCoordinate += 0.5f;

				parameters.outputTexture.At(j, k) = parameters.hdrTexture.At(textureCoordinate);
			}
		}
	}

	/*
	*	Creates one layer of the diffuse irradiance textures.
	*/
	static void CreateDiffuseIrradianceLayer(const IrradianceLayerCreationParameters &parameters) NOEXCEPT
	{
		for (uint32 j = 0; j < parameters.outputResolution; ++j)
		{
			for (uint32 k = 0; k < parameters.outputResolution; ++k)
			{
				Vector3 direction{ GetPositionVector(parameters.index, static_cast<float>(j) / static_cast<float>(parameters.outputResolution), static_cast<float>(k) / static_cast<float>(parameters.outputResolution)) };
				direction *= -1.0f;

				Vector3 rightVector{ Vector3::CrossProduct(Vector3(0.0f, 1.0f, 0.0f), direction) };
				Vector3 upVector{ Vector3::CrossProduct(direction, rightVector) };

				float sampleDelta{ 0.025f };
				float numberOfSamples{ 0.0f };

				Vector3 finalIrradiance{ 0.0f, 0.0f, 0.0f };

				for (float phi = 0.0f; phi < (2.0f * CatalystBaseMathConstants::PI); phi += sampleDelta)
				{
					for (float theta = 0.0f; theta < (0.5f * CatalystBaseMathConstants::PI); theta += sampleDelta)
					{
						Vector3 tangentSample{ Vector3(CatalystBaseMath::SineRadians(theta) * CatalystBaseMath::CosineRadians(phi), CatalystBaseMath::SineRadians(theta) * CatalystBaseMath::SineRadians(phi), CatalystBaseMath::CosineRadians(theta)) };
						Vector3 sampleVector{ tangentSample._X * rightVector + tangentSample._Y * upVector + tangentSample._Z * direction };

						Vector4 sampledValue{ parameters.diffuseTexture.At(sampleVector) };

						finalIrradiance += Vector3(sampledValue._X, sampledValue._Y, sampledValue._Z) * CatalystBaseMath::CosineRadians(theta) * CatalystBaseMath::SineRadians(theta);

						numberOfSamples += 1.0f;
					}
				}

				parameters.outputTexture.At(j, k) = CatalystBaseMathConstants::PI * finalIrradiance * (1.0f / numberOfSamples);
			}
		}
	}

};