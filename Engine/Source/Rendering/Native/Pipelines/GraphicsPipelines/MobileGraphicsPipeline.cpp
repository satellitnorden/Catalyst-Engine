//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/MobileGraphicsPipeline.h>

//Components.
#include <Components/Components/StaticModelComponent.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Mobile push constant data class definition.
*/
class MobilePushConstantData final
{

public:

	//The model matrix.
	Matrix4x4 _ModelMatrix;

	//The sky light luminance.
	Vector3<float32> _SkyLightLuminance;

	//Some padding.
	Padding<4> _Padding;

	//The material index.
	uint32 _MaterialIndex;

	//The mobile index.
	uint32 _MobilePass;

};

static_assert(offsetof(MobilePushConstantData, _ModelMatrix) == 0, "Oh no!");
static_assert(offsetof(MobilePushConstantData, _SkyLightLuminance) == 64, "Oh no!");
static_assert(offsetof(MobilePushConstantData, _MaterialIndex) == 80, "Oh no!");
static_assert(offsetof(MobilePushConstantData, _MobilePass) == 84, "Oh no!");

/*
*	Initializes this graphics pipeline.
*/
void MobileGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("MobileVertexShader"))->_Handle);
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("MobileFragmentShader"))->_Handle);

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Set whether or not this graphics pipeline is rendering directly to the screen.
	SetIsRenderingDirectlyToScreen(true);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(MobilePushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(Vertex, _TextureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Vertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::CLEAR);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::CLEAR);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleList);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Mobile");
#endif

	//Finalize the pipeline.
	FinalizePipeline();

	//Create the sky vertex/index buffer.
	{
		constexpr StaticArray<Vector3<float32>, 3> VERTEX_POSITIONS
		{
			Vector3<float32>(-1.0f, -1.0f, FLOAT32_EPSILON),
			Vector3<float32>(3.0f, -1.0f, FLOAT32_EPSILON),
			Vector3<float32>(-1.0f, 3.0f, FLOAT32_EPSILON)
		};

		StaticArray<Vertex, 3> vertices;

		for (int32 i{ 0 }; i < 3; ++i)
		{
			vertices[i]._Position = VERTEX_POSITIONS[i];
			vertices[i]._Normal = Vector3<float32>(1.0f, 0.0f, 0.0f);
			vertices[i]._Tangent = Vector3<float32>(0.0f, 1.0f, 0.0f);
			vertices[i]._TextureCoordinate = Vector2<float32>((vertices[i]._Position._X + 1.0f) * 0.5f, (vertices[i]._Position._Y + 1.0f) * 0.5f);
		}

		const void *const RESTRICT data_chunks[]{ vertices.Data() };
		const uint64 data_sizes[]{ sizeof(Vertex) * vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &_SkyVertexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_SkyVertexBuffer);
	}

	{
		constexpr StaticArray<uint32, 3> INDICES
		{
			static_cast<uint32>(0),
			static_cast<uint32>(1),
			static_cast<uint32>(2)
		};

		const void* const RESTRICT data_chunks[]{ INDICES.Data() };
		const uint64 data_sizes[]{ sizeof(uint32) * INDICES.Size() };
		RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_SkyIndexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_SkyIndexBuffer);
	}
}

/*
*	Executes this graphics pipeline.
*/
void MobileGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());

	//Cache the sky light luminance.
	Vector3<float32> sky_light_luminance{ VectorConstants::ZERO };

	/*
	{
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
		{
			if (component->_LightType == LightType::DIRECTIONAL)
			{
				sky_light_luminance = component->_Color * component->_Intensity;

				break;
			}
		}
	}
	*/

	//Draw static models.
	{
		for (StaticModelInstanceData &instance_data : StaticModelComponent::Instance->InstanceData())
		{
			//Skip this model depending on visibility.
			if (!TEST_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Draw all meshes.
			for (uint64 mesh_index{ 0 }, size{ instance_data._ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Skip this mesh if it's hidden.
				if (!TEST_BIT(instance_data._MeshesVisibleMask, BIT(mesh_index)))
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ instance_data._ModelResource->_Meshes[mesh_index] };

				//Push constants.
				MobilePushConstantData data;

				ASSERT(false, "This code is not ready. :x");
				//data._ModelMatrix = instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				data._SkyLightLuminance = sky_light_luminance;
				data._MaterialIndex = instance_data._MaterialResources[mesh_index]->_Index;
				data._MobilePass = UNDERLYING(MobilePass::MODEL);

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(MobilePushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._IndexCount, 1);
			}
		}
	}

	//Draw the sky.
	if (WorldSystem::Instance->GetSkySystem()->GetSkyEnabled())
	{
		//Push constants.
		MobilePushConstantData data;

		data._ModelMatrix = Matrix4x4();
		data._SkyLightLuminance = sky_light_luminance;
		data._MaterialIndex = 0;
		data._MobilePass = UNDERLYING(MobilePass::SKY);

		command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(MobilePushConstantData), &data);

		//Bind the vertex/inder buffer.
		command_buffer->BindVertexBuffer(this, 0, _SkyVertexBuffer, &OFFSET);
		command_buffer->BindIndexBuffer(this, _SkyIndexBuffer, OFFSET);

		//Draw!
		command_buffer->DrawIndexed(this, 3, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void MobileGraphicsPipeline::Terminate() NOEXCEPT
{

}