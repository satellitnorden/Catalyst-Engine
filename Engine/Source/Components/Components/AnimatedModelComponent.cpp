//Header file.
#include <Components/Components/AnimatedModelComponent.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems
#include <Systems/CatalystEngineSystem.h>
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

	//The start bone transform.
	uint32 _StartBoneTransform;

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

	AddEditableAnimationAssetField
	(
		"Animation",
		offsetof(AnimatedModelInitializationData, _InitialAnimation),
		offsetof(AnimatedModelInstanceData, _CurrentAnimation)
	);

	//Register the "AnimatedModels" render input stream.
	{
		//Set up the vertex input attribute descriptions.
		DynamicArray<VertexInputAttributeDescription> vertex_input_attribute_descriptions;

		vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Position)));
		vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Normal)));
		vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _Tangent)));
		vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32Z32W32UnsignedInt, static_cast<uint32>(offsetof(AnimatedVertex, _BoneIndices)));
		vertex_input_attribute_descriptions.Emplace(4, 0, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, static_cast<uint32>(offsetof(AnimatedVertex, _BoneWeights)));
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
*	Post-initializes this component.
*/
void AnimatedModelComponent::PostInitialize() NOEXCEPT
{
	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("AnimationBoneTransforms"),
		sizeof(Matrix4x4) * 1'024,
		[](DynamicArray<byte> *const RESTRICT data, void *const RESTRICT arguments)
		{
			const DynamicArray<Matrix4x4> *const RESTRICT final_bone_transforms{ static_cast<const DynamicArray<Matrix4x4> *const RESTRICT>(arguments) };

			if (final_bone_transforms->Empty())
			{
				return;
			}

			data->Resize<false>(sizeof(Matrix4x4) * final_bone_transforms->Size());
			Memory::Copy(data->Data(), final_bone_transforms->Data(), sizeof(Matrix4x4) * final_bone_transforms->Size());
		},
		&_FinalBoneTransforms
	);
}

/*
*	Creates an instance.
*/
void AnimatedModelComponent::CreateInstance(Entity *const RESTRICT entity, AnimatedModelInitializationData *const RESTRICT initialization_data, AnimatedModelInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Model = initialization_data->_Model;
	instance_data->_CurrentAnimation = initialization_data->_InitialAnimation;
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
*	Updates this component.
*/
void AnimatedModelComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
	ASSERT(update_phase == UpdatePhase::PRE_RENDER, "Wrong update phase!");

	//Iterate over the instances.
	for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
	{
		//Cache the instance data.
		AnimatedModelInstanceData &instance_data{ _InstanceData[instance_index] };

		//No skeleton, nothing to skin. :x
		if (!instance_data._Model)
		{
			continue;
		}

		//Ensure the correct size of the final bone transforms.
		instance_data._FinalBoneTransforms.Resize<false>(instance_data._Model->_Skeleton._Bones.Size());

		//If there is a current animation - Animate. Otherwise, just set to identity.
		if (instance_data._CurrentAnimation)
		{
			//Walk the skeleton.
			for (uint64 bone_index{ 0 }; bone_index < instance_data._Model->_Skeleton._Bones.Size(); ++bone_index)
			{
				//Cache the bone.
				const Bone &bone{ instance_data._Model->_Skeleton._Bones[bone_index] };

				//Find the channel for this bone.
				for (const AnimationChannel &channel : instance_data._CurrentAnimation->_Channels)
				{
					if (channel._BoneIdentifier == bone._Name)
					{
						//Retrieve the current bone transform.
						BoneTransform current_bone_transform;

						for (const AnimationKeyframe &keyframe : channel._Keyframes)
						{
							if (instance_data._CurrentAnimationTime >= keyframe._Timestamp)
							{
								current_bone_transform = keyframe._BoneTransform;
							}
						}

						//Construct the local transform.
						//const Matrix4x4 local_transform{ current_bone_transform._Translation, current_bone_transform._Rotation, current_bone_transform._Scale };
						const Matrix4x4 local_transform{ Vector3<float32>(0.0f, 0.0f, 0.0f), current_bone_transform._Rotation, current_bone_transform._Scale };

						//Add the final bone transform.
						instance_data._FinalBoneTransforms[bone._Index] = local_transform;

						break;
					}
				}
			}

			//Update the current animation time.
			instance_data._CurrentAnimationTime += CatalystEngineSystem::Instance->GetDeltaTime();

			while (instance_data._CurrentAnimationTime >= instance_data._CurrentAnimation->_Duration)
			{
				instance_data._CurrentAnimationTime -= instance_data._CurrentAnimation->_Duration;
			}
		}
		
		else
		{
			for (Matrix4x4 &final_bone_transform : instance_data._FinalBoneTransforms)
			{
				final_bone_transform = MatrixConstants::IDENTITY;
			}
		}
	}
}

/*
*	Runs after the given update phase.
*/
void AnimatedModelComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{
	ASSERT(update_phase == UpdatePhase::PRE_RENDER, "Wrong update phase!");

	//Clear the final bone transforms.
	_FinalBoneTransforms.Clear();

	//Calculate the number of final bone transforms.
	uint64 number_of_final_bone_transforms{ 0 };

	for (const AnimatedModelInstanceData &instance_data : _InstanceData)
	{
		number_of_final_bone_transforms += instance_data._FinalBoneTransforms.Size();
	}

	//Nothing to to here if there's no final bone transforms.
	if (number_of_final_bone_transforms == 0)
	{
		return;
	}

	//Resize to fit the number of final bone transforms.
	_FinalBoneTransforms.Resize<false>(number_of_final_bone_transforms);

	//Add all final bone transforms!
	uint32 start_bone_transform{ 0 };

	for (AnimatedModelInstanceData &instance_data : _InstanceData)
	{
		Memory::Copy(&_FinalBoneTransforms[start_bone_transform], instance_data._FinalBoneTransforms.Data(), sizeof(Matrix4x4) * instance_data._FinalBoneTransforms.Size());
		instance_data._StartBoneTransform = start_bone_transform;
		start_bone_transform += static_cast<uint32>(instance_data._FinalBoneTransforms.Size());
	}
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
		push_constant_data._StartBoneTransform = animated_model_instance_data._StartBoneTransform;

		for (uint64 i{ 0 }; i < sizeof(AnimatedModelPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}
}