#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//Core.
#include <Core/General/Pair.h>

//Components.
#include <Components/Core/ComponentManager.h>

//File.
#include <File/Writers/TGAWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/AccelerationStructure.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>
#include <Rendering/Native/Shader/CatalystLighting.h>
#include <Rendering/Native/Shader/CatalystTerrain.h>
#include <Rendering/Native/Shader/CatalystToneMapping.h>
#include <Rendering/Native/Shader/CatalystVolumetricLighting.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldSystem.h>

//Terrin.
#include <Terrain/TerrainGeneralUtilities.h>
#include <Terrain/TerrainVertex.h>

//User interface.
#include <UserInterface/TextUserInterfaceElementDescription.h>

//Rendering reference system constants.
namespace RenderingReferenceSystemConstants
{
	constexpr uint32 SLIZE_SIZE{ 16 };
}

/*
*	Updates the rendering reference system during the render update phase.
*/
void RenderingReferenceSystem::RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Should the rendering reference start/stop?
	const KeyboardState *const RESTRICT state{ InputSystem::Instance->GetKeyboardState() };

	if (state->GetButtonState(KeyboardButton::F1) == ButtonState::Pressed)
	{
		//Is the rendering reference in progress? If so, stop it.
		if (_RenderingReferenceInProgress)
		{
			EndRenderingReference();
		}

		//Otherwise, start it.
		else
		{
			StartRenderingReference();
		}
	}

	//If rendering reference is in progress, do one iteration on the texture.
	if (_RenderingReferenceInProgress)
	{
		UpdateRenderingReference();
	}
}

/*
*	Starts the rendering reference.
*/
void RenderingReferenceSystem::StartRenderingReference() NOEXCEPT
{
	//Start the rendering reference progress.
	_RenderingReferenceInProgress = true;

	//Create the rendering reference data.
	_RenderingReferenceData = static_cast<RenderingReferenceData *const RESTRICT>(Memory::Allocate(sizeof(RenderingReferenceData)));
	new (_RenderingReferenceData) RenderingReferenceData();

	//Initialize the texture.
	_RenderingReferenceData->_RenderingReferenceTexture.Initialize(RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height);

	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			_RenderingReferenceData->_RenderingReferenceTexture.At(X, Y) = Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	//Calculate how much asynchronous data is needed.
	uint32 current_slice{ 0 };
	const uint32 maximum_y{ RenderingSystem::Instance->GetScaledResolution()._Height };

	uint32 counter{ 0 };

	for (;;)
	{
		++counter;

		current_slice += RenderingReferenceSystemConstants::SLIZE_SIZE;

		if (current_slice >= maximum_y)
		{
			break;
		}
	}

	_RenderingReferenceData->_AsynchronousData.Reserve(counter);

	//Calculate all the synchronous data.
	current_slice = 0;

	for (;;)
	{
		//Add new asynchronous data.
		_RenderingReferenceData->_AsynchronousData.Emplace();

		RenderingReferenceData::AsynchronousData& data{ _RenderingReferenceData->_AsynchronousData.Back() };

		data._Task._Function = [](void *const RESTRICT arguments)
		{
			RenderingSystem::Instance->GetRenderingReferenceSystem()->ExecuteAsynchronous(static_cast<const RenderingReferenceData::AsynchronousData *const RESTRICT>(arguments));
		};
		data._Task._Arguments = &data;
		data._Task._ExecutableOnSameThread = false;
		data._StartY = current_slice;
		data._EndY = CatalystBaseMath::Minimum<uint32>(current_slice + RenderingReferenceSystemConstants::SLIZE_SIZE, maximum_y);

		current_slice += RenderingReferenceSystemConstants::SLIZE_SIZE;

		if (current_slice >= maximum_y)
		{
			break;
		}
	}

	//Create the progress information.
	{
		TextUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::TEXT;
		description._Minimum = Vector2<float>(0.01f, 0.01f);
		description._Maximum = Vector2<float>(0.99f, 0.25f);
		description._Font = &ResourceLoader::GetFontResource(HashString("Catalyst_Engine_Default_Font"));
		description._Scale = 0.025f;
		description._Text = "";

		_RenderingReferenceData->_ProgressInformation = static_cast<TextUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	//Prepare the terrain data.
	{
		//Keep track of the indices offset.
		uint32 indices_offset{ 0 };

		//Define constants.
		constexpr StaticArray<float, 2> VERTEX_BORDER_OFFSETS{ 1.0f / 64.0f, 1.0f / 32.0f };

		//Generate the terrain vertices and indices.
		DynamicArray<TerrainVertex> vertices;
		DynamicArray<uint32> indices;

		TerrainGeneralUtilities::GenerateTerrainPlane(	*TerrainSystem::Instance->GetTerrainProperties(),
														TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution,
														&vertices,
														&indices);

		//Iterate over all terrain patches and intersect against their triangles.
		const TerrainPatchInformation* RESTRICT patch_information{ TerrainSystem::Instance->GetTerrainPatchInformations()->Data() };
		const TerrainPatchRenderInformation* RESTRICT patch_render_information{ TerrainSystem::Instance->GetTerrainPatchRenderInformations()->Data() };

		for (uint64 i{ 0 }, size{ TerrainSystem::Instance->GetTerrainPatchInformations()->Size() }; i < size; ++i, ++patch_information, ++patch_render_information)
		{
			//Cache relevant data.
			const int32 patch_borders{ patch_render_information->_Borders };

			//Add all vertex data.
			for (const TerrainVertex &vertex : vertices)
			{
				//Calculate the position.
				Vector3<float32> position;

				position._X = vertex._Position._X;
				position._Y = 0.0f;
				position._Z = vertex._Position._Y;

				//Apply the border offsets.
				{
					//Cache relevant data.
					const int32 vertex_borders{ vertex._Borders };

					//Calculate the horizontal border offset multiplier.
					float32 is_upper_multiplier{ static_cast<float>((vertex_borders & BIT(0)) & (patch_borders & BIT(0))) };
					float32 is_lower_multiplier{ static_cast<float>((vertex_borders & BIT(4)) & (patch_borders & BIT(4))) };
					float32 horizontal_border_offset_weight{ CatalystBaseMath::Minimum<float32>(is_upper_multiplier + is_lower_multiplier, 1.0f) };

					//Calculate the vertical border offset multiplier.
					float32 is_right_multiplier{ static_cast<float>((vertex_borders& BIT(2))& (patch_borders& BIT(2))) };
					float32 is_left_multiplier{ static_cast<float>((vertex_borders& BIT(6))& (patch_borders& BIT(6))) };
					float32 vertical_border_offset_weight{ CatalystBaseMath::Minimum<float32>(is_right_multiplier + is_left_multiplier, 1.0f) };

					position._X -= VERTEX_BORDER_OFFSETS[0] * horizontal_border_offset_weight;
					position._Z -= VERTEX_BORDER_OFFSETS[0] * vertical_border_offset_weight;

					//Calculate the horizontal border offset multiplier.
					is_upper_multiplier = static_cast<float>((vertex_borders& BIT(1))& (patch_borders& BIT(1)));
					is_lower_multiplier = static_cast<float>((vertex_borders& BIT(5))& (patch_borders& BIT(5)));
					horizontal_border_offset_weight = CatalystBaseMath::Minimum<float32>(is_upper_multiplier + is_lower_multiplier, 1.0f);

					//Calculate the vertical border offset multiplier.
					is_right_multiplier = static_cast<float>((vertex_borders& BIT(3))& (patch_borders& BIT(3)));
					is_left_multiplier = static_cast<float>((vertex_borders& BIT(7))& (patch_borders& BIT(7)));
					vertical_border_offset_weight = CatalystBaseMath::Minimum<float32>(is_right_multiplier + is_left_multiplier, 1.0f);

					position._X -= VERTEX_BORDER_OFFSETS[1] * horizontal_border_offset_weight;
					position._Z -= VERTEX_BORDER_OFFSETS[1] * vertical_border_offset_weight;
				}

				position._X = patch_render_information->_WorldPosition._X + position._X * patch_render_information->_PatchSize;
				position._Z = patch_render_information->_WorldPosition._Y + position._Z * patch_render_information->_PatchSize;

				TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y);

				//Add the vertex data to the terrain acceleration structure.
				_RenderingReferenceData->_TerrainAccelerationStructure.AddVertexData(AccelerationStructure<byte>::VertexData(position, 0));
			}

			//Add all triangle data.
			for (uint64 j{ 0 }; j < indices.Size(); j += 3)
			{
				//Add the triangle to the terrain acceleration structure.
				_RenderingReferenceData->_TerrainAccelerationStructure.AddTriangleData(AccelerationStructure<byte>::TriangleData(StaticArray<uint32, 3>(indices[j + 0] + indices_offset, indices[j + 1] + indices_offset, indices[j + 2] + indices_offset)));
			}

			//Update the indices offset.
			indices_offset += static_cast<uint32>(vertices.Size());
		}

		//Build the terrain acceleration structure.
		_RenderingReferenceData->_TerrainAccelerationStructure.Build(4);
	}

	//Prepare the models data.
	{
		//Iterate over all model components and add their data to the models acceleration structure.
		const uint64 number_of_model_components{ ComponentManager::GetNumberOfModelComponents() };
		ModelComponent* RESTRICT component{ ComponentManager::GetModelModelComponents() };
		uint32 indices_offset{ 0 };

		for (uint64 i{ 0 }; i < number_of_model_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_Model->_Meshes)
			{
				//Make local copies of the vertices and indices.
				DynamicArray<Vertex> vertices{ mesh._Vertices };
				DynamicArray<uint32> indices{ mesh._Indices };

				//Transform all vertices.
				for (Vertex &vertex : vertices)
				{
					vertex.Transform(component->_CurrentWorldTransform, 0.0f);
				}

				//Add all vertices.
				for (Vertex &vertex : vertices)
				{
					_RenderingReferenceData->_ModelsAccelerationStructure.AddVertexData(AccelerationStructure<Vertex>::VertexData(vertex._Position, vertex));
				}

				//Add all indices.
				for (uint64 j{ 0 }, size{ indices.Size() }; j < size; j += 3)
				{
					_RenderingReferenceData->_ModelsAccelerationStructure.AddTriangleData(AccelerationStructure<Vertex>::TriangleData(StaticArray<uint32, 3>(indices[j + 0] + indices_offset, indices[j + 1] + indices_offset, indices[j + 2] + indices_offset)));
				}

				//Update the indices offset.
				indices_offset += static_cast<uint32>(vertices.Size());
			}
		}

		//Build the models acceleration structure.
		_RenderingReferenceData->_ModelsAccelerationStructure.Build(4);
	}

	//Set the update speed to zero.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);
}

/*
*	Ends the rendering reference.
*/
void RenderingReferenceSystem::EndRenderingReference() NOEXCEPT
{
	//End the rendering reference progress.
	_RenderingReferenceInProgress = false;

	//Wait for all tasks to finish.
	for (RenderingReferenceData::AsynchronousData& data : _RenderingReferenceData->_AsynchronousData)
	{
		data._Task.WaitFor();
	}

	//Normalize the values in the rendering reference texture.
	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			_RenderingReferenceData->_RenderingReferenceTexture.At(X, Y) /= static_cast<float>(_RenderingReferenceData->_Iterations + 1);
		}
	}

	//Write the image to file.
	TGAWriter::Write(_RenderingReferenceData->_RenderingReferenceTexture, "RenderingReference.tga");

	//Destroy the progress information.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_RenderingReferenceData->_ProgressInformation);

	//Set the update speed back to normal.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);

	//Destroy the rendering reference data.
	_RenderingReferenceData->~RenderingReferenceData();
	Memory::Free(_RenderingReferenceData);
}

/*
*	Updates the rendering reference.
*/
void RenderingReferenceSystem::UpdateRenderingReference() NOEXCEPT
{
	//Are all tasks executed?
	bool all_tasks_executed{ true };

	for (const RenderingReferenceData::AsynchronousData& data : _RenderingReferenceData->_AsynchronousData)
	{
		all_tasks_executed &= data._Task.IsExecuted();
	}

	//If so, fire off a new batch.
	if (all_tasks_executed)
	{
		//Execute all tasks.
		for (RenderingReferenceData::AsynchronousData& data : _RenderingReferenceData->_AsynchronousData)
		{
			TaskSystem::Instance->ExecuteTask(&data._Task);
		}

		//Update the number of iterations.
		++_RenderingReferenceData->_Iterations;

		//Reset the texels calculated.
		_RenderingReferenceData->_TexelsCalculated.store(0);

		//Recreate the texture.
		if (_RenderingReferenceData->_RenderingReferenceTextureHandle)
		{
			RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_RenderingReferenceData->_RenderingReferenceTextureIndex);
			RenderingSystem::Instance->DestroyTexture2D(&_RenderingReferenceData->_RenderingReferenceTextureHandle);
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_RenderingReferenceData->_RenderingReferenceTexture), TextureFormat::R32G32B32A32_Float), &_RenderingReferenceData->_RenderingReferenceTextureHandle);
		_RenderingReferenceData->_RenderingReferenceTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_RenderingReferenceData->_RenderingReferenceTextureHandle);

		//Set the properties for the rendering reference.
		RenderingReferenceRenderPass::Instance->SetProperties(_RenderingReferenceData->_RenderingReferenceTextureIndex, _RenderingReferenceData->_Iterations);
	}

	//Update the progress information.
	const float iteration_percent{ _RenderingReferenceData->_TexelsCalculated.load() / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width * RenderingSystem::Instance->GetScaledResolution()._Height) * 100.0f };

	char buffer[128];
	sprintf_s(buffer, "Rendering Reference Progress: Iterations - %u - Iteration completion - %.3f%%", _RenderingReferenceData->_Iterations, iteration_percent);

	_RenderingReferenceData->_ProgressInformation->_Text = buffer;
}

/*
*	Executes asynchronously.
*/
void RenderingReferenceSystem::ExecuteAsynchronous(const RenderingReferenceData::AsynchronousData *const RESTRICT data) NOEXCEPT
{
	//Calculate all texels.
	for (uint32 Y{ data->_StartY }; Y < data->_EndY; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			//Calculate the texel.
			CalculateTexel(X, Y);

			//Increment the number of texels calculated.
			++_RenderingReferenceData->_TexelsCalculated;
		}
	}
}

/*
*	Calculates a texel.
*/
void RenderingReferenceSystem::CalculateTexel(const uint32 X, const uint32 Y) NOEXCEPT
{
	//Calculate the ray.
	Ray ray;

	ray.SetOrigin(Perceiver::Instance->GetPosition());
	ray.SetDirection(CalculateRayDirection(X, Y));

	//Cast a ray against the scene and determine the color.
	Vector3<float> color{ CastRayScene(ray, 0) };

	//Perform some post processing...
	color = CatalystToneMapping::ApplyToneMapping(color);

	//Write the color.
	_RenderingReferenceData->_RenderingReferenceTexture.At(X, Y) += Vector4<float>(color, 1.0f);
}

/*
*	Calculates the ray direction.
*/
Vector3<float> RenderingReferenceSystem::CalculateRayDirection(const uint32 X, const uint32 Y) NOEXCEPT
{
	//Define constants.
	constexpr float JITTER{ 0.5f };

	//Calculate the coordinate.
	const Vector2<float> coordinate{	(static_cast<float>(X) + 0.5f + CatalystRandomMath::RandomFloatInRange(-JITTER, JITTER)) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width),
										(static_cast<float>(Y) + 0.5f + CatalystRandomMath::RandomFloatInRange(-JITTER, JITTER)) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height) };

	//Calculate the ray direction.
	return RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(coordinate);
}

/*
*	Casts a ray against the scene. Returns the color.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CastRayScene(const Ray& ray, const uint8 recursion) NOEXCEPT
{
	//Cast a rays against the scene.
	VolumetricDescription volumetric_description;
	SurfaceDescription surface_description;
	float hit_distance{ FLOAT_MAXIMUM };
	bool has_hit_volumetric{ false };
	bool has_hit_surface{ false };

	has_hit_surface |= CastSurfaceRayTerrain(ray, &surface_description, &hit_distance);
	has_hit_surface |= CastSurfaceRayModels(ray, &surface_description, &hit_distance);
	has_hit_volumetric |= CastVolumetricRayScene(ray, &volumetric_description, &hit_distance);

	//Determine the color.
	if (has_hit_volumetric)
	{
		return CalculateVolumetricLighting(ray, hit_distance, volumetric_description, recursion);
	}

	else if (has_hit_surface)
	{
		return CalculateSurfaceLighting(ray, hit_distance, surface_description, recursion);
	}

	else
	{
		return CastRaySky(ray);
	}
}

/*
*	Casts a surface ray against the volumetric particles. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastVolumetricRayScene(const Ray& ray, VolumetricDescription *const RESTRICT volumetric_description, float* const RESTRICT hit_distance) NOEXCEPT
{
	//Randomize the distance weight.
	float distance_weight{ CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f) };

	//Modify the distance weight based on the thickness.
	distance_weight = 1.0f - pow(1.0f - distance_weight, WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness);

	//Calculate the volumetric hit distance.
	const float volumetric_hit_distance{ CatalystBaseMath::Minimum<float>(*hit_distance, WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance) * distance_weight };

	//Calculate the hit position.
	const Vector3<float> hit_position{ ray._Origin + ray._Direction * volumetric_hit_distance };

	//Calculate the opacity.
	const float opacity{ CatalystVolumetricLighting::CalculateVolumetricLightingOpacity(CatalystBaseMath::Minimum<float>(*hit_distance, WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance),
																						WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance,
																						hit_position._Y,
																						WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Height,
																						WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness) };

	//Determine if a volumetric particle was hit.
	if (CatalystRandomMath::RandomChance(opacity))
	{
		//Fill in the surface description/hit distance.
		volumetric_description->_Albedo = Vector3<float>(0.5f, 0.75f, 1.0f);

		*hit_distance = volumetric_hit_distance;

		return true;
	}

	else
	{
		return false;
	}
}

/*
*	Casts a surface ray against terrain.
*/
NO_DISCARD bool RenderingReferenceSystem::CastSurfaceRayTerrain(const Ray &ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
{
	//Trace the terrain acceleration structure.
	if (const AccelerationStructure<byte>::TriangleData *const RESTRICT triangle_data{ _RenderingReferenceData->_TerrainAccelerationStructure.TraceSurface(ray, hit_distance) })
	{
		//Calculate the hit position.
		const Vector3<float> hit_position{ ray._Origin + ray._Direction * *hit_distance };	

		//Retrieve the terrain material at the hit position.
		Vector4<uint8> terrain_material_indices;
		Vector4<float> terrain_material_blend;

		TerrainSystem::Instance->GetTerrainMaterialAtPosition(hit_position, &terrain_material_indices, &terrain_material_blend);

		//Retrieve the materials referenced by the terrain material indices.
		StaticArray<Material, 4> materials;

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			materials[i] = RenderingSystem::Instance->GetMaterialSystem()->GetGlobalMaterial(terrain_material_indices[i]);
		}

		//Fill in the surface descipription.
		{
			//Fill in the albedo.
			Memory::Set(&surface_description->_Albedo, 0, sizeof(Vector3<float>));

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const Vector4<float> sample{ _RenderingReferenceData->_Textures[materials[i]._AlbedoTextureIndex]->_Texture2D.Sample(Vector2<float>(hit_position._X, hit_position._Z) * 0.25f, AddressMode::Repeat) };

				surface_description->_Albedo += Vector3<float>(sample._R, sample._G, sample._B) * terrain_material_blend[i];
			}
		}

		{
			//Fill in the normal.
			Memory::Set(&surface_description->_Normal, 0, sizeof(Vector3<float>));

			//Calculate the terrain tangent space matrix.
			float center_height;
			float right_height;
			float up_height;

			TerrainSystem::Instance->GetTerrainHeightAtPosition(hit_position, &center_height);
			TerrainSystem::Instance->GetTerrainHeightAtPosition(hit_position + Vector3<float>(1.0f, 0.0f, 0.0f), &right_height);
			TerrainSystem::Instance->GetTerrainHeightAtPosition(hit_position + Vector3<float>(0.0f, 0.0f, 1.0f), &up_height);

			const Matrix3x3 terrain_tangent_space_matrix{ CatalystTerrain::CalculateTerrainTangentSpaceMatrix(center_height, right_height, up_height) };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const Vector4<float> sample{ _RenderingReferenceData->_Textures[materials[i]._NormalMapTextureIndex]->_Texture2D.Sample(Vector2<float>(hit_position._X, hit_position._Z) * 0.25f, AddressMode::Repeat) };

				surface_description->_Normal += (Vector3<float>(sample._R, sample._G, sample._B) * 2.0f - 1.0f) * terrain_material_blend[i];
			}

			surface_description->_Normal = terrain_tangent_space_matrix * Vector3<float>::Normalize(surface_description->_Normal);
		}

		{
			//Fill in the material properties.
			Memory::Set(&surface_description->_Roughness, 0, sizeof(float32));
			Memory::Set(&surface_description->_Metallic, 0, sizeof(float32));
			Memory::Set(&surface_description->_AmbientOcclusion, 0, sizeof(float32));

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const Vector4<float> sample{ _RenderingReferenceData->_Textures[materials[i]._MaterialPropertiesTextureIndex]->_Texture2D.Sample(Vector2<float>(hit_position._X, hit_position._Z) * 0.25f, AddressMode::Repeat) };

				surface_description->_Roughness += sample._X * terrain_material_blend[i];
				surface_description->_Metallic += sample._Y * terrain_material_blend[i];
				surface_description->_AmbientOcclusion += sample._Z * terrain_material_blend[i];
			}
		}

		return true;
	}

	else
	{
		return false;
	}
}

/*
*	Casts a surface ray against models. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastSurfaceRayModels(const Ray &ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
{
	//Trace the models acceleration structure.
	if (const AccelerationStructure<Vertex>::TriangleData *const RESTRICT triangle_data{ _RenderingReferenceData->_ModelsAccelerationStructure.TraceSurface(ray, hit_distance) })
	{	
		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * *hit_distance };

		//Retrieve the vertex data.
		const AccelerationStructure<Vertex>::VertexData &vertex_data_1{ _RenderingReferenceData->_ModelsAccelerationStructure.GetVertexData(triangle_data->_Indices[0]) };
		const AccelerationStructure<Vertex>::VertexData &vertex_data_2{ _RenderingReferenceData->_ModelsAccelerationStructure.GetVertexData(triangle_data->_Indices[1]) };
		const AccelerationStructure<Vertex>::VertexData &vertex_data_3{ _RenderingReferenceData->_ModelsAccelerationStructure.GetVertexData(triangle_data->_Indices[2]) };

		//Reconstruct the triangle.
		Triangle triangle;

		triangle._Vertices[0] = vertex_data_1._Position;
		triangle._Vertices[1] = vertex_data_2._Position;
		triangle._Vertices[2] = vertex_data_3._Position;

		//Calculate the barycentri coordinates.
		const Vector3<float32> barycentric_coordinates{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_position) };

		//Calculate the normal.
		const Vector3<float32> normal{	Vector3<float32>::Normalize(vertex_data_1._UserData._Normal * barycentric_coordinates[0]
																	+ vertex_data_2._UserData._Normal * barycentric_coordinates[1]
																	+ vertex_data_3._UserData._Normal * barycentric_coordinates[2]) };


		//Calculate the tangent.
		const Vector3<float32> tangent{	Vector3<float32>::Normalize(vertex_data_1._UserData._Tangent * barycentric_coordinates[0]
																	+ vertex_data_2._UserData._Tangent * barycentric_coordinates[1]
																	+ vertex_data_3._UserData._Tangent * barycentric_coordinates[2]) };

		//Calculate the texture coordinate.
		const Vector2<float32> texture_coordinate{	vertex_data_1._UserData._TextureCoordinate * barycentric_coordinates[0]
													+ vertex_data_2._UserData._TextureCoordinate * barycentric_coordinates[1]
													+ vertex_data_3._UserData._TextureCoordinate * barycentric_coordinates[2] };

		//Fill in the surface description.
		surface_description->_Albedo = Vector3<float32>(0.5f, 0.5f, 0.5f);
		surface_description->_Normal = normal;
		surface_description->_Roughness = 0.1f;
		surface_description->_Metallic = 0.9f;
		surface_description->_AmbientOcclusion = 1.0f;

		return true;
	}

	else
	{
		return false;
	}
}

/*
*	Casts a ray against the sky. Returns the color.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CastRaySky(const Ray& ray) NOEXCEPT
{
	const SkyGradient current_sky_gradient{ WorldSystem::Instance->GetSkySystem()->GetCurrentSkyGradient() };
	const Vector3<float> lower_sky_color{ current_sky_gradient._LowerSkyColor };
	const Vector3<float> upper_sky_color{ current_sky_gradient._UpperSkyColor };

	const float upward{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(ray._Direction, VectorConstants::UP), 0.0f) };

	return CatalystBaseMath::LinearlyInterpolate(lower_sky_color, upper_sky_color, upward);
}

/*
*	Calculates the surface lighting.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CalculateSurfaceLighting(const Ray &incoming_ray, const float hit_distance, const SurfaceDescription& surface_description, const uint8 recursion) NOEXCEPT
{
	//Define constants.
	constexpr float NORMAL_OFFSET_FACTOR{ FLOAT_EPSILON * 1'024.0f * 1'024.0f };
	constexpr float DIRECTIONAL_LIGHT_SIZE{ FLOAT_EPSILON * 1'024.0f * 32.0f };

	//Calculate the lighting.
	Vector3<float> lighting{ 0.0f, 0.0f, 0.0f };

	//Calculate the indirect lighting.
	{
		//Calculate the diffuseness of the surface.
		const float32 surface_diffuseness{ surface_description._Roughness * (1.0f - surface_description._Metallic) };

		//Calculate the indirect lighting direction.
		Vector3<float> indirect_lighting_direction;

		indirect_lighting_direction._X = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Y = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Z = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);

		const Vector3<float32> reflection_vector{ Vector3<float32>::Reflect(incoming_ray._Direction, surface_description._Normal) };

		indirect_lighting_direction = CatalystBaseMath::LinearlyInterpolate(reflection_vector, indirect_lighting_direction, surface_diffuseness);
		indirect_lighting_direction.NormalizeSafe();

		indirect_lighting_direction = Vector3<float>::DotProduct(surface_description._Normal, indirect_lighting_direction) >= 0.0f ? indirect_lighting_direction : -indirect_lighting_direction;

		Ray indirect_lighting_ray;

		indirect_lighting_ray.SetOrigin(incoming_ray._Origin + incoming_ray._Direction * hit_distance + surface_description._Normal * NORMAL_OFFSET_FACTOR);
		indirect_lighting_ray.SetDirection(indirect_lighting_direction);

		Vector3<float> indirect_lighting;

		if (recursion < 2)
		{
			indirect_lighting = CastRayScene(indirect_lighting_ray, recursion + 1);
		}

		else
		{
			indirect_lighting = CastRaySky(indirect_lighting_ray);
		}

		lighting += CatalystLighting::CalculateLighting(-incoming_ray._Direction,
																surface_description._Albedo,
																surface_description._Normal,
																surface_description._Roughness,
																surface_description._Metallic,
																surface_description._AmbientOcclusion,
																1.0f,
																-indirect_lighting_direction,
																indirect_lighting);
	}

	//Calculate the direct lighting.
	{
		//Iterate over all light components and calculate their lighting.
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent *RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i = 0; i < number_of_light_components; ++i, ++component)
		{
			switch (static_cast<LightType>(component->_LightType))
			{
				case LightType::DIRECTIONAL:
				{
					//Cast a shadow ray to determine visibility.
					Ray shadow_ray;

					shadow_ray.SetOrigin(incoming_ray._Origin + incoming_ray._Direction * hit_distance + surface_description._Normal * NORMAL_OFFSET_FACTOR);
					shadow_ray.SetDirection(Vector3<float>::Normalize(-component->_Direction + Vector3<float>(CatalystRandomMath::RandomFloatInRange(-DIRECTIONAL_LIGHT_SIZE, DIRECTIONAL_LIGHT_SIZE), CatalystRandomMath::RandomFloatInRange(-DIRECTIONAL_LIGHT_SIZE, DIRECTIONAL_LIGHT_SIZE), CatalystRandomMath::RandomFloatInRange(-DIRECTIONAL_LIGHT_SIZE, DIRECTIONAL_LIGHT_SIZE))));

					const bool in_shadow{ CastShadowRayScene(shadow_ray) };

					if (!in_shadow)
					{
						lighting += CatalystLighting::CalculateLighting(	-incoming_ray._Direction,
																				surface_description._Albedo,
																				surface_description._Normal,
																				surface_description._Roughness,
																				surface_description._Metallic,
																				surface_description._AmbientOcclusion,
																				1.0f,
																				component->_Direction,
																				component->_Luminance);
					}

					break;
				}

				case LightType::POINT:
				{
					break;
				}
			}
		}
	}

	return lighting;
}

/*
*	Calculates the volumetric lighting.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CalculateVolumetricLighting(const Ray &incoming_ray, const float hit_distance, const VolumetricDescription &volumetric_description, const uint8 recursion) NOEXCEPT
{
	//Calculate the lighting.
	Vector3<float> lighting{ 0.0f, 0.0f, 0.0f };

	//Calculate the indirect lighting.
	{
		Vector3<float> indirect_lighting_direction;

		indirect_lighting_direction._X = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Y = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Z = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);

		indirect_lighting_direction.NormalizeSafe();

		Ray indirect_lighting_ray;

		indirect_lighting_ray.SetOrigin(incoming_ray._Origin + incoming_ray._Direction * hit_distance);
		indirect_lighting_ray.SetDirection(indirect_lighting_direction);

		Vector3<float> indirect_lighting;

		if (recursion < 2)
		{
			indirect_lighting = CastRayScene(indirect_lighting_ray, recursion + 1);
		}

		else
		{
			indirect_lighting = CastRaySky(indirect_lighting_ray);
		}

		lighting += volumetric_description._Albedo * indirect_lighting;
	}

	//Calculate the direct lighting.
	{
		//Iterate over all light components and calculate their lighting.
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i = 0; i < number_of_light_components; ++i, ++component)
		{
			switch (static_cast<LightType>(component->_LightType))
			{
				case LightType::DIRECTIONAL:
				{
					//Cast a shadow ray to determine visibility.
					Ray shadow_ray;

					shadow_ray.SetOrigin(incoming_ray._Origin + incoming_ray._Direction * hit_distance);
					shadow_ray.SetDirection(-component->_Direction);

					const bool in_shadow{ CastShadowRayScene(shadow_ray) };

					if (!in_shadow)
					{
						lighting += volumetric_description._Albedo * component->_Luminance;
					}

					break;
				}

				case LightType::POINT:
				{
					break;
				}
			}
		}
	}

	return lighting;
}

/*
*	Casts a shadow ray against the scene. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastShadowRayScene(const Ray &ray) NOEXCEPT
{
	return CastShadowRayTerrain(ray) || CastShadowRayModels(ray);
}

/*
*	Casts a shadow ray against the terrain. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastShadowRayTerrain(const Ray &ray) NOEXCEPT
{
	return _RenderingReferenceData->_TerrainAccelerationStructure.TraceShadow(ray);
}

/*
*	Casts a shadow ray against models. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastShadowRayModels(const Ray &ray) NOEXCEPT
{
	return _RenderingReferenceData->_ModelsAccelerationStructure.TraceShadow(ray);
}
#endif