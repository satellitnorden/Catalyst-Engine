//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/OceanGraphicsPipeline.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Noise/SimplexNoise.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Ocean graphics pipeline constants
namespace OceanGraphicsPipelineConstants
{
	constexpr uint8 OCEAN_TEXTURE_RESOLUTION{ 32 };
	constexpr Vector2<float> OCEAN_TEXTURE_OFFSET{ 1.0f / static_cast<float>(OCEAN_TEXTURE_RESOLUTION), 1.0f / static_cast<float>(OCEAN_TEXTURE_RESOLUTION) };
}

/*
*	Initializes this graphics pipeline.
*/
void OceanGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the ocean texture.
	CreateOceanTexture();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::OceanFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::One);
	SetBlendFactorDestinationColor(BlendFactor::One);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void OceanGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Returns the ocean height at the given coordinate.
*/
float OceanGraphicsPipeline::OceanHeight(const Vector2<float> coordinate) const NOEXCEPT
{
	return SimplexNoise::GenerateNormalized(coordinate, 0.0f);
}

/*
*	Returns the ocean normal at the given coordinate.
*/
void OceanGraphicsPipeline::OceanNormal(const Vector2<float> coordinate, Vector3<float> *const RESTRICT normal, float *const RESTRICT height) const NOEXCEPT
{
	//Retrieve the positions in a cross.
	const Vector3<float> center{ 0.0f, OceanHeight(coordinate), 0.0f };
	const Vector3<float> up{ 0.0f, OceanHeight(coordinate + OceanGraphicsPipelineConstants::OCEAN_TEXTURE_OFFSET * Vector2<float>(0.0f, 1.0f)), 1.0f };
	const Vector3<float> right{ 1.0f, OceanHeight(coordinate + OceanGraphicsPipelineConstants::OCEAN_TEXTURE_OFFSET * Vector2<float>(1.0f, 0.0f)), 0.0f };
	const Vector3<float> down{ 0.0f, OceanHeight(coordinate + OceanGraphicsPipelineConstants::OCEAN_TEXTURE_OFFSET * Vector2<float>(0.0f, -1.0f)), -1.0f };
	const Vector3<float> left{ -1.0f, OceanHeight(coordinate + OceanGraphicsPipelineConstants::OCEAN_TEXTURE_OFFSET * Vector2<float>(-1.0f, 0.0f)), 0.0f };

	*normal = Vector3<float>::Normalize(Vector3<float>::CrossProduct(up - center, right - center) + Vector3<float>::CrossProduct(right - center, down - center) + Vector3<float>::CrossProduct(down - center, left - center) + Vector3<float>::CrossProduct(left - center, up - center));
	*height = center._Y;
}

/*
*	Creates the ocean texture.
*/
void OceanGraphicsPipeline::CreateOceanTexture() NOEXCEPT
{
	//Create the data for the ocean texture.
	Texture2D<Vector4<byte>> ocean_texture{ OceanGraphicsPipelineConstants::OCEAN_TEXTURE_RESOLUTION };

	for (uint8 x{ 0 }; x < OceanGraphicsPipelineConstants::OCEAN_TEXTURE_RESOLUTION; ++x)
	{
		for (uint8 y{ 0 }; y < OceanGraphicsPipelineConstants::OCEAN_TEXTURE_RESOLUTION; ++y)
		{
			//Calculate the coordinate.
			const Vector2<float> coordinate{ static_cast<float>(x) / static_cast<float>(OceanGraphicsPipelineConstants::OCEAN_TEXTURE_RESOLUTION), static_cast<float>(y) / static_cast<float>(OceanGraphicsPipelineConstants::OCEAN_TEXTURE_RESOLUTION) };

			//Retrieve the ocean normal and height.
			Vector3<float> ocean_normal;
			float ocean_height;
			
			OceanNormal(coordinate, &ocean_normal, &ocean_height);

			const Vector3<byte> ocean_normal_byte{ static_cast<byte>((ocean_normal._X * 0.5f + 0.5f) * 255.0f) , static_cast<byte>((ocean_normal._Y * 0.5f + 0.5f) * 255.0f) , static_cast<byte>((ocean_normal._Z * 0.5f + 0.5f) * 255.0f) };
			const byte ocean_height_byte = static_cast<byte>(ocean_height * 255.0f);

			ocean_texture.At(x, y) = Vector4<byte>(ocean_normal_byte._X, ocean_normal_byte._Y, ocean_normal_byte._Z, ocean_height_byte);
		}
	}

	//Create the texture!
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(ocean_texture), TextureFormat::R8G8B8A8_Byte), &_OceanTexture);
}

/*
*	Creates the render data table layout.
*/
void OceanGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 3> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void OceanGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);


	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, _OceanTexture, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
}