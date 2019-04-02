#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/BinaryFile.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/Pipelines/RenderPasses.h>
#include <Rendering/Translation/Vulkan/VulkanFrameData.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassMainStageData.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassSubStageData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/LightingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

//Vulkan rendering system data.
namespace VulkanRenderingSystemData
{
	/*
	*	Vulkan destruction data definition.
	*/
	class VulkanDestructionData final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			ConstantBuffer,
			RenderDataTable,
			Texture2D,
			UniformBuffer
		};

		//The number of frames since destruction was requested.
		uint8 _Frames{ 0 };

		//The type.
		Type _Type;

		//The handle.
		OpaqueHandle _Handle;

		/*
		*	Constructor taking all values as arguments.
		*/
		VulkanDestructionData(const Type type, OpaqueHandle handle) NOEXCEPT
			:
			_Type(type),
			_Handle(handle)
		{

		}

	};

	//Container for all shader modules.
	StaticArray<VulkanShaderModule *RESTRICT, UNDERLYING(Shader::NumberOfShaders)> _ShaderModules;

	//Container for all Vulkan render pass main stage data.
	StaticArray<VulkanRenderPassMainStageData, UNDERLYING(RenderPassMainStage::NumberOfRenderPassMainStages)> _VulkanRenderPassMainStageData;

	//Container for all Vulkan render pass data.
	StaticArray<VulkanRenderPassSubStageData, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)> _VulkanRenderPassSubStageData;

	//The Vulkan frame data.
	VulkanFrameData _FrameData;

	//The destruction queue.
	DynamicArray<VulkanDestructionData> _DestructionQueue;
}

//Vulkan rendering system logic.
namespace VulkanRenderingSystemLogic
{

	/*
	*	Concatenates all secondary command buffers into the previous one.
	*/
	void ConcatenateCommandBuffers() NOEXCEPT
	{
		//Begin the current primary command buffer.
		VulkanCommandBuffer *const RESTRICT currentPrimaryCommandBuffer{ VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer() };
		currentPrimaryCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Iterate over all render passes and concatenate their command buffers into the primary command buffer.
		RenderPassMainStage currentStage{ RenderPassMainStage::None };

		for (const Pipeline *const RESTRICT pipeline : RenderingSystem::Instance->GetPipelines())
		{
			//Wait for the render pass to finish it's render.
			pipeline->WaitForRender();

			//Begin a new render pass, if necessary.
			if (currentStage != pipeline->GetMainStage())
			{
				if (currentStage != RenderPassMainStage::None)
				{
					currentPrimaryCommandBuffer->CommandEndRenderPass();
				}

				currentStage = pipeline->GetMainStage();

				vkCmdPipelineBarrier(currentPrimaryCommandBuffer->Get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 0, nullptr);

				if (VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._ShouldClear)
				{
					currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f),
						0.0f,
						VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
						VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? RenderingSystem::Instance->GetCurrentFramebufferIndex() : 0]->Get(),
						pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height },
						VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS,
						VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._NumberOfAttachments);
				}

				else
				{
					currentPrimaryCommandBuffer->CommandBeginRenderPass(VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
						VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? RenderingSystem::Instance->GetCurrentFramebufferIndex() : 0]->Get(),
						pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height },
						VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
				}
			}

			else
			{
				currentPrimaryCommandBuffer->CommandNextSubpass();
			}

			//Record the execute commands.
			if (pipeline->IncludeInRender())
			{
				currentPrimaryCommandBuffer->CommandExecuteCommands(reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(pipeline->GetCurrentCommandBuffer()->GetCommandBufferData())->Get());
			}
		}

		//End the last remaining render pass.
		currentPrimaryCommandBuffer->CommandEndRenderPass();
	}

	/*
	*	Initializes all shader modules.
	*/
	void InitializeShaderModules() NOEXCEPT
	{
		//Open the shader collection file.
		BinaryFile<IOMode::In> shaderCollection{ "..\\..\\..\\Resources\\Final\\CatalystShaderCollection.csc" };

		{
			//Initialize the anti-aliasing fragment shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::AntiAliasingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		{
			//Initialize the debug axis-aligned bounding box fragment shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::DebugAxisAlignedBoundingBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		{
			//Initialize the debug axis-aligned bounding box vertex shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::DebugAxisAlignedBoundingBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
		}

		{
			//Initialize the debug screen box fragment shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::DebugScreenBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		{
			//Initialize the debug screen box vertex shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::DebugScreenBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
		}

		{
			//Initialize the passthrough fragment shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::PassthroughFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		{
			//Initialize the tone mapping fragment shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::ToneMappingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		{
			//Initialize the viewport vertex shader module.
			uint64 size{ 0 };
			shaderCollection.Read(&size, sizeof(uint64));
			DynamicArray<byte> data;
			data.UpsizeFast(size);
			shaderCollection.Read(data.Data(), size);
			VulkanRenderingSystemData::_ShaderModules[UNDERLYING(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
		}

		shaderCollection.Close();
	}

	/*
	*	Initializes all Vulkan render passes.
	*/
	void InitializeVulkanRenderPasses() NOEXCEPT
	{
#if defined(CATALYST_CONFIGURATION_DEBUG)
		//Initialize the debug render pass.
		{
			constexpr uint64 NUMBER_OF_DEBUG_SUBPASSES{ 2 };

			constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
			constexpr uint32 SCENE_INDEX{ 1 };

			VulkanRenderPassCreationParameters renderPassParameters;

			StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
			{
				//Depth buffer.
				VulkanUtilities::CreateAttachmentDescription(static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
																VK_ATTACHMENT_LOAD_OP_LOAD,
																VK_ATTACHMENT_STORE_OP_STORE,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_DONT_CARE,
																VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

																//Scene.
																VulkanUtilities::CreateAttachmentDescription(static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
																												VK_ATTACHMENT_LOAD_OP_LOAD,
																												VK_ATTACHMENT_STORE_OP_STORE,
																												VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																												VK_ATTACHMENT_STORE_OP_DONT_CARE,
																												VK_IMAGE_LAYOUT_GENERAL,
																												VK_IMAGE_LAYOUT_GENERAL)
			};

			renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
			renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

			constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

			constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
			{
				VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
			};

			StaticArray<VkSubpassDescription, NUMBER_OF_DEBUG_SUBPASSES> subpassDescriptions;

			subpassDescriptions[0] = VulkanUtilities::CreateSubpassDescription(0,
				nullptr,
				1,
				colorAttachmentReferences.Data(),
				&depthAttachmentReference,
				0,
				nullptr);

			subpassDescriptions[1] = VulkanUtilities::CreateSubpassDescription(0,
				nullptr,
				1,
				colorAttachmentReferences.Data(),
				nullptr,
				0,
				nullptr);

			renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
			renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

			StaticArray<VkSubpassDependency, NUMBER_OF_DEBUG_SUBPASSES - 1> subpassDependencies
			{
				VulkanUtilities::CreateSubpassDependency(0,
															1,
															VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
															VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
															VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
															VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
															VK_DEPENDENCY_BY_REGION_BIT)
			};

			renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
			renderPassParameters._SubpassDependencies = subpassDependencies.Data();;

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

			//Create the framebuffer.
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._RenderPass->Get();

			StaticArray<VkImageView, 2> attachments
			{
				static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
				static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
			};

			framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
			framebufferParameters._Attachments = attachments.Data();
			framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._FrameBuffers.Reserve(1);
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._NumberOfAttachments = 1;
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._ShouldClear = false;
		}
#endif

		//Initialize the tone mapping render pass.
		{
			constexpr uint64 NUMBER_OF_TONE_MAPPING_SUBPASSES{ 1 };

			constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 0 };

			VulkanRenderPassCreationParameters renderPassParameters;

			StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
			{
				//Screen.
				VulkanUtilities::CreateAttachmentDescription(static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_STORE,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_DONT_CARE,
																VK_IMAGE_LAYOUT_GENERAL,
																VK_IMAGE_LAYOUT_GENERAL)
			};

			renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
			renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

			constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
			{
				VkAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
			};

			StaticArray<VkSubpassDescription, NUMBER_OF_TONE_MAPPING_SUBPASSES> subpassDescriptions;

			for (VkSubpassDescription &subpassDescription : subpassDescriptions)
			{
				subpassDescription = VulkanUtilities::CreateSubpassDescription(0,
					nullptr,
					1,
					colorAttachmentReferences.Data(),
					nullptr,
					0,
					nullptr);
			}

			renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
			renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

			renderPassParameters._SubpassDependencyCount = 0;
			renderPassParameters._SubpassDependencies = nullptr;

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

			//Create the framebuffer.
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._RenderPass->Get();

			StaticArray<VkImageView, 1> attachments
			{
				static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
			};

			framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
			framebufferParameters._Attachments = attachments.Data();
			framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._FrameBuffers.Reserve(1);
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._NumberOfAttachments = 1;
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._ShouldClear = false;
		}

		//Initialize the anti-aliasing final render pass.
		{
			constexpr uint64 NUMBER_OF_ANTI_ALIASING_SUBPASSES{ 1 };

			constexpr uint32 SCREEN_INDEX{ 0 };

			VulkanRenderPassCreationParameters renderPassParameters;

			StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
			{
				//Screen.
				VulkanUtilities::CreateAttachmentDescription(VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_STORE,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_DONT_CARE,
																VK_IMAGE_LAYOUT_UNDEFINED,
																VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			};

			renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
			renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

			constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
			{
				VkAttachmentReference{ SCREEN_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			};

			StaticArray<VkSubpassDescription, NUMBER_OF_ANTI_ALIASING_SUBPASSES> subpassDescriptions;

			for (VkSubpassDescription &subpassDescription : subpassDescriptions)
			{
				subpassDescription = VulkanUtilities::CreateSubpassDescription(0,
					nullptr,
					1,
					colorAttachmentReferences.Data(),
					nullptr,
					0,
					nullptr);
			}

			renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
			renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

			renderPassParameters._SubpassDependencyCount = 0;
			renderPassParameters._SubpassDependencies = nullptr;

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

			//Create the framebuffers.
			const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._FrameBuffers.Reserve(swapchainImages.Size());

			for (VkImageView swapchainImage : swapchainImages)
			{
				VulkanFramebufferCreationParameters framebufferParameters;

				framebufferParameters._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._RenderPass->Get();

				framebufferParameters._AttachmentCount = 1;
				framebufferParameters._Attachments = &swapchainImage;
				framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

				VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
			}

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._NumberOfAttachments = 1;
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._ShouldClear = false;
		}

#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
		//Initialize the render override render pass.
		{
			constexpr uint64 NUMBER_OF_SUBPASSES{ 1 };

			constexpr uint32 SCREEN_INDEX{ 0 };

			VulkanRenderPassCreationParameters renderPassParameters;

			StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
			{
				//Screen.
				VulkanUtilities::CreateAttachmentDescription(VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_STORE,
																VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																VK_ATTACHMENT_STORE_OP_DONT_CARE,
																VK_IMAGE_LAYOUT_UNDEFINED,
																VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			};

			renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
			renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

			constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
			{
				VkAttachmentReference{ SCREEN_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			};

			StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

			for (VkSubpassDescription &subpassDescription : subpassDescriptions)
			{
				subpassDescription = VulkanUtilities::CreateSubpassDescription(0,
					nullptr,
					1,
					colorAttachmentReferences.Data(),
					nullptr,
					0,
					nullptr);
			}

			renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
			renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

			renderPassParameters._SubpassDependencyCount = 0;
			renderPassParameters._SubpassDependencies = nullptr;

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

			//Create the framebuffers.
			const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._FrameBuffers.Reserve(swapchainImages.Size());

			for (VkImageView swapchainImage : swapchainImages)
			{
				VulkanFramebufferCreationParameters framebufferParameters;

				framebufferParameters._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._RenderPass->Get();

				framebufferParameters._AttachmentCount = 1;
				framebufferParameters._Attachments = &swapchainImage;
				framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

				VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
			}

			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._NumberOfAttachments = 1;
			VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::RenderOverride)]._ShouldClear = false;
		}
#endif
	}

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT
	{
		for (uint64 i = 0; i < VulkanRenderingSystemData::_DestructionQueue.Size();)
		{
			++VulkanRenderingSystemData::_DestructionQueue[i]._Frames;

			if (VulkanRenderingSystemData::_DestructionQueue[i]._Frames > VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() + 1)
			{
				switch (VulkanRenderingSystemData::_DestructionQueue[i]._Type)
				{
					case VulkanRenderingSystemData::VulkanDestructionData::Type::ConstantBuffer:
					{
						VulkanInterface::Instance->DestroyConstantBuffer(static_cast<VulkanConstantBuffer *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::RenderDataTable:
					{
						VulkanInterface::Instance->DestroyDescriptorSet(static_cast<VulkanDescriptorSet *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::Texture2D:
					{
						VulkanInterface::Instance->Destroy2DTexture(static_cast<Vulkan2DTexture *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::UniformBuffer:
					{
						VulkanInterface::Instance->DestroyUniformBuffer(static_cast<VulkanUniformBuffer *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}
#if defined(CATALYST_CONFIGURATION_DEBUG)
					default:
					{
						ASSERT(false, "Unhandled case!");

						break;
					}
#endif
				}

				VulkanRenderingSystemData::_DestructionQueue.EraseAt(i);
			}

			else
			{
				++i;
			}
		}
	}

}

/*
*	Creates a buffer.
*/
void RenderingSystem::CreateBuffer(const uint64 size, ConstantBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the buffer.
	*handle = static_cast<ConstantBufferHandle>(VulkanInterface::Instance->CreateBuffer(size));
}

/*
*	Uploads data to a buffer.
*/
void RenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, ConstantBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Upload data to the buffer.
	static_cast<VulkanConstantBuffer *const RESTRICT>(*handle)->UploadData(data, static_cast<const VkDeviceSize *const RESTRICT>(dataSizes), dataChunks);
}

/*
*	Destroys a buffer.
*/
void RenderingSystem::DestroyBuffer(ConstantBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.EmplaceSlow(VulkanRenderingSystemData::VulkanDestructionData::Type::ConstantBuffer, *handle);
}

/*
*	Creates a depth buffer.
*/
void RenderingSystem::CreateDepthBuffer(const Resolution resolution, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the depth buffer.
	*handle = static_cast<DepthBufferHandle>(VulkanInterface::Instance->CreateDepthBuffer(VkExtent2D{ resolution._Width, resolution._Height }));
}

/*
*	Creates a render data table layout.
*/
void RenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Translate from the API-agnostic structure to the Vulkan-specific.
	DynamicArray<VkDescriptorSetLayoutBinding> vulkanBindings;
	vulkanBindings.Reserve(numberOfBindings);

	for (uint32 i = 0; i < numberOfBindings; ++i)
	{
		const RenderDataTableLayoutBinding &binding{ bindings[i] };

		vulkanBindings.EmplaceFast(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), binding._NumberOfArrayElements, VulkanTranslationUtilities::GetVulkanShaderStages(binding._ShaderStage)));
	}

	*handle = VulkanInterface::Instance->CreateDescriptorSetLayout(vulkanBindings.Data(), numberOfBindings);
}

/*
*	Creates a render data table.
*/
void RenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateDescriptorSet(*static_cast<const VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanImage *const RESTRICT vulkanImage{ static_cast<VulkanImage *const RESTRICT>(image) };
	VulkanSampler *const RESTRICT vulkanSampler{ static_cast<VulkanSampler *const RESTRICT>(sampler) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = vulkanSampler->Get();
	descriptorImageInfo.imageView = vulkanImage->GetImageView();
	descriptorImageInfo.imageLayout = vulkanImage->GetImageLayout();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanImage *const RESTRICT vulkanImage{ static_cast<VulkanImage *const RESTRICT>(image) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = VK_NULL_HANDLE;
	descriptorImageInfo.imageView = vulkanImage->GetImageView();
	descriptorImageInfo.imageLayout = vulkanImage->GetImageLayout();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a sampler to a render data table.
*/
void RenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanSampler *const RESTRICT vulkanSampler{ static_cast<VulkanSampler *const RESTRICT>(sampler) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = vulkanSampler->Get();
	descriptorImageInfo.imageView = VK_NULL_HANDLE;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void RenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, UniformBufferHandle uniformBuffer) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanUniformBuffer *const RESTRICT vulkanUniformBuffer{ static_cast<VulkanUniformBuffer *const RESTRICT>(uniformBuffer) };

	//Create the destriptor buffer info.
	VkDescriptorBufferInfo descriptorBufferInfo;

	descriptorBufferInfo.buffer = vulkanUniformBuffer->Get();
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = vulkanUniformBuffer->GetSize();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pImageInfo = nullptr;
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Destroys a render data table.
*/
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.EmplaceSlow(VulkanRenderingSystemData::VulkanDestructionData::Type::RenderDataTable, *handle);
}

/*
*	Creates a render target.
*/
void RenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target.
	*handle = static_cast<RenderTargetHandle>(VulkanInterface::Instance->CreateRenderTarget(VulkanTranslationUtilities::GetVulkanExtent(resolution),
																							VulkanTranslationUtilities::GetVulkanFormat(format)));
}

/*
*	Creates a sampler.
*/
void RenderingSystem::CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateSampler(	VulkanTranslationUtilities::GetVulkanTextureFilter(properties._MagnificationFilter),
														VulkanTranslationUtilities::GetVulkanMipmapMode(properties._MipmapMode),
														VulkanTranslationUtilities::GetVulkanAddressMode(properties._AddressMode));
}

/*
*	Creates a texture 2D.
*/
void RenderingSystem::CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture 2D.
	*handle = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(	static_cast<uint32>(data._TextureDataContainer._TextureData.Size()),
																						data._TextureDataContainer._TextureWidth,
																						data._TextureDataContainer._TextureHeight,
																						data._TextureDataContainer._TextureChannels,
																						data._TextureDataContainer._TextureTexelSize,
																						data._TextureDataContainer._TextureData.Data(),
																						VulkanTranslationUtilities::GetVulkanFormat(data._TextureFormat)));
}

/*
*	Destroys a texture 2D.
*/
void RenderingSystem::DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.EmplaceSlow(VulkanRenderingSystemData::VulkanDestructionData::Type::Texture2D, *handle);
}

/*
*	Creates a texture cube.
*/
void RenderingSystem::CreateTextureCube(const float *const RESTRICT data, const Resolution resolution, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture cube.
	*handle = static_cast<TextureCubeHandle>(VulkanInterface::Instance->CreateCubeMapTexture(data, resolution._Width, resolution._Height));
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
void RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize, const BufferUsage usage, UniformBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	VkBufferUsageFlags vulkanUsage{ 0 };

	if (TEST_BIT(usage, BufferUsage::UniformBuffer))
	{
		vulkanUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	if (TEST_BIT(usage, BufferUsage::VertexBuffer))
	{
		vulkanUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}

	*handle = static_cast<UniformBufferHandle>(VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize, vulkanUsage));
}

/*
*	Uploads data to a uniform buffer.
*/
void RenderingSystem::UploadDataToUniformBuffer(const void *const RESTRICT data, UniformBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	static_cast<VulkanUniformBuffer *const RESTRICT>(*handle)->UploadData(data);
}

/*
*	Destroys a uniform buffer.
*/
void RenderingSystem::DestroyUniformBuffer(UniformBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.EmplaceSlow(VulkanRenderingSystemData::VulkanDestructionData::Type::UniformBuffer, *handle);
}

/*
*	Pre-initializes the rendering system.
*/
void RenderingSystem::PreInitialize() NOEXCEPT
{
	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize();

	//Initialize all shader modules.
	VulkanRenderingSystemLogic::InitializeShaderModules();

	//Initialize the Vulkan frame data.
	VulkanRenderingSystemData::_FrameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages());
}

/*
*	Post-initializes the rendering system.
*/
void RenderingSystem::PostInitialize() NOEXCEPT
{
	//Initialize all Vulkan render passes.
	VulkanRenderingSystemLogic::InitializeVulkanRenderPasses();
}

/*
*	Begins a frame.
*/
void RenderingSystem::BeginFrame() NOEXCEPT
{
	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(VulkanRenderingSystemData::_FrameData.GetImageAvailableSemaphore());

	//Process the destruction queue.
	VulkanRenderingSystemLogic::ProcessDestructionQueue();

	//Set the current frame.
	VulkanRenderingSystemData::_FrameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Wait for the current fence to finish.
	VulkanRenderingSystemData::_FrameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	VulkanRenderingSystemData::_FrameData.GetCurrentFence()->Reset();
}

/*
*	Ends a frame.
*/
void RenderingSystem::EndFrame() NOEXCEPT
{
	//Concatenate all secondary command buffers into the previous one.
	VulkanRenderingSystemLogic::ConcatenateCommandBuffers();

	//End the current command buffer.
	VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(	*VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer(),
															1,
															VulkanRenderingSystemData::_FrameData.GetImageAvailableSemaphore(),
															VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
															1,
															VulkanRenderingSystemData::_FrameData.GetRenderFinishedSemaphore(),
															VulkanRenderingSystemData::_FrameData.GetCurrentFence()->Get());

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(VulkanRenderingSystemData::_FrameData.GetRenderFinishedSemaphore());
}

/*
*	Pre-releases the rendering system.
*/
void RenderingSystem::PreRelease() NOEXCEPT
{
	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Post-releases the rendering system.
*/
void RenderingSystem::PostRelease() NOEXCEPT
{

}

/*
*	Returns the number of framebuffers.
*/
uint8 RenderingSystem::GetNumberOfFramebuffers() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages();
}

/*
*	Returns the current framebuffer index.
*/
uint8 RenderingSystem::GetCurrentFramebufferIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Finalizes the initialization of a pipeline.
*/
void RenderingSystem::FinalizePipelineInitialization(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	//Create the pipeline.
	VulkanPipelineCreationParameters parameters;

	parameters._BlendEnable = pipeline->IsBlendEnabled();
	parameters._BlendFactorSourceColor = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorSourceColor());
	parameters._BlendFactorDestinationColor = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorDestinationColor());
	parameters._BlendFactorSourceAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorSourceAlpha());
	parameters._BlendFactorDestinationAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorDestinationAlpha());
	parameters._ColorAttachmentCount = static_cast<uint32>(pipeline->GetRenderTargets().Size());
	parameters._CullMode = VulkanTranslationUtilities::GetVulkanCullMode(pipeline->GetCullMode());
	parameters._DepthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(pipeline->GetDepthCompareOperator());
	parameters._DepthTestEnable = pipeline->IsDepthTestEnabled();
	parameters._DepthWriteEnable = pipeline->IsDepthWriteEnabled();

	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(pipeline->GetRenderDataTableLayouts().Size());

	for (RenderDataTableLayoutHandle renderDataTableLayout : pipeline->GetRenderDataTableLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
	}

	parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
	parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

	DynamicArray<VkPushConstantRange> pushConstantRanges;

	if (pipeline->GetPushConstantRanges().Empty())
	{
		parameters._PushConstantRangeCount = 0;
		parameters._PushConstantRanges = nullptr;
	}

	else
	{
		pushConstantRanges.Reserve(pipeline->GetPushConstantRanges().Size());

		for (const PushConstantRange &pushConstantRange : pipeline->GetPushConstantRanges())
		{
			pushConstantRanges.EmplaceFast(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
		}

		parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
		parameters._PushConstantRanges = pushConstantRanges.Data();
	}
	
	parameters._ShaderModules.Reserve(5);
	if (pipeline->GetVertexShader() != Shader::None) parameters._ShaderModules.EmplaceFast(VulkanRenderingSystemData::_ShaderModules[UNDERLYING(pipeline->GetVertexShader())]);
	if (pipeline->GetTessellationControlShader() != Shader::None) parameters._ShaderModules.EmplaceFast(VulkanRenderingSystemData::_ShaderModules[UNDERLYING(pipeline->GetTessellationControlShader())]);
	if (pipeline->GetTessellationEvaluationShader() != Shader::None) parameters._ShaderModules.EmplaceFast(VulkanRenderingSystemData::_ShaderModules[UNDERLYING(pipeline->GetTessellationEvaluationShader())]);
	if (pipeline->GetGeometryShader() != Shader::None) parameters._ShaderModules.EmplaceFast(VulkanRenderingSystemData::_ShaderModules[UNDERLYING(pipeline->GetGeometryShader())]);
	if (pipeline->GetFragmentShader() != Shader::None) parameters._ShaderModules.EmplaceFast(VulkanRenderingSystemData::_ShaderModules[UNDERLYING(pipeline->GetFragmentShader())]);
	parameters._StencilTestEnable = pipeline->IsStencilTestEnabled();
	parameters._StencilFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilFailOperator());
	parameters._StencilPassOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilPassOperator());
	parameters._StencilDepthFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilDepthFailOperator());
	parameters._StencilCompareOperator = VulkanTranslationUtilities::GetVulkanCompareOperator(pipeline->GetStencilCompareOperator());
	parameters._StencilCompareMask = pipeline->GetStencilCompareMask();
	parameters._StencilWriteMask = pipeline->GetStencilWriteMask();
	parameters._StencilReferenceMask = pipeline->GetStencilReferenceMask();
	parameters._Subpass = VulkanTranslationUtilities::GetSubStageIndex(pipeline->GetMainStage(), pipeline->GetSubStage());

	parameters._Topology = VulkanTranslationUtilities::GetVulkanTopology(pipeline->GetTopology());

	DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	vertexInputAttributeDescriptions.Reserve(pipeline->GetVertexInputAttributeDescriptions().Size());

	for (const VertexInputAttributeDescription &vertexInputAttributeDescription : pipeline->GetVertexInputAttributeDescriptions())
	{
		vertexInputAttributeDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
	}

	parameters._VertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	parameters._VertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

	DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	vertexInputBindingDescriptions.Reserve(pipeline->GetVertexInputBindingDescriptions().Size());

	for (const VertexInputBindingDescription &vertexInputBindingDescription : pipeline->GetVertexInputBindingDescriptions())
	{
		vertexInputBindingDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
	}

	parameters._VertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
	parameters._VertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
	parameters._ViewportExtent = pipeline->GetRenderTargets().Empty() ? VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height } : pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };

	parameters._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(pipeline->GetMainStage())]._RenderPass;

	//Create the pipeline!
	VulkanRenderingSystemData::_VulkanRenderPassSubStageData[UNDERLYING(pipeline->GetSubStage())]._Pipeline = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	VulkanRenderingSystemData::_VulkanRenderPassSubStageData[UNDERLYING(pipeline->GetSubStage())]._Framebuffers.Reserve(VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(pipeline->GetMainStage())]._FrameBuffers.Size());

	for (VulkanFramebuffer *RESTRICT vulkanFrameBuffer : VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(pipeline->GetMainStage())]._FrameBuffers)
	{
		VulkanRenderingSystemData::_VulkanRenderPassSubStageData[UNDERLYING(pipeline->GetSubStage())]._Framebuffers.EmplaceFast(vulkanFrameBuffer->Get());
	}

	VulkanRenderingSystemData::_VulkanRenderPassSubStageData[UNDERLYING(pipeline->GetSubStage())]._RenderPass = VulkanRenderingSystemData::_VulkanRenderPassMainStageData[UNDERLYING(pipeline->GetMainStage())]._RenderPass->Get();

	pipeline->SetData(&VulkanRenderingSystemData::_VulkanRenderPassSubStageData[UNDERLYING(pipeline->GetSubStage())]);

	//Add the command buffers.
	const uint64 numberOfCommandBuffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	pipeline->SetNumberOfCommandBuffers(numberOfCommandBuffers);

	//Create the directional shadow command pool.
	VulkanCommandPool *const RESTRICT pipelineCommandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) };

	for (uint64 i = 0; i < numberOfCommandBuffers; ++i)
	{
		VulkanCommandBuffer *const RESTRICT pipelineCommandBuffer{ new (MemoryUtilities::GlobalPoolAllocate<sizeof(VulkanCommandBuffer)>()) VulkanCommandBuffer };
		pipelineCommandPool->AllocateSecondaryCommandBuffer(*pipelineCommandBuffer);
		pipeline->AddCommandBuffer(new (MemoryUtilities::GlobalPoolAllocate<sizeof(CommandBuffer)>()) CommandBuffer(pipelineCommandBuffer));
	}
}
#endif