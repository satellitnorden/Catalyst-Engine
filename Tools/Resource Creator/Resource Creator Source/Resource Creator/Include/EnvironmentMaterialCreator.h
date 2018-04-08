#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/CPUTextureCube.h>

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
		CPUTexture2D hdrTexture{ static_cast<uint64>(width), static_cast<uint64>(height) };

		//Copy the data into the cpu texture.
		MemoryUtilities::CopyMemory(hdrTexture.Data(), data, width * height * 4 * sizeof(float));

		//Get the resolution of the diffuse output textures.
		const uint32 diffuseOutputResolution{ std::strtoul(arguments[5], nullptr, 0) };

		//Get the resolution of the diffuse irradiance output textures.
		const uint32 diffuseIrradianceOutputResolution{ std::strtoul(arguments[6], nullptr, 0) };

		//Create the diffuse output textures.
		StaticArray<CPUTexture2D, 6> diffuseOutputTextures
		{
			CPUTexture2D(diffuseOutputResolution),
			CPUTexture2D(diffuseOutputResolution),
			CPUTexture2D(diffuseOutputResolution),
			CPUTexture2D(diffuseOutputResolution),
			CPUTexture2D(diffuseOutputResolution),
			CPUTexture2D(diffuseOutputResolution)
		};

		StaticArray<DiffuseLayerCreationParameters, 6> diffuseLayerCreationParameters
		{
			DiffuseLayerCreationParameters(0, diffuseOutputResolution, diffuseOutputTextures[0], hdrTexture),
			DiffuseLayerCreationParameters(1, diffuseOutputResolution, diffuseOutputTextures[1], hdrTexture),
			DiffuseLayerCreationParameters(2, diffuseOutputResolution, diffuseOutputTextures[2], hdrTexture),
			DiffuseLayerCreationParameters(3, diffuseOutputResolution, diffuseOutputTextures[3], hdrTexture),
			DiffuseLayerCreationParameters(4, diffuseOutputResolution, diffuseOutputTextures[4], hdrTexture),
			DiffuseLayerCreationParameters(5, diffuseOutputResolution, diffuseOutputTextures[5], hdrTexture),
		};

		//Calculate the diffuse.
		StaticArray<Semaphore, 6> diffuseSemaphores;

		for (uint8 i = 0; i < 6; ++i)
		{
			TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
			{
				const DiffuseLayerCreationParameters &parameters{ *static_cast<DiffuseLayerCreationParameters *const RESTRICT>(arguments) };

				CreateDiffuseLayer(parameters);
			}, &diffuseLayerCreationParameters[i], &diffuseSemaphores[i]));
		}
		
		//Wait for the diffuse tasks to finish.
		for (uint8 i = 0; i < 6; ++i)
		{
			diffuseSemaphores[i].WaitFor();
		}

		//Create the diffuse texture.
		CPUTextureCube diffuseTexture{ diffuseOutputTextures };

		//Create the diffuse irradiance output textures.
		StaticArray<CPUTexture2D, 6> diffuseIrradianceOutputTextures
		{
			CPUTexture2D(diffuseIrradianceOutputResolution),
			CPUTexture2D(diffuseIrradianceOutputResolution),
			CPUTexture2D(diffuseIrradianceOutputResolution),
			CPUTexture2D(diffuseIrradianceOutputResolution),
			CPUTexture2D(diffuseIrradianceOutputResolution),
			CPUTexture2D(diffuseIrradianceOutputResolution)
		};

		StaticArray<IrradianceLayerCreationParameters, 6> diffuseIrradianceLayerCreationParameters
		{
			IrradianceLayerCreationParameters(0, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[0], diffuseTexture),
			IrradianceLayerCreationParameters(1, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[1], diffuseTexture),
			IrradianceLayerCreationParameters(2, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[2], diffuseTexture),
			IrradianceLayerCreationParameters(3, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[3], diffuseTexture),
			IrradianceLayerCreationParameters(4, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[4], diffuseTexture),
			IrradianceLayerCreationParameters(5, diffuseIrradianceOutputResolution, diffuseIrradianceOutputTextures[5], diffuseTexture),
		};

		//Calculate the diffuse.
		StaticArray<Semaphore, 6> diffuseIrradianceSemaphores;

		for (uint8 i = 0; i < 6; ++i)
		{
			TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
			{
				const IrradianceLayerCreationParameters &parameters{ *static_cast<IrradianceLayerCreationParameters *const RESTRICT>(arguments) };

				CreateDiffuseIrradianceLayer(parameters);
			}, &diffuseIrradianceLayerCreationParameters[i], &diffuseIrradianceSemaphores[i]));
		}

		//Wait for the irradiance tasks to finish.
		for (uint8 i = 0; i < 6; ++i)
		{
			diffuseIrradianceSemaphores[i].WaitFor();
		}

		//Write the diffuse output resolution to the file.
		file.Write(&diffuseOutputResolution, sizeof(uint32));

		//Write the diffuse to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseOutputTextures[i].Data(), diffuseOutputResolution * diffuseOutputResolution * 4 * sizeof(float));
		}

		//Write the diffuse irradiance output resolution to the file.
		file.Write(&diffuseIrradianceOutputResolution, sizeof(uint32));

		//Write the diffuse irradiance to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseIrradianceOutputTextures[i].Data(), diffuseIrradianceOutputResolution * diffuseIrradianceOutputResolution * 4 * sizeof(float));
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
			case 0: return Vector3(-1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight));
			case 1: return Vector3(1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight));
			case 2: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), -1.0f, CatalystMath::LinearlyInterpolate(1.0f, -1.0f, yWeight)); //Up.
			case 3: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), 1.0f, CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight)); //Down.
			case 4: return Vector3(CatalystMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), -1.0f);
			case 5: return Vector3(CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight), CatalystMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), 1.0f);
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

				Vector2 textureCoordinate{ CatalystMath::ArctangentRadians(position.Z, position.X), CatalystMath::ArcsineRadians(position.Y) };
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

				for (float phi = 0.0f; phi < (2.0f * CatalystMathConstants::PI); phi += sampleDelta)
				{
					for (float theta = 0.0f; theta < (0.5f * CatalystMathConstants::PI); theta += sampleDelta)
					{
						Vector3 tangentSample{ Vector3(CatalystMath::SineRadians(theta) * CatalystMath::CosineRadians(phi), CatalystMath::SineRadians(theta) * CatalystMath::SineRadians(phi), CatalystMath::CosineRadians(theta)) };
						Vector3 sampleVector{ tangentSample.X * rightVector + tangentSample.Y * upVector + tangentSample.Z * direction };

						Vector4 sampledValue{ parameters.diffuseTexture.At(sampleVector) };

						finalIrradiance += Vector3(sampledValue.X, sampledValue.Y, sampledValue.Z) * CatalystMath::CosineRadians(theta) * CatalystMath::SineRadians(theta);

						numberOfSamples += 1.0f;
					}
				}

				parameters.outputTexture.At(j, k) = CatalystMathConstants::PI * finalIrradiance * (1.0f / numberOfSamples);
			}
		}
	}

};