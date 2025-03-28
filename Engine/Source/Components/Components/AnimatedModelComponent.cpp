//Header file.
#include <Components/Components/AnimatedModelComponent.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Animated model push constant data.
*/
class AnimatedModelPushConstantData final
{

public:

	//The previous transformation.
	Matrix4x4 _PreviousTransformation;

	//The current transformation.
	Matrix4x4 _CurrentTransformation;

};

/*
*	Initializes this component.
*/
void AnimatedModelComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableAnimatedModelAssetField
	(
		"Model",
		offsetof(AnimatedModelInitializationData, _Model),
		offsetof(AnimatedModelInstanceData, _Model)
	);

	//Register the "AnimatedModels" render input stream.
	{
		//Set up the vertex input attribute descriptions.
		DynamicArray<VertexInputAttributeDescription> vertex_input_attribute_descriptions;

		vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Position)));
		vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Normal)));
		vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Tangent)));
		vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32Z32W32UnsignedInt, static_cast<uint32>(offsetof(AnimatedVertex, _BoneIndices)));
		vertex_input_attribute_descriptions.Emplace(4, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _BoneWeights)));
		vertex_input_attribute_descriptions.Emplace(5, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _TextureCoordinate)));

		//Set up the vertex input binding descriptions.
		DynamicArray<VertexInputBindingDescription> vertex_input_binding_descriptions;

		vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(AnimatedVertex)), VertexInputBindingDescription::InputRate::Vertex);

		//Register the input stream!
		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("AnimatedModels"),
			vertex_input_attribute_descriptions,
			vertex_input_binding_descriptions,
			sizeof(AnimatedModelPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				static_cast<AnimatedModelComponent *const RESTRICT>(user_data)->GatherAnimatedModelInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			this
		);
	}
}

/*
*	Creates an instance.
*/
void AnimatedModelComponent::CreateInstance(Entity *const RESTRICT entity, AnimatedModelInitializationData *const RESTRICT initialization_data, AnimatedModelInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Model = initialization_data->_Model;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void AnimatedModelComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Animated model component needs a world transform component!");

	//Cache the instance data.
	const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
	AnimatedModelInstanceData &animated_model_instance_data{ InstanceData(entity) };
}

/*
*	Destroys an instance.
*/
void AnimatedModelComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Gathers the animated model input stream.
*/
void AnimatedModelComponent::GatherAnimatedModelInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Go through all instances.
	for (uint64 instance_index{ 0 }; instance_index < NumberOfInstances(); ++instance_index)
	{
		//Cache data.
		Entity *const RESTRICT entity{ InstanceToEntity(instance_index) };
		const AnimatedModelInstanceData &animated_model_instance_data{ _InstanceData[instance_index] };
		const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

		//Skip if there's no model.
		if (!animated_model_instance_data._Model)
		{
			continue;
		}

		//Add a new entry.
		input_stream->_Entries.Emplace();
		RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

		new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
		new_entry._VertexBuffer = animated_model_instance_data._Model->_VertexBuffer;
		new_entry._IndexBuffer = animated_model_instance_data._Model->_IndexBuffer;
		new_entry._IndexBufferOffset = 0;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 0;
		new_entry._IndexCount = animated_model_instance_data._Model->_IndexCount;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		AnimatedModelPushConstantData push_constant_data;

		push_constant_data._PreviousTransformation = world_transform_instance_data._PreviousWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
		push_constant_data._CurrentTransformation = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

		for (uint64 i{ 0 }; i < sizeof(AnimatedModelPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}
}