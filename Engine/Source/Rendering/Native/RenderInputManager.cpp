//Header file.
#include <Rendering/Native/RenderInputManager.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/GrassCore.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Push constant data struct definitions.
struct ModelDepthPushConstantData
{
	Matrix4x4 _ModelMatrix;
	uint32 _MaterialIndex;
};

struct ModelFullPushConstantData
{
	Matrix4x4 _PreviousModelMatrix;
	Matrix4x4 _CurrentModelMatrix;
	uint32 _MaterialIndex;
};

struct InstancedModelPushConstantData
{
	Vector3<float32> _WorldGridDelta;
	Padding<4> _Padding;
	uint32 _ModelFlags;
	float32 _StartFadeOutDistanceSquared;
	float32 _EndFadeOutDistanceSquared;
	uint32 _MaterialIndex;
};

struct InstancedImpostorPushConstantData
{
	Vector3<float32> _WorldGridDelta;
	Padding<4> _Padding;
	float32 _HalfWidth;
	float32 _WholeWidth;
	float32 _Height;
	uint32 _MaterialIndex;
	float32 _StartFadeInDistance;
	float32 _EndFadeInDistance;
	float32 _StartFadeOutDistance;
	float32 _EndFadeOutDistance;
};

struct TerrainPushConstantData
{
	Vector2<float32> _WorldPosition;
	Vector2<float32> _MinimumHeightMapCoordinate;
	Vector2<float32> _MaximumHeightMapCoordinate;
	uint32 _Borders;
	float32 _PatchResolutionReciprocal;
	float32 _PatchSize;
	uint32 _HeightMapTextureIndex;
	uint32 _NormalMapTextureIndex;
	uint32 _IndexMapTextureIndex;
	uint32 _BlendMapTextureIndex;
	float32 _MapResolution;
	float32 _MapResolutionReciprocal;
};

struct GrassPushConstantData
{
	Vector3<float32> _WorldGridDelta;
	Padding<4> _Padding1;
	uint32 _MaterialIndex;
	float32 _VertexFactor;
	float32 _Thickness;
	float32 _Height;
	float32 _Tilt;
	float32 _Bend;
	float32 _FadeOutDistance;
};

/*
*	Initializes the render input manager.
*/
void RenderInputManager::Initialize() NOEXCEPT
{
	//Register the "Viewport" input stream.
	RegisterInputStream
	(
		HashString("Viewport"),
		DynamicArray< VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._VertexBuffer = EMPTY_HANDLE;
				new_entry._IndexBuffer = EMPTY_HANDLE;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 3;
				new_entry._IndexCount = 0;
				new_entry._InstanceCount = 0;
			}
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);

	//Register the "ComputeFull" input stream.
	RegisterInputStream
	(
		HashString("ComputeFull"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(0)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(0)._Height;
				new_entry._DispatchDepth = 1;
			}
		},
		RenderInputStream::Mode::DISPATCH,
		nullptr
	);

	//Register the "ComputeHalf" input stream.
	RegisterInputStream
	(
		HashString("ComputeHalf"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(1)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(1)._Height;
				new_entry._DispatchDepth = 1;
			}
		},
		RenderInputStream::Mode::DISPATCH,
		nullptr
	);

	//Register the "ComputeQuarter" input stream.
	RegisterInputStream
	(
		HashString("ComputeQuarter"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(2)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(2)._Height;
				new_entry._DispatchDepth = 1;
			}
		},
		RenderInputStream::Mode::DISPATCH,
		nullptr
	);

	//Register the "ComputeEighth" input stream.
	RegisterInputStream
	(
		HashString("ComputeEighth"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(3)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(3)._Height;
				new_entry._DispatchDepth = 1;
			}
		},
		RenderInputStream::Mode::DISPATCH,
		nullptr
	);

	//Register the "ComputeSixteenth" input stream.
	RegisterInputStream
	(
		HashString("ComputeSixteenth"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(4)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(4)._Height;
				new_entry._DispatchDepth = 1;
			}
		},
		RenderInputStream::Mode::DISPATCH,
		nullptr
	);

	//Register model input streams.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> models_required_vertex_input_attribute_descriptions;

		models_required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
		models_required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Normal)));
		models_required_vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Tangent)));
		models_required_vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));

		DynamicArray<VertexInputBindingDescription> models_required_vertex_input_binding_descriptions;
		
		models_required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

		RegisterInputStream
		(
			HashString("OpaqueSingleSidedModels"),
			models_required_vertex_input_attribute_descriptions,
			models_required_vertex_input_binding_descriptions,
			sizeof(ModelFullPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialResource::Type::OPAQUE, false, input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);

		RegisterInputStream
		(
			HashString("OpaqueDoubleSidedModels"),
			models_required_vertex_input_attribute_descriptions,
			models_required_vertex_input_binding_descriptions,
			sizeof(ModelFullPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialResource::Type::OPAQUE, true, input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);

		{
			DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

			required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
			required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));

			DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

			required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

			RegisterInputStream
			(
				HashString("MaskedSingleSidedModelsDepth"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(ModelDepthPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherDepthModelInputStream(MaterialResource::Type::MASKED, false, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED,
				this
			);

			RegisterInputStream
			(
				HashString("MaskedDoubleSidedModelsDepth"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(ModelDepthPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherDepthModelInputStream(MaterialResource::Type::MASKED, true, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED,
				this
			);
		}

		RegisterInputStream
		(
			HashString("MaskedSingleSidedModelsFull"),
			models_required_vertex_input_attribute_descriptions,
			models_required_vertex_input_binding_descriptions,
			sizeof(ModelFullPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialResource::Type::MASKED, false, input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);

		RegisterInputStream
		(
			HashString("MaskedDoubleSidedModelsFull"),
			models_required_vertex_input_attribute_descriptions,
			models_required_vertex_input_binding_descriptions,
			sizeof(ModelFullPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialResource::Type::MASKED, true, input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);
	}

	//Register instanced model input streams.
	{
		{
			DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

			required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
			required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Normal)));
			required_vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));
			required_vertex_input_attribute_descriptions.Emplace(3, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 0));
			required_vertex_input_attribute_descriptions.Emplace(4, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 1));
			required_vertex_input_attribute_descriptions.Emplace(5, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 2));
			required_vertex_input_attribute_descriptions.Emplace(6, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 3));

			DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

			required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);
			required_vertex_input_binding_descriptions.Emplace(1, static_cast<uint32>(sizeof(Matrix4x4)), VertexInputBindingDescription::InputRate::Instance);

			RegisterInputStream
			(
				HashString("SingleSidedInstancedModelDepth"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(InstancedModelPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherInstancedModelInputStream(false, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED_INSTANCED,
				this
			);

			RegisterInputStream
			(
				HashString("DoubleSidedInstancedModelDepth"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(InstancedModelPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherInstancedModelInputStream(true, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED_INSTANCED,
				this
			);
		}

		{
			DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

			required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
			required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Normal)));
			required_vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Tangent)));
			required_vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));
			required_vertex_input_attribute_descriptions.Emplace(4, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 0));
			required_vertex_input_attribute_descriptions.Emplace(5, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 1));
			required_vertex_input_attribute_descriptions.Emplace(6, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 2));
			required_vertex_input_attribute_descriptions.Emplace(7, 1, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(sizeof(Vector4<float32>) * 3));

			DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

			required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);
			required_vertex_input_binding_descriptions.Emplace(1, static_cast<uint32>(sizeof(Matrix4x4)), VertexInputBindingDescription::InputRate::Instance);

			RegisterInputStream
			(
				HashString("SingleSidedInstancedModelFull"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(InstancedModelPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherInstancedModelInputStream(false, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED_INSTANCED,
				this
			);

			RegisterInputStream
			(
				HashString("DoubleSidedInstancedModelFull"),
				required_vertex_input_attribute_descriptions,
				required_vertex_input_binding_descriptions,
				sizeof(InstancedModelPushConstantData),
				[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
				{
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherInstancedModelInputStream(true, input_stream);
				},
				RenderInputStream::Mode::DRAW_INDEXED_INSTANCED,
				this
			);
		}
	}

	//Register the instanced impostor input stream.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, 0);

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vector3<float32>)), VertexInputBindingDescription::InputRate::Instance);
		
		RegisterInputStream
		(
			HashString("InstancedImpostor"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(InstancedImpostorPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherInstancedImpostorInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INSTANCED,
			this
		);
	}

	//Register the terrain input stream.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(TerrainVertex, _Position)));
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, static_cast<uint32>(offsetof(TerrainVertex, _Borders)));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(TerrainVertex)), VertexInputBindingDescription::InputRate::Vertex);

		RegisterInputStream
		(
			HashString("Terrain"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(TerrainPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherTerrainInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);
	}

	//Register the grass input stream.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, 0);
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, 0);

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vector4<float32>)), VertexInputBindingDescription::InputRate::Instance);

		RegisterInputStream
		(
			HashString("Grass"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(GrassPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherGrassInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INSTANCED,
			this
		);
	}
}

/*
*	Updates the render input manager during the render update phase.
*/
void RenderInputManager::RenderUpdate() NOEXCEPT
{
	PROFILING_SCOPE(RenderInputManager::RenderUpdate);

	//Run all gather functions.
	for (RenderInputStream &input_stream : _InputStreams)
	{
		input_stream._GatherFunction(input_stream._UserData, &input_stream);
	}
}

/*
*	Registers an input stream.
*/
void RenderInputManager::RegisterInputStream
(
	const HashString identifier,
	const DynamicArray<VertexInputAttributeDescription> &requited_vertex_input_attribute_descriptions,
	const DynamicArray<VertexInputBindingDescription> &required_vertex_input_binding_descriptions,
	const uint64 required_push_constant_data_size,
	const RenderInputStreamGatherFunction gather_function,
	const RenderInputStream::Mode mode,
	void *const RESTRICT user_data
) NOEXCEPT
{
	//Add the new input stream.
	_InputStreams.Emplace();
	RenderInputStream &new_input_stream{ _InputStreams.Back() };

	new_input_stream._Identifier = identifier;
	new_input_stream._RequiredVertexInputAttributeDescriptions = requited_vertex_input_attribute_descriptions;
	new_input_stream._RequiredVertexInputBindingDescriptions = required_vertex_input_binding_descriptions;
	new_input_stream._RequiredPushConstantDataSize = required_push_constant_data_size;
	new_input_stream._GatherFunction = gather_function;
	new_input_stream._Mode = mode;
	new_input_stream._UserData = user_data;
}

/*
*	Returns the input stream with the given identifier.
*/
NO_DISCARD const RenderInputStream &RenderInputManager::GetInputStream(const HashString identifier) NOEXCEPT
{
	for (const RenderInputStream &input_stream : _InputStreams)
	{
		if (input_stream._Identifier == identifier)
		{
			return input_stream;
		}
	}

	ASSERT(false, "Couldn't find input stream!");

	return RenderInputStream();
}

/*
*	Gathers a depth model input stream.
*/
void RenderInputManager::GatherDepthModelInputStream
(
	const MaterialResource::Type material_type,
	const bool double_sided,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForStaticModelsCulling();

		//Wait for static models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForStaticModelsLevelOfDetail();

		//Go through all components.
		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip this model if it's not visible.
			if (!component->_Visibility)
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (component->_MaterialResources[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ component->_ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelDepthPushConstantData push_constant_data;

				push_constant_data._ModelMatrix = component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = component->_MaterialResources[i]->_Index;

				for (uint64 i{ 0 }; i < sizeof(ModelDepthPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}

	//Gather dynamic models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForDynamicModelsCulling();

		//Wait for dynamic models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForDynamicModelsLevelOfDetail();

		//Go through all components.
		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip this model if it's not visible.
			if (!component->_Visibility)
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (component->_MaterialResources[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ component->_ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelDepthPushConstantData push_constant_data;

				push_constant_data._ModelMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = component->_MaterialResources[i]->_Index;

				for (uint64 i{ 0 }; i < sizeof(ModelDepthPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}

/*
*	Gathers a full model input stream.
*/
void RenderInputManager::GatherFullModelInputStream
(
	const MaterialResource::Type material_type,
	const bool double_sided,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForStaticModelsCulling();

		//Wait for static models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForStaticModelsLevelOfDetail();

		//Go through all components.
		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip this model if it's not visible.
			if (!component->_Visibility)
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (component->_MaterialResources[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ component->_ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelFullPushConstantData push_constant_data;

				push_constant_data._PreviousModelMatrix = push_constant_data._CurrentModelMatrix = component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = component->_MaterialResources[i]->_Index;

				for (uint64 i{ 0 }; i < sizeof(ModelFullPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}

	//Gather dynamic models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForDynamicModelsCulling();

		//Wait for dynamic models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForDynamicModelsLevelOfDetail();

		//Go through all components.
		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip this model if it's not visible.
			if (!component->_Visibility)
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (component->_MaterialResources[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ component->_ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelFullPushConstantData push_constant_data;

				push_constant_data._PreviousModelMatrix = component->_PreviousWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._CurrentModelMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = component->_MaterialResources[i]->_Index;

				for (uint64 i{ 0 }; i < sizeof(ModelFullPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}

/*
*	Gathers an instanced model input stream.
*/
void RenderInputManager::GatherInstancedModelInputStream
(
	const bool double_sided,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather instanced static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedStaticModelComponents() };
		const InstancedStaticModelComponent *RESTRICT component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents() };

		//Wait for culling to finish.
		CullingSystem::Instance->WaitForInstancedStaticModelsCulling();

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip if not visible.
			if (!component->_Visibility)
			{
				continue;
			}

			//Draw all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the double-sidedness.
				if (component->_MaterialResources[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ component->_ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[0]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[0]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = component->_TransformationsBuffer;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[0]._IndexCount;
				new_entry._InstanceCount = component->_NumberOfTransformations;

				//Set up the push constant data.
				InstancedModelPushConstantData push_constant_data;

				const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
				}

				push_constant_data._ModelFlags = static_cast<uint32>(component->_ModelFlags);

				if (component->_ModelFadeData.Valid())
				{
					push_constant_data._StartFadeOutDistanceSquared = component->_ModelFadeData.Get()._StartFadeOutDistance * component->_ModelFadeData.Get()._StartFadeOutDistance;
					push_constant_data._EndFadeOutDistanceSquared = component->_ModelFadeData.Get()._EndFadeOutDistance * component->_ModelFadeData.Get()._EndFadeOutDistance;
				}

				else
				{
					push_constant_data._StartFadeOutDistanceSquared = push_constant_data._EndFadeOutDistanceSquared = FLOAT32_MAXIMUM;
				}

				push_constant_data._MaterialIndex = component->_MaterialResources[i]->_Index;

				for (uint64 i{ 0 }; i < sizeof(InstancedModelPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}

/*
*	Gathers an instanced impostor input stream.
*/
void RenderInputManager::GatherInstancedImpostorInputStream
(
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather instanced static impostors.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedImpostorComponents() };
		const InstancedImpostorComponent *RESTRICT component{ ComponentManager::GetInstancedImpostorInstancedImpostorComponents() };
	
		//Wait for culling.
		CullingSystem::Instance->WaitForInstancedImpostorsCulling();

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Ignore if not visible.
			if (!TEST_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = component->_TransformationsBuffer;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = component->_NumberOfTransformations;

			//Set up the push constant data.
			InstancedImpostorPushConstantData push_constant_data;

			const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
			}

			push_constant_data._HalfWidth = component->_Dimensions._X * 0.5f;
			push_constant_data._WholeWidth = component->_Dimensions._X;
			push_constant_data._Height = component->_Dimensions._Y;
			push_constant_data._MaterialIndex = component->_MaterialResource->_Index;
			push_constant_data._StartFadeInDistance = component->_StartFadeInDistance;
			push_constant_data._EndFadeInDistance = component->_EndFadeInDistance;
			push_constant_data._StartFadeOutDistance = component->_StartFadeOutDistance;
			push_constant_data._EndFadeOutDistance = component->_EndFadeOutDistance;

			for (uint64 i{ 0 }; i < sizeof(InstancedImpostorPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}

/*
*	Gathers a terrain quad tree node.
*/
FORCE_INLINE void GatherTerrainQuadTreeNode
(
	const TerrainComponent *const RESTRICT component,
	const TerrainQuadTreeNode &node,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	if (node.IsSubdivided())
	{
		for (const TerrainQuadTreeNode &child_node : node._ChildNodes)
		{
			GatherTerrainQuadTreeNode(component, child_node, input_stream);
		}
	}

	else
	{
		//Ignore this node if it not visible.
		if (!node._Visible)
		{
			return;
		}

		//Add a new entry.
		input_stream->_Entries.Emplace();
		RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

		new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
		new_entry._VertexBuffer = component->_Buffer;
		new_entry._IndexBuffer = component->_Buffer;
		new_entry._IndexBufferOffset = component->_IndexOffset;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 0;
		new_entry._IndexCount = component->_IndexCount;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		TerrainPushConstantData push_constant_data;

		const Vector3<float32> component_world_position{ component->_WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };
		push_constant_data._WorldPosition = Vector2<float32>(component_world_position._X, component_world_position._Z) + node._Position;
		push_constant_data._MinimumHeightMapCoordinate = node._MinimumHeightMapCoordinate;
		push_constant_data._MaximumHeightMapCoordinate = node._MaximumHeightMapCoordinate;
		push_constant_data._Borders = node._Borders;
		push_constant_data._PatchResolutionReciprocal = 1.0f / static_cast<float32>(component->_BaseResolution - 2);
		push_constant_data._PatchSize = node._PatchSize;
		push_constant_data._HeightMapTextureIndex = component->_HeightMapTextureIndex;
		push_constant_data._NormalMapTextureIndex = component->_NormalMapTextureIndex;
		push_constant_data._IndexMapTextureIndex = component->_IndexMapTextureIndex;
		push_constant_data._BlendMapTextureIndex = component->_BlendMapTextureIndex;
		push_constant_data._MapResolution = static_cast<float32>(component->_HeightMap.GetResolution());
		push_constant_data._MapResolutionReciprocal = 1.0f / push_constant_data._MapResolution;

		for (uint64 i{ 0 }; i < sizeof(TerrainPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte* const RESTRICT) & push_constant_data)[i]);
		}
	}
}

/*
*	Gathers a terrain input stream.
*/
void RenderInputManager::GatherTerrainInputStream
(
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather terrains.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
		const TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForTerrainCulling();

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Is this terrain visible?
			if (!component->_Visibility)
			{
				continue;
			}

#if 1
			//Walk the quad tree.
			GatherTerrainQuadTreeNode(component, component->_QuadTree._RootNode, input_stream);
#else
			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = component->_Buffer;
			new_entry._IndexBuffer = component->_Buffer;
			new_entry._IndexBufferOffset = component->_IndexOffset;
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 0;
			new_entry._IndexCount = component->_IndexCount;
			new_entry._InstanceCount = 0;

			//Set up the push constant data.
			TerrainPushConstantData push_constant_data;

			const Vector3<float32> world_position{ component->_WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };
			push_constant_data._WorldPosition = Vector2<float32>(world_position._X, world_position._Z);
			push_constant_data._MinimumHeightMapCoordinate = Vector2<float32>(0.0f, 0.0f);
			push_constant_data._MaximumHeightMapCoordinate = Vector2<float32>(1.0f, 1.0f);
			push_constant_data._PatchSize = static_cast<float32>(component->_PatchSize);
			push_constant_data._HeightMapTextureIndex = component->_HeightMapTextureIndex;
			push_constant_data._NormalMapTextureIndex = component->_NormalMapTextureIndex;
			push_constant_data._IndexMapTextureIndex = component->_IndexMapTextureIndex;
			push_constant_data._BlendMapTextureIndex = component->_BlendMapTextureIndex;
			push_constant_data._MapResolution = static_cast<float32>(component->_HeightMap.GetResolution());
			push_constant_data._MapResolutionReciprocal = 1.0f / push_constant_data._MapResolution;

			for (uint64 i{ 0 }; i < sizeof(TerrainPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
#endif
		}
	}
}

/*
*	Gathers a grass input stream.
*/
void RenderInputManager::GatherGrassInputStream
(
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather grass.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfGrassComponents() };
		const GrassComponent *RESTRICT component{ ComponentManager::GetGrassGrassComponents() };

		//Wait for culling.
		CullingSystem::Instance->WaitForGrassCulling();

		//Wait for level of detaill.
		LevelOfDetailSystem::Instance->WaitForGrassLevelOfDetail();

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Ignore if not visible.
			if (!TEST_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = component->_InstanceBuffer;
			new_entry._VertexCount = GrassConstants::HIGHEST_VERTICES - (component->_LevelOfDetail * 2);
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = component->_NumberOfInstances;

			//Set up the push constant data.
			GrassPushConstantData push_constant_data;

			const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
			}

			push_constant_data._MaterialIndex = component->_MaterialResource->_Index;
			push_constant_data._VertexFactor = 1.0f / static_cast<float32>((new_entry._VertexCount - 1) >> 1);
			push_constant_data._Thickness = component->_Thickness;
			push_constant_data._Height = component->_Height;
			push_constant_data._Tilt = component->_Tilt;
			push_constant_data._Bend = component->_Bend;
			push_constant_data._FadeOutDistance = component->_FadeOutDistance;

			for (uint64 i{ 0 }; i < sizeof(GrassPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}