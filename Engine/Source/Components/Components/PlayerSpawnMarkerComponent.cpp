//Header file.
#include <Components/Components/PlayerSpawnMarkerComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Content.
#include <Content/Assets/ModelAsset.h>

//Editor.
#include <Editor/EditorUtilities.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

//Systems.
#if defined(CATALYST_EDITOR)
#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/ContentSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Player spawn marker push constant data.
*/
class PlayerSpawnMarkerPushConstantData final
{

public:

	//The transformation.
	Matrix4x4 _Transformation;

};

/*
*	Gathers a player spawn marker render input stream.
*/
void GatherPlayerSpawnMarkerRenderInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
#if defined(CATALYST_EDITOR)
	const bool SHOULD_RENDER{ !CatalystEditorSystem::Instance->IsInGame() };
#else
	constexpr bool SHOULD_RENDER{ false };
#endif

	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	if (SHOULD_RENDER)
	{
		//Cache the model.
		AssetPointer<ModelAsset> model{ ContentSystem::Instance->GetAsset<ModelAsset>(HashString("PlayerSpawnMarker")) };

		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < PlayerSpawnMarkerComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			//Cache data.
			Entity *const RESTRICT entity{ PlayerSpawnMarkerComponent::Instance->InstanceToEntity(instance_index) };
			const PlayerSpawnMarkerInstanceData &circle_marker_instance_data{ PlayerSpawnMarkerComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Iterate over the meshes.
			for (const Mesh &mesh : model->_Meshes)
			{
				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[0]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[0]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[0]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				PlayerSpawnMarkerPushConstantData push_constant_data;

				push_constant_data._Transformation = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				
				for (uint64 i{ 0 }; i < sizeof(PlayerSpawnMarkerPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}

/*
*	Initializes this component.
*/
void PlayerSpawnMarkerComponent::Initialize() NOEXCEPT
{
	{
		//Set up the required vertex input attribute/binding descriptions for models.
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Normal)));
		required_vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Tangent)));
		required_vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

		//Register the input stream.
		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("PlayerSpawnMarker"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(PlayerSpawnMarkerPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				GatherPlayerSpawnMarkerRenderInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			nullptr
		);
	}
}

/*
*	Creates an instance.
*/
void PlayerSpawnMarkerComponent::CreateInstance(Entity *const RESTRICT entity, PlayerSpawnMarkerInitializationData *const RESTRICT initialization_data, PlayerSpawnMarkerInstanceData *const RESTRICT instance_data) NOEXCEPT
{

}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void PlayerSpawnMarkerComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Circle marker component needs a world transform component!");
}

/*
*	Performs an editor selection.
*/
NO_DISCARD bool PlayerSpawnMarkerComponent::EditorSelect(const Ray& ray, Entity* const RESTRICT entity, float32* const RESTRICT hit_distance) NOEXCEPT
{
	//Cache the instance data.
	const PlayerSpawnMarkerInstanceData &instance_data{ InstanceData(entity) };
	const WorldTransformInstanceData &world_transform_data{ WorldTransformComponent::Instance->InstanceData(entity) };

	//Cache the model transform.
	const Matrix4x4 model_transform{ world_transform_data._CurrentWorldTransform.ToAbsoluteMatrix4x4() };

	//Cache the model.
	AssetPointer<ModelAsset> model{ ContentSystem::Instance->GetAsset<ModelAsset>(HashString("PlayerSpawnMarker")) };

	//Cast the selection ray!
	return EditorUtilities::SelectionRay(ray, model_transform, model.Get(), hit_distance);
}

/*
*	Destroys an instance.
*/
void PlayerSpawnMarkerComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns a random world transform.
*/
NO_DISCARD WorldTransform PlayerSpawnMarkerComponent::GetRandomWorldTransform() NOEXCEPT
{
	if (NumberOfInstances() > 0)
	{
		const uint64 random_index{ CatalystRandomMath::RandomIntegerInRange<uint64>(0, NumberOfInstances() - 1) };

		return WorldTransformComponent::Instance->InstanceData(InstanceToEntity(random_index))._CurrentWorldTransform;
	}

	else
	{
		return WorldTransform();
	}
}