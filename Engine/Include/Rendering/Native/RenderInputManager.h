#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Assets/MaterialAsset.h>

//Rendering.
#include <Rendering/Native/RenderInputStream.h>

class RenderInputManager final
{

public:

	/*
	*	Initializes the render input manager.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the render input manager during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Registers an input stream.
	*/
	void RegisterInputStream
	(
		const HashString identifier,
		const DynamicArray<VertexInputAttributeDescription> &requited_vertex_input_attribute_descriptions,
		const DynamicArray<VertexInputBindingDescription> &required_vertex_input_binding_descriptions,
		const uint64 required_push_constant_data_size,
		const RenderInputStreamGatherFunction gather_function,
		const RenderInputStream::Mode mode,
		void *const RESTRICT user_data
	) NOEXCEPT;

	/*
	*	Returns the input stream with the given identifier.
	*/
	NO_DISCARD const RenderInputStream &GetInputStream(const HashString identifier) NOEXCEPT;

private:

	//The input streams.
	DynamicArray<RenderInputStream> _InputStreams;

	/*
	*	Gathers a depth model input stream.
	*/
	void GatherDepthModelInputStream
	(
		const MaterialAsset::Type material_type,
		const bool double_sided,
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a full model input stream.
	*/
	void GatherFullModelInputStream
	(
		const MaterialAsset::Type material_type,
		const bool double_sided,
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers an instanced model input stream.
	*/
	void GatherInstancedModelInputStream
	(
		const bool double_sided,
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers an instanced impostor input stream.
	*/
	void GatherInstancedImpostorInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a terrain input stream.
	*/
	void GatherTerrainInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a water input stream.
	*/
	void GatherWaterInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a grass input stream.
	*/
	void GatherGrassInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

#if defined(CATALYST_EDITOR)
	/*
	*	Gathers a model editor metadata input stream.
	*/
	void GatherModelEditorMetadataInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a plane editor metadata input stream.
	*/
	void GatherPlaneEditorMetadataInputStream
	(
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;
#endif

};