//Header file.
#include <Rendering/Native/RenderInputManager.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Components.
#include <Components/Components/GrassComponent.h>
#include <Components/Components/InstancedImpostorComponent.h>
#include <Components/Components/InstancedStaticModelComponent.h>
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/TerrainComponent.h>
#include <Components/Components/WaterComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/GrassCore.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//User interface.
#include <UserInterface/UserInterfaceUtilities.h>

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

struct WaterPushConstantData
{
	Vector2<float32> _WorldPosition;
	uint32 _Borders;
	float32 _PatchResolutionReciprocal;
	float32 _PatchSize;
	uint32 _TextureIndex;
};

struct GrassPushConstantData
{
	Vector3<float32> _WorldGridDelta;
	Padding<4> _Padding1;
	Vector3<float32> _Minimum;
	Padding<4> _Padding2;
	Vector3<float32> _Maximum;
	Padding<4> _Padding3;
	uint32 _MaterialIndex;
	float32 _VertexFactor;
	float32 _Thickness;
	float32 _Height;
	float32 _Tilt;
	float32 _Bend;
	float32 _FadeOutDistance;
};

struct UserInterfaceTextPushConstantData
{
	Vector4<float32> _ColorOpacity;
	Vector2<float32> _BoundsMinimum;
	Vector2<float32> _BoundsMaximum;
	Vector2<float32> _TextureMinimum;
	Vector2<float32> _TextureMaximum;
	uint32 _TextureIndex;
	float32 _SmoothingFactor;
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

	//Register the "RayTracingViewportHalf" input stream.
	RegisterInputStream
	(
		HashString("RayTracingViewportHalf"),
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
				new_entry._TraceRaysWidth = RenderingSystem::Instance->GetScaledResolution(1)._Width;
				new_entry._TraceRaysHeight = RenderingSystem::Instance->GetScaledResolution(1)._Height;

			}
		},
		RenderInputStream::Mode::TRACE_RAYS,
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
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialAsset::Type::OPAQUE, false, input_stream);
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
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialAsset::Type::OPAQUE, true, input_stream);
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
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherDepthModelInputStream(MaterialAsset::Type::MASKED, false, input_stream);
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
					static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherDepthModelInputStream(MaterialAsset::Type::MASKED, true, input_stream);
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
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialAsset::Type::MASKED, false, input_stream);
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
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherFullModelInputStream(MaterialAsset::Type::MASKED, true, input_stream);
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

	//Register the water input stream.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(TerrainVertex, _Position)));
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, static_cast<uint32>(offsetof(TerrainVertex, _Borders)));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(TerrainVertex)), VertexInputBindingDescription::InputRate::Vertex);

		RegisterInputStream
		(
			HashString("Water"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(WaterPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherWaterInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);
	}

	//Register the grass input stream.
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, 0);
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, static_cast<uint32>(sizeof(uint32) * 1));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(uint32) * 2), VertexInputBindingDescription::InputRate::Instance);

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

	//Register the user interface text input stream.
	RegisterInputStream
	(
		HashString("UserInterfaceText"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(UserInterfaceTextPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			static_cast<RenderInputManager *const RESTRICT>(user_data)->GatherUserInterfaceTextInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW,
		this
	);
}

/*
*	Updates the render input manager during the render update phase.
*/
void RenderInputManager::RenderUpdate() NOEXCEPT
{
	PROFILING_SCOPE("RenderInputManager::RenderUpdate");

	//Run all gather functions.
	for (RenderInputStream &input_stream : _InputStreams)
	{
		while (!input_stream._Task.IsExecuted())
		{
			TaskSystem::Instance->DoWork(Task::Priority::HIGH);
		}

		input_stream._Task._Arguments = &input_stream;
		TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &input_stream._Task);
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
#if !defined(CATALYST_CONFIGURATION_FINAL)
	for (const RenderInputStream &input_stream : _InputStreams)
	{
		if (input_stream._Identifier == identifier)
		{
			ASSERT(false, "Already has input stream!");
		}
	}
#endif

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

	//Set up the task.
	new_input_stream._Task._Function = [](void *const RESTRICT arguments)
	{
		PROFILING_SCOPE("GatherInputStream");

		RenderInputStream *const RESTRICT render_input_stream{ static_cast<RenderInputStream *const RESTRICT>(arguments) };

		render_input_stream->_GatherFunction(render_input_stream->_UserData, render_input_stream);
	};
	new_input_stream._Task._Arguments = nullptr; //Filled in later.
	new_input_stream._Task._ExecutableOnSameThread = false;
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
			while (!input_stream._Task.IsExecuted())
			{
				TaskSystem::Instance->DoWork(Task::Priority::HIGH);
			}

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
	const MaterialAsset::Type material_type,
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
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < StaticModelComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			Entity *const RESTRICT entity{ StaticModelComponent::Instance->InstanceToEntity(instance_index) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Skip this model if it's not visible.
			if (!TEST_BIT(static_model_instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ static_model_instance_data._Model->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (static_model_instance_data._Materials[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (static_model_instance_data._Materials[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ static_model_instance_data._Model->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelDepthPushConstantData push_constant_data;

				push_constant_data._ModelMatrix = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = static_model_instance_data._Materials[i]->_Index;

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
	const MaterialAsset::Type material_type,
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
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < StaticModelComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			Entity *const RESTRICT entity{ StaticModelComponent::Instance->InstanceToEntity(instance_index) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Skip this model if it's not visible.
			if (!TEST_BIT(static_model_instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ static_model_instance_data._Model->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (static_model_instance_data._Materials[i]->_Type != material_type)
				{
					continue;
				}

				//Skip this mesh depending on double sided-ness.
				if (static_model_instance_data._Materials[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ static_model_instance_data._Model->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ModelFullPushConstantData push_constant_data;

				push_constant_data._PreviousModelMatrix = world_transform_instance_data._PreviousWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._CurrentModelMatrix = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._MaterialIndex = static_model_instance_data._Materials[i]->_Index;

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
		for (InstancedStaticModelInstanceData &instance_data : InstancedStaticModelComponent::Instance->InstanceData())
		{
			//Skip if not visible.
			if (!instance_data._Visibility)
			{
				continue;
			}

			//Draw all meshes.
			for (uint64 i{ 0 }, size{ instance_data._Model->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the double-sidedness.
				if (instance_data._Materials[i]->_DoubleSided != double_sided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ instance_data._Model->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[0]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[0]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = instance_data._TransformationsBuffer;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[0]._IndexCount;
				new_entry._InstanceCount = instance_data._NumberOfTransformations;

				//Set up the push constant data.
				InstancedModelPushConstantData push_constant_data;

				const Vector3<int32> delta{ instance_data._Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
				}

				push_constant_data._ModelFlags = static_cast<uint32>(instance_data._ModelFlags);

				if (instance_data._ModelFadeData.Valid())
				{
					push_constant_data._StartFadeOutDistanceSquared = instance_data._ModelFadeData.Get()._StartFadeOutDistance * instance_data._ModelFadeData.Get()._StartFadeOutDistance;
					push_constant_data._EndFadeOutDistanceSquared = instance_data._ModelFadeData.Get()._EndFadeOutDistance * instance_data._ModelFadeData.Get()._EndFadeOutDistance;
				}

				else
				{
					push_constant_data._StartFadeOutDistanceSquared = push_constant_data._EndFadeOutDistanceSquared = FLOAT32_MAXIMUM;
				}

				push_constant_data._MaterialIndex = instance_data._Materials[i]->_Index;

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
		for (InstancedImpostorInstanceData &instance_data : InstancedImpostorComponent::Instance->InstanceData())
		{
			//Ignore if not visible.
			if (!TEST_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
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
			new_entry._InstanceBuffer = instance_data._TransformationsBuffer;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = instance_data._NumberOfTransformations;

			//Set up the push constant data.
			InstancedImpostorPushConstantData push_constant_data;

			const Vector3<int32> delta{ instance_data._Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
			}

			push_constant_data._HalfWidth = instance_data._Dimensions._X * 0.5f;
			push_constant_data._WholeWidth = instance_data._Dimensions._X;
			push_constant_data._Height = instance_data._Dimensions._Y;
			push_constant_data._MaterialIndex = instance_data._Material->_Index;
			push_constant_data._StartFadeInDistance = instance_data._StartFadeInDistance;
			push_constant_data._EndFadeInDistance = instance_data._EndFadeInDistance;
			push_constant_data._StartFadeOutDistance = instance_data._StartFadeOutDistance;
			push_constant_data._EndFadeOutDistance = instance_data._EndFadeOutDistance;

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
	const TerrainInstanceData &instance_data,
	const TerrainQuadTreeNode &node,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	if (node.IsSubdivided())
	{
		for (const TerrainQuadTreeNode &child_node : node._ChildNodes)
		{
			GatherTerrainQuadTreeNode(instance_data, child_node, input_stream);
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
		new_entry._VertexBuffer = instance_data._Buffer;
		new_entry._IndexBuffer = instance_data._Buffer;
		new_entry._IndexBufferOffset = instance_data._IndexOffset;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 0;
		new_entry._IndexCount = instance_data._IndexCount;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		TerrainPushConstantData push_constant_data;

		const Vector3<float32> component_world_position{ instance_data._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };
		push_constant_data._WorldPosition = Vector2<float32>(component_world_position._X, component_world_position._Z) + node._Position;
		push_constant_data._MinimumHeightMapCoordinate = node._MinimumHeightMapCoordinate;
		push_constant_data._MaximumHeightMapCoordinate = node._MaximumHeightMapCoordinate;
		push_constant_data._Borders = node._Borders;
		push_constant_data._PatchResolutionReciprocal = 1.0f / static_cast<float32>(instance_data._BaseResolution);
		push_constant_data._PatchSize = node._PatchSize;
		push_constant_data._HeightMapTextureIndex = instance_data._HeightMapTextureIndex;
		push_constant_data._NormalMapTextureIndex = instance_data._NormalMapTextureIndex;
		push_constant_data._IndexMapTextureIndex = instance_data._IndexMapTextureIndex;
		push_constant_data._BlendMapTextureIndex = instance_data._BlendMapTextureIndex;
		push_constant_data._MapResolution = static_cast<float32>(instance_data._HeightMap.GetResolution());
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
		for (const TerrainInstanceData &instance_data : TerrainComponent::Instance->InstanceData())
		{
			//Is this terrain visible?
			if (!instance_data._Visibility)
			{
				continue;
			}

			//Walk the quad tree.
			GatherTerrainQuadTreeNode(instance_data, instance_data._QuadTree._RootNode, input_stream);
		}
	}
}

/*
*	Gathers a water quad tree node.
*/
FORCE_INLINE void GatherWaterQuadTreeNode
(
	const WaterInstanceData &instance_data,
	const TerrainQuadTreeNode &node,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	if (node.IsSubdivided())
	{
		for (const TerrainQuadTreeNode &child_node : node._ChildNodes)
		{
			GatherWaterQuadTreeNode(instance_data, child_node, input_stream);
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
		new_entry._VertexBuffer = instance_data._Buffer;
		new_entry._IndexBuffer = instance_data._Buffer;
		new_entry._IndexBufferOffset = instance_data._IndexOffset;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 0;
		new_entry._IndexCount = instance_data._IndexCount;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		WaterPushConstantData push_constant_data;

		const Vector3<float32> component_world_position{ instance_data._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };
		push_constant_data._WorldPosition = Vector2<float32>(component_world_position._X, component_world_position._Z) + node._Position;
		push_constant_data._Borders = node._Borders;
		push_constant_data._PatchResolutionReciprocal = 1.0f / static_cast<float32>(instance_data._BaseResolution);
		push_constant_data._PatchSize = node._PatchSize;
		push_constant_data._TextureIndex = instance_data._Texture->_Index;

		for (uint64 i{ 0 }; i < sizeof(WaterPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}
}

/*
*	Gathers a water input stream.
*/
void RenderInputManager::GatherWaterInputStream
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
		for (const WaterInstanceData &instance_data : WaterComponent::Instance->InstanceData())
		{
			//Is this terrain visible?
			if (!instance_data._Visibility)
			{
				continue;
			}

			//Walk the quad tree.
			GatherWaterQuadTreeNode(instance_data, instance_data._QuadTree._RootNode, input_stream);
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

	//Cache camera properties.
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };

	//Gather grass.
	for (GrassInstanceData &instance_data : GrassComponent::Instance->InstanceData())
	{
		//Ignore if not visible.
		if (!TEST_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
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
		new_entry._InstanceBuffer = instance_data._InstanceBuffer;
		new_entry._VertexCount = GrassConstants::HIGHEST_VERTICES - (instance_data._LevelOfDetail * 2);
		new_entry._IndexCount = 0;
		new_entry._InstanceCount = instance_data._NumberOfInstances;

		//Calculate the sort value.
		{
			const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(WorldSystem::Instance->GetCurrentWorldGridCell()) };
			const Vector3<float32> closest_point{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };

			new_entry._SortValue = Vector3<float32>::LengthSquared(closest_point - camera_local_position);
		}

		//Set up the push constant data.
		GrassPushConstantData push_constant_data;

		const Vector3<int32> delta{ instance_data._WorldGridCell - WorldSystem::Instance->GetCurrentWorldGridCell() };

		for (uint8 i{ 0 }; i < 3; ++i)
		{
			push_constant_data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
		}

		push_constant_data._Minimum = instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum.GetLocalPosition();
		push_constant_data._Maximum = instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum.GetLocalPosition();
		push_constant_data._MaterialIndex = instance_data._Material->_Index;
		push_constant_data._VertexFactor = 1.0f / static_cast<float32>((new_entry._VertexCount - 1) >> 1);
		push_constant_data._Thickness = instance_data._Thickness;
		push_constant_data._Height = instance_data._Height;
		push_constant_data._Tilt = instance_data._Tilt;
		push_constant_data._Bend = instance_data._Bend;
		push_constant_data._FadeOutDistance = instance_data._FadeOutDistance;

		for (uint64 i{ 0 }; i < sizeof(GrassPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}

	//Sort.
	if (!input_stream->_Entries.Empty())
	{
		SortingAlgorithms::StandardSort<RenderInputStreamEntry>
		(
			input_stream->_Entries.Begin(),
			input_stream->_Entries.End(),
			nullptr,
			[](const void *const RESTRICT user_data, const RenderInputStreamEntry *const RESTRICT first, const RenderInputStreamEntry *const RESTRICT second)
			{
				return first->_SortValue < second->_SortValue;
			}
		);
	}
}

/*
*	Gathers a user interface text input stream.
*/
void RenderInputManager::GatherUserInterfaceTextInputStream
(
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Calculate the aspect ratio reciprocal.
	const float32 aspect_ratio_reciprocal{ 1.0f / RenderingSystem::Instance->GetFullAspectRatio() };

	//Gather user interface text primitives.
	for (const UserInterfacePrimitive *const RESTRICT primitive : *UserInterfaceSystem::Instance->GetUserInterfacePrimitives())
	{
		//Don't care about primitives that aren't visible.
		if (primitive->_Opacity <= 0.0f)
		{
			continue;
		}

		//Don't care about primitives that aren't text.
		if (primitive->_Type != UserInterfacePrimitiveType::TEXT)
		{
			continue;
		}
		
		//Cast to the proper type primitive.
		const TextUserInterfacePrimitive *const RESTRICT type_primitive{ static_cast<const TextUserInterfacePrimitive *const RESTRICT>(primitive) };

		//Calculate the aligned minimum/maximum.
		Vector2<float32> aligned_minimum;
		Vector2<float32> aligned_maximum;

		UserInterfaceUtilities::CalculateAlignedBoundingBox
		(
			type_primitive->_Minimum,
			type_primitive->_Maximum,
			type_primitive->_Font,
			type_primitive->_Text,
			type_primitive->_Scale,
			type_primitive->_HorizontalAlignment,
			type_primitive->_VerticalAlignment,
			&aligned_minimum,
			&aligned_maximum
		);

		//Gather all characters.
		float32 current_offset{ 0.0f };

		for (uint64 i{ 0 }, length{ type_primitive->_Text.Length() }; i < length; ++i)
		{
			//Cache the chartacter.
			const char character{ type_primitive->_Text[i] };

			//Only draw if it's a valid character.
			if (character == '\n')
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
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 4;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = 0;

			//Set up the push constant data.
			UserInterfaceTextPushConstantData push_constant_data;

			push_constant_data._ColorOpacity = Vector4<float32>(1.0f, 1.0f, 1.0f, type_primitive->_Opacity);
			push_constant_data._BoundsMinimum._X = aligned_minimum._X + current_offset;
			push_constant_data._BoundsMinimum._Y = aligned_minimum._Y;
			push_constant_data._BoundsMaximum._X = aligned_minimum._X + current_offset + type_primitive->_Font->_CharacterDescriptions[character]._Size._X * type_primitive->_Scale * aspect_ratio_reciprocal;
			push_constant_data._BoundsMaximum._Y = aligned_minimum._Y + type_primitive->_Font->_CharacterDescriptions[character]._Size._Y * type_primitive->_Scale;

			push_constant_data._BoundsMinimum._X += type_primitive->_Font->_CharacterDescriptions[character]._Offset._X * type_primitive->_Scale * aspect_ratio_reciprocal;
			push_constant_data._BoundsMaximum._X += type_primitive->_Font->_CharacterDescriptions[character]._Offset._X * type_primitive->_Scale * aspect_ratio_reciprocal;

			push_constant_data._BoundsMinimum._Y += type_primitive->_Font->_CharacterDescriptions[character]._Offset._Y * type_primitive->_Scale;
			push_constant_data._BoundsMaximum._Y += type_primitive->_Font->_CharacterDescriptions[character]._Offset._Y * type_primitive->_Scale;
			
			push_constant_data._TextureMinimum = type_primitive->_Font->_CharacterDescriptions[character]._TextureBounds._Minimum;
			push_constant_data._TextureMaximum = type_primitive->_Font->_CharacterDescriptions[character]._TextureBounds._Maximum;
			push_constant_data._TextureIndex = type_primitive->_Font->_MasterTextureIndex;
			push_constant_data._SmoothingFactor = 0.3'25f;

			for (uint64 i{ 0 }; i < sizeof(UserInterfaceTextPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}

			//Update the current offset.
			current_offset += type_primitive->_Font->_CharacterDescriptions[character]._Advance * type_primitive->_Scale * aspect_ratio_reciprocal;
		}
	}
}