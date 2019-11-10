//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/CloudsGraphicsPipeline.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Texture3D.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The sky light view direction.
	Vector3<float> _SkyLightViewDirection;

	//Padding.
	Padding<4> _Padding1;

	//The sky light luminance.
	Vector3<float> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding2;

	//The cloud density.
	float _CloudDensity;

};

/*
*	Initializes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the cloud texture.
	CreateCloudTexture();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::CloudsFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution() / 2);

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
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
	SetStencilCompareOperator(CompareOperator::NotEqual);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr float MINIMUM_CLOUD_DENSITY{ 0.3f };
	constexpr float MAXIMUM_CLOUD_DENSITY{ 0.7f };

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	PushConstantData data;

	//Assume that the first light is the sky light, for now. (:
	if (ComponentManager::GetNumberOfLightComponents() > 0)
	{
		data._SkyLightViewDirection = Vector3<float>::Normalize(Perceiver::Instance->GetPosition() - ComponentManager::GetLightLightComponents()[0]._Position);
		data._SkyLightLuminance = ComponentManager::GetLightLightComponents()[0]._Color * ComponentManager::GetLightLightComponents()[0]._Strength * 0.0000000025f;
	}

	else
	{
		data._SkyLightViewDirection = VectorConstants::UP;
		data._SkyLightLuminance = VectorConstants::ONE;
	}

	data._CloudDensity = CatalystBaseMath::Scale(EnvironmentManager::GetCloudDensity(), 0.0f, 1.0f, MINIMUM_CLOUD_DENSITY, MAXIMUM_CLOUD_DENSITY);
	//data._CloudDensity = 0.5f;

	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the cloud texture.
*/
void CloudsGraphicsPipeline::CreateCloudTexture() NOEXCEPT
{
	//Defone constants.
	constexpr uint32 CLOUD_TEXTURE_RESOLUTION{ 64 };
	constexpr uint32 CLOUD_TEXTURE_LAYER_0_POINTS{ 64 };

	CATALYST_BENCHMARK_SECTION_START();

	//Generate the points for the layer.
	DynamicArray<Vector3<float>> points;
	points.Reserve(CLOUD_TEXTURE_LAYER_0_POINTS * 27);

	for (uint32 i{ 0 }; i < CLOUD_TEXTURE_LAYER_0_POINTS; ++i)
	{
		points.EmplaceSlow(	CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
							CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
							CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f));
	}

	//Copy the first N points to the sides of the cube.
	for (int8 X{ -1 }; X <= 1; ++X)
	{
		for (int8 Y{ -1 }; Y <= 1; ++Y)
		{
			for (int8 Z{ -1 }; Z <= 1; ++Z)
			{
				if (X == 0 && Y == 0 && Z == 0)
				{
					continue;
				}

				for (uint8 i{ 0 }; i < CLOUD_TEXTURE_LAYER_0_POINTS; ++i)
				{
					const Vector3<float> offset{ static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z) };
					points.EmplaceFast(points[i] + offset);
				}
			}
		}
	}

	//Create the temporary texture.
	Texture3D<float> temporary_texture{ CLOUD_TEXTURE_RESOLUTION };

	//Keep track of the longest distance.
	float longest_distance{ -FLOAT_MAXIMUM };

	for (uint32 X{ 0 }; X < CLOUD_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < CLOUD_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 Z{ 0 }; Z < CLOUD_TEXTURE_RESOLUTION; ++Z)
			{
				//Calcualte the position in the texture.
				const Vector3<float> position{	static_cast<float>(X) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION),
												static_cast<float>(Y) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION),
												static_cast<float>(Z) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION) };

				//Find the closest distance.
				float closest_distance{ FLOAT_MAXIMUM };

				for (const Vector3<float>& point : points)
				{
					const float distance{ Vector3<float>::Length(position - point) };
					closest_distance = CatalystBaseMath::Minimum<float>(closest_distance, distance);
				}

				//Write to the texture.
				temporary_texture.At(X, Y, Z) = closest_distance;

				//Update the longest distance.
				longest_distance = CatalystBaseMath::Maximum<float>(longest_distance, closest_distance);
			}
		}
	}

	//Create the final texture.
	Texture3D<byte> final_texture{ CLOUD_TEXTURE_RESOLUTION };

	for (uint32 X{ 0 }; X < CLOUD_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < CLOUD_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 Z{ 0 }; Z < CLOUD_TEXTURE_RESOLUTION; ++Z)
			{
				//Get the distance at the current position.
				float distance{ temporary_texture.At(X, Y, Z) };

				//Normalize the distance.
				distance /= longest_distance;

				//Invert the distance.
				distance = 1.0f - distance;

				//Convert it into byte.
				final_texture.At(X, Y, Z) = static_cast<byte>(distance * UINT8_MAXIMUM);
			}
		}
	}

	CATALYST_BENCHMARK_SECTION_END("CreateCloudTexture");

	//Create the texture 3D.
	RenderingSystem::Instance->CreateTexture3D(	TextureData(TextureDataContainer(final_texture),
															TextureFormat::R8_Byte), &_CloudTexture);
}

/*
*	Creates the render data table layout.
*/
void CloudsGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void CloudsGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, _CloudTexture, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
}