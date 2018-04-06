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
#include <Rendering/Engine Layer/CPUTexture4.h>

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

		StaticArray<LayerCreationParameters, 6> diffuseLayerCreationParameters
		{
			LayerCreationParameters(0, outputResolution, diffuseOutputTextures[0], hdrTexture),
			LayerCreationParameters(1, outputResolution, diffuseOutputTextures[1], hdrTexture),
			LayerCreationParameters(2, outputResolution, diffuseOutputTextures[2], hdrTexture),
			LayerCreationParameters(3, outputResolution, diffuseOutputTextures[3], hdrTexture),
			LayerCreationParameters(4, outputResolution, diffuseOutputTextures[4], hdrTexture),
			LayerCreationParameters(5, outputResolution, diffuseOutputTextures[5], hdrTexture),
		};

		//Calculate the diffuse.
		StaticArray<Semaphore, 6> diffuseSemaphores;

		for (uint8 i = 0; i < 6; ++i)
		{
			TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
			{
				const LayerCreationParameters &parameters{ *static_cast<LayerCreationParameters *const RESTRICT>(arguments) };

				CreateDiffuseLayer(parameters);
			}, &diffuseLayerCreationParameters[i], &diffuseSemaphores[i]));
		}

		//Create the diffuse irradiance output textures.
		StaticArray<CPUTexture4, 6> diffuseIrradianceOutputTextures
		{
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution)
		};

		StaticArray<LayerCreationParameters, 6> diffuseIrradianceLayerCreationParameters
		{
			LayerCreationParameters(0, outputResolution, diffuseIrradianceOutputTextures[0], hdrTexture),
			LayerCreationParameters(1, outputResolution, diffuseIrradianceOutputTextures[1], hdrTexture),
			LayerCreationParameters(2, outputResolution, diffuseIrradianceOutputTextures[2], hdrTexture),
			LayerCreationParameters(3, outputResolution, diffuseIrradianceOutputTextures[3], hdrTexture),
			LayerCreationParameters(4, outputResolution, diffuseIrradianceOutputTextures[4], hdrTexture),
			LayerCreationParameters(5, outputResolution, diffuseIrradianceOutputTextures[5], hdrTexture),
		};

		//Calculate the diffuse.
		StaticArray<Semaphore, 6> diffuseIrradianceSemaphores;

		for (uint8 i = 0; i < 6; ++i)
		{
			TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
			{
				const LayerCreationParameters &parameters{ *static_cast<LayerCreationParameters *const RESTRICT>(arguments) };

				CreateDiffuseIrradianceLayer(parameters);
			}, &diffuseIrradianceLayerCreationParameters[i], &diffuseIrradianceSemaphores[i]));
		}
		
		//Wait for all tasks to finish.
		for (uint8 i = 0; i < 6; ++i)
		{
			diffuseSemaphores[i].WaitFor();
		}

		for (uint8 i = 0; i < 6; ++i)
		{
			diffuseIrradianceSemaphores[i].WaitFor();
		}

		//Write the diffuse to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseOutputTextures[i].Data(), outputResolution * outputResolution * 4 * sizeof(float));
		}

		//Write the diffuse irradiance to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(diffuseIrradianceOutputTextures[i].Data(), outputResolution * outputResolution * 4 * sizeof(float));
		}

		/*
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
		*/

		//Close the file.
		file.Close();
	}

private:

	/*
	*	Definition for the layer creation parameters.
	*/
	class LayerCreationParameters final
	{

	public:

		LayerCreationParameters(const uint8 initialIndex, const uint32 initialOutputResolution, CPUTexture4 &initialOutputTexture, const CPUTexture4 &initialHdrTexture) NOEXCEPT
			:
			index(initialIndex),
			outputResolution(initialOutputResolution),
			outputTexture(initialOutputTexture),
			hdrTexture(initialHdrTexture)
		{

		}

		const uint8 index;
		const uint32 outputResolution;
		CPUTexture4 &outputTexture;
		const CPUTexture4 &hdrTexture;

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
	static void CreateDiffuseLayer(const LayerCreationParameters &parameters) NOEXCEPT
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
	static void CreateDiffuseIrradianceLayer(const LayerCreationParameters &parameters) NOEXCEPT
	{
		for (uint32 j = 0; j < parameters.outputResolution; ++j)
		{
			for (uint32 k = 0; k < parameters.outputResolution; ++k)
			{
				Vector3 finalIrradiance{ 0.0f, 0.0f, 0.0f };
				Vector3 direction{ GetPositionVector(parameters.index, static_cast<float>(j) / static_cast<float>(parameters.outputResolution), static_cast<float>(k) / static_cast<float>(parameters.outputResolution)) };

				direction.Normalize();

				Vector3 rightVector{ Vector3::CrossProduct(Vector3(0.0f, 1.0f, 0.0f), direction) };

				rightVector.Normalize();

				Vector3 upVector{ Vector3::CrossProduct(direction, rightVector) };

				upVector.Normalize();

				float sampleDelta{ 0.025f };
				float numberOfSamples{ 0.0f };

				for (float phi = 0.0f; phi < (2.0f * CatalystMathConstants::PI); phi += sampleDelta)
				{
					for (float theta = 0.0f; theta < (0.5f * CatalystMathConstants::PI); theta += sampleDelta)
					{
						Vector3 tangentSample{ Vector3(CatalystMath::SineRadians(theta) * CatalystMath::CosineRadians(phi), CatalystMath::SineRadians(theta) * CatalystMath::SineRadians(phi), CatalystMath::CosineRadians(theta)) };
						Vector3 sampleVector{ tangentSample.X * rightVector + tangentSample.Y * upVector + tangentSample.Z * direction };

						//sampleVector.Normalize();

						Vector2 textureCoordinate{ CatalystMath::ArctangentRadians(sampleVector.Z, sampleVector.X), CatalystMath::ArcsineRadians(sampleVector.Y) };
						textureCoordinate *= EnvironmentMaterialCreatorConstants::INVERSE_ATAN;
						textureCoordinate += 0.5f;

						Vector4 sampledValue{ parameters.hdrTexture.At(textureCoordinate) };

						finalIrradiance += Vector3(sampledValue.X, sampledValue.Y, sampledValue.Z) * CatalystMath::CosineRadians(theta) * CatalystMath::SineRadians(theta);

						numberOfSamples += 1.0f;
					}
				}

				parameters.outputTexture.At(j, k) = CatalystMathConstants::PI * finalIrradiance * (1.0f / numberOfSamples);

				if (parameters.outputTexture.At(j, k).Length() < 0.0001f)
				{
					BREAKPOINT;
				}
			}
		}
	}

};