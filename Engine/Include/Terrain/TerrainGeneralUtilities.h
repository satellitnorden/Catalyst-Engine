#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureData.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainGeneralUtilities
{
	/*
	*	Generates a normal at the given position.
	*/
	FORCE_INLINE static void GenerateNormal(const TerrainProperties &properties, const Vector3<float> &position, Vector3<float> *const RESTRICT normal, float *const RESTRICT height = nullptr, const void *const RESTRICT context = nullptr) NOEXCEPT
	{
		constexpr float OFFSET{ 1.0f };

		float center;
		float up;
		float right;

		properties._HeightFunction(properties, position, &center, context);
		properties._HeightFunction(properties, position + Vector3<float>(0.0f, 0.0f, OFFSET), &up, context);
		properties._HeightFunction(properties, position + Vector3<float>(OFFSET, 0.0f, 0.0f), &right, context);
		
		normal->_X = center - right;
		normal->_Y = 1.0f;
		normal->_Z = center - up;

		normal->Normalize();

		if (height)
		{
			*height = center;
		}
	}

	/*
	*	Generates a height texture.
	*/
	FORCE_INLINE static void GenerateHeightTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3<float> &patchWorldPosition, float *const RESTRICT minimumHeight, float *const RESTRICT maximumHeight, Texture2DHandle *const RESTRICT texture) NOEXCEPT
	{
		*minimumHeight = FLOAT_MAXIMUM;
		*maximumHeight = -FLOAT_MAXIMUM;

		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

		DynamicArray<float> data;
		data.UpsizeFast(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION * TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION);

		for (uint32 Y{ 0 }; Y < TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 X{ 0 }; X < TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION; ++X)
			{
				const float coordinateX{ static_cast<float>(X) / static_cast<float>(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION - 1) };
				const float coordinateY{ static_cast<float>(Y) / static_cast<float>(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION - 1) };

				const Vector3<float> worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
													0.0f,
													patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				float height;

				properties._HeightFunction(properties, worldPosition, &height, nullptr);

				data[(Y * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + X] = height;

				*minimumHeight = CatalystBaseMath::Minimum<float>(*minimumHeight, height);
				*maximumHeight = CatalystBaseMath::Maximum<float>(*maximumHeight, height);
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(	TextureDataContainer(data.Data(),
																TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION,
																TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION,
																1,
																1),
																TextureFormat::R32_Float), texture);
	}

	/*
	*	Generates the terrain textures.
	*/
	FORCE_INLINE static void GenerateTerrainTextures(const TerrainProperties &properties, const float patch_size_multiplier, const uint32 resolution, const Vector3<float> &patch_world_position, Texture2DHandle *const RESTRICT texture_handle) NOEXCEPT
	{
		/*
		*	Task generation context class definition.
		*/
		class TaskGenerationContext final
		{

		public:

			//The texture to write into.
			Texture2D<Vector4<byte>>* RESTRICT _Texture;

			//The properties.
			const TerrainProperties *RESTRICT _Properties;

			//The patch world position.
			Vector3<float> _PatchWorldPosition;

			//The patch size.
			float _PatchSize;

			//The resolution.
			uint32 _Resolution;

			//The start X coordinate.
			uint32 _StartX;

			//The start Y coordinate.
			uint32 _StartY;

			//The end X coordinate.
			uint32 _EndX;

			//The end Y coordinate.
			uint32 _EndY;

		};

		//Define constants.
		constexpr uint32 NUMBER_OF_TASKS_PER_DIMENSION{ 4 };
		constexpr uint32 NUMBER_OF_TASKS{ NUMBER_OF_TASKS_PER_DIMENSION * NUMBER_OF_TASKS_PER_DIMENSION };

		//Calculate the patch size.
		const float patch_size{ TerrainConstants::TERRAIN_PATCH_SIZE * patch_size_multiplier };

		//Store the intermediate data in a CPU texture.
		Texture2D<Vector4<byte>> texture{ resolution };

		//Fire off a couple of sub-tasks that will generate parts of the texture.
		StaticArray<TaskGenerationContext, NUMBER_OF_TASKS> contexts;
		StaticArray<Task, NUMBER_OF_TASKS> tasks;

		const uint32 resolution_step{ resolution / NUMBER_OF_TASKS_PER_DIMENSION };
		uint32 current_x{ 0 };
		uint32 current_y{ 0 };

		for (uint8 i{ 0 }; i < NUMBER_OF_TASKS; ++i)
		{
			//Set up the context.
			contexts[i]._Texture = &texture;
			contexts[i]._Properties = &properties;
			contexts[i]._PatchWorldPosition = patch_world_position;
			contexts[i]._PatchSize = patch_size;
			contexts[i]._Resolution = resolution;
			contexts[i]._StartX = current_x;
			contexts[i]._StartY = current_y;
			contexts[i]._EndX = current_x + resolution_step;
			contexts[i]._EndY = current_y + resolution_step;

			current_x += resolution_step;

			if (current_x == resolution)
			{
				current_x = 0;
				current_y += resolution_step;
			}

			//Set up the task.
			tasks[i]._Function = [](void* const RESTRICT arguments)
			{
				TaskGenerationContext* const RESTRICT context{ static_cast<TaskGenerationContext* const RESTRICT>(arguments) };

				for (uint32 Y{ context->_StartY }; Y < context->_EndY; ++Y)
				{
					for (uint32 X{ context->_StartX }; X < context->_EndX; ++X)
					{
						const float coordinate_X{ static_cast<float>(X) / static_cast<float>(context->_Resolution - 1) };
						const float coordinate_Y{ static_cast<float>(Y) / static_cast<float>(context->_Resolution - 1) };

						const Vector3<float> world_position{ context->_PatchWorldPosition._X + ((-1.0f + (2.0f * coordinate_X)) * (context->_PatchSize * 0.5f)),
															0.0f,
															context->_PatchWorldPosition._Z + ((-1.0f + (2.0f * coordinate_Y)) * (context->_PatchSize * 0.5f)) };

						//Generate the height and normal.
						float height;
						Vector3<float> normal;
						GenerateNormal(*context->_Properties, world_position, &normal, &height);

						//Write the normal.
						Vector4<byte>& texture_value{ context->_Texture->At(X, Y) };

						texture_value._X = static_cast<byte>(((normal._X + 1.0f) * 0.5f) * 255.0f);
						texture_value._Y = static_cast<byte>(((normal._Y + 1.0f) * 0.5f) * 255.0f);
						texture_value._Z = static_cast<byte>(((normal._Z + 1.0f) * 0.5f) * 255.0f);

						//Write the material.
						context->_Properties->_MaterialFunction(*context->_Properties, world_position, height, normal, &texture_value._W, nullptr);
					}
				}
			};
			tasks[i]._Arguments = &contexts[i];
			tasks[i]._ExecutableOnSameThread = false;

			//Fire off the task!
			TaskSystem::Instance->ExecuteTask(&tasks[i]);
		}

		//Wait for all tasks to finish.
		for (const Task &task : tasks)
		{
			task.WaitFor();
		}

		//Create the textures.
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(texture), TextureFormat::R8G8B8A8_Byte), texture_handle);
	}

	/*
	*	Generates a patch identifier.
	*/
	FORCE_INLINE static NO_DISCARD uint64 GeneratePatchIdentifier() NOEXCEPT
	{
		static uint64 counter{ 0 };

		return counter++;
	}

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	FORCE_INLINE static void GenerateTerrainPlane(DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve(TerrainConstants::TERRAIN_PATCH_RESOLUTION * TerrainConstants::TERRAIN_PATCH_RESOLUTION);
		indices->Reserve((TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) * (TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) * 6);

		uint32 x{ 0 };
		uint32 y{ 0 };

		for (uint32 i{ 0 }; i < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++i, x = x < 3 ? x + 1 : 0)
		{
			for (uint32 j{ 0 }; j < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++j, y = y < 3 ? y + 1 : 0)
			{
				TerrainVertex vertex;

				vertex._Position._X = static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;
				vertex._Position._Y = static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;

				vertex._Borders = 0;

				//Left.
				if (i == 0)
				{
					if (y == 1 || y == 3)
					{
						vertex._Borders |= BIT(6);
					}

					if (y == 2 || y == 3)
					{
						vertex._Borders |= BIT(7);
					}
				}

				//Right.
				else if (i == TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
				{
					if (y == 1 || y == 3)
					{
						vertex._Borders |= BIT(2);
					}

					if (y == 2 || y == 3)
					{
						vertex._Borders |= BIT(3);
					}
				}

				//Upper.
				if (j == 0)
				{
					if (x == 1 || x == 3)
					{
						vertex._Borders |= BIT(0);
					}

					if (x == 2 || x == 3)
					{
						vertex._Borders |= BIT(1);
					}
				}

				//Lower.
				else if (j == TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
				{
					if (x == 1 || x == 3)
					{
						vertex._Borders |= BIT(4);
					}

					if (x == 2 || x == 3)
					{
						vertex._Borders |= BIT(5);
					}
				}

				vertices->EmplaceFast(vertex);

				if (i != TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1 && j != TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
				{
					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);
					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);

					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);
				}
			}
		}
	}
}