//Header file.
#include <Rendering/Native/VirtualRealitySystem.h>

//Systems.
#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
#include <Systems/CatalystEngineSystem.h>
#endif
#include <Systems/RenderingSystem.h>

#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Third party.
#include <ThirdParty/VrApi/VrApi.h>
#endif

/*
*	Virtual reality uniform data definition.
*/
class VirtualRealityUniformData final
{

public:

	//The eye world to clip matrices.
	StaticArray<Matrix4x4, 2> _EyeWorldToClipMatrices;

};

/*
*	Post-initializes the virtual reality system.
*/
void VirtualRealitySystem::PostInitialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Create the virtual reality uniform data buffers.
	CreateVirtualRealityUniformDataBuffers();
}

/*
*	Updates the virtual reality system during the render update phase.
*/
void VirtualRealitySystem::RenderUpdate() NOEXCEPT
{
	//Update the current render data table.
	RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Update the current virtual reality uniform data buffer.
	{
		BufferHandle &current_virtual_reality_uniform_data_buffer{ _VirtualRealityUniformDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		VirtualRealityUniformData virtual_reality_uniform_data;

#if defined(CATALYST_PLATFORM_ANDROID)
		virtual_reality_uniform_data._EyeWorldToClipMatrices[0] = virtual_reality_uniform_data._EyeWorldToClipMatrices[1] = *RenderingSystem::Instance->GetCurrentCamera()->GetViewMatrix();
#elif defined(CATALYST_PLATFORM_OCULUS_QUEST)
	//Retrieve the predicted display time.
    const float64 predicted_display_time{ vrapi_GetPredictedDisplayTime(CatalystPlatform::_ovrMobile, static_cast<uint32>(CatalystEngineSystem::Instance->GetTotalFrames())) };

    //Retrieve the predicted tracking.
    const ovrTracking2 predicted_tracking{ vrapi_GetPredictedTracking2(CatalystPlatform::_ovrMobile, predicted_display_time) };

    //Set the eye world to clip matrices.
    for (uint8 i{ 0 }; i < 2; ++i)
    {
    	Matrix4x4 projection_matrix;
    	Matrix4x4 view_matrix;

    	Memory::Copy(&projection_matrix, &predicted_tracking.Eye[i].ProjectionMatrix, sizeof(Matrix4x4));
    	Memory::Copy(&view_matrix, &predicted_tracking.Eye[i].ViewMatrix, sizeof(Matrix4x4));

    	projection_matrix.Transpose();
    	view_matrix.Transpose();

    	virtual_reality_uniform_data._EyeWorldToClipMatrices[i] = projection_matrix * view_matrix;
    }
#elif defined(CATALYST_PLATFORM_WINDOWS)
		virtual_reality_uniform_data._EyeWorldToClipMatrices[0] = virtual_reality_uniform_data._EyeWorldToClipMatrices[1] = *RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix();
#else
	#error "Unknown platform!"
#endif

		const void *const RESTRICT data_chunks[]{ &virtual_reality_uniform_data };
		const uint64 data_sizes[]{ sizeof(VirtualRealityUniformData) };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_virtual_reality_uniform_data_buffer);

		//Bind the virtual reality uniform data buffer.
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &current_render_data_table, current_virtual_reality_uniform_data_buffer);
	}
}

/*
*	Returns the current virtual reality data render data table.
*/
RenderDataTableHandle VirtualRealitySystem::GetCurrentVirtualRealityDataRenderDataTable() const NOEXCEPT
{
	//Return the current virtual reality data render data table.
	return _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Creates the render data table layout.
*/
void VirtualRealitySystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Create the render data table layout.
	{
		StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX)
		};

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
	}
}

/*
*	Creates the render data tables.
*/
void VirtualRealitySystem::CreateRenderDataTables() NOEXCEPT
{
	//Create the render data tables.
	_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &render_data_table : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);
	}
}

/*
*	Creates the virtual reality uniform data buffers.
*/
void VirtualRealitySystem::CreateVirtualRealityUniformDataBuffers() NOEXCEPT
{
	//Create the virtual reality uniform data buffers.
	_VirtualRealityUniformDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &virtual_reality_uniform_data_buffer : _VirtualRealityUniformDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(VirtualRealityUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &virtual_reality_uniform_data_buffer);
	}
}