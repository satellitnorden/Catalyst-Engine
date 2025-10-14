#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/RawDataData.h>
#include <Resources/Loading/RenderPipelineData.h>
#include <Resources/Loading/Texture3DData.h>

//Forward declarations.
class BinaryInputFile;

class ResourceLoadingSystem final
{

public:

	/*
	*	Given a file, load raw data data.
	*/
	void LoadRawData(BinaryInputFile *const RESTRICT file, RawDataData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load render pipeline data.
	*/
	void LoadRenderPipeline(BinaryInputFile *const RESTRICT file, RenderPipelineData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture 3D data.
	*/
	void LoadTexture3D(BinaryInputFile *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT;

};