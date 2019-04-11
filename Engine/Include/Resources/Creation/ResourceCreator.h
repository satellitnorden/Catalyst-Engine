#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/GlobalTexture2D.h>
#include <Rendering/Native/Model.h>
#include <Rendering/Native/TextureCube.h>

//Resources.
#include <Resources/Data/ModelData.h>
#include <Resources/Data/TextureCubeData.h>
#include <Resources/Data/Texture2DData.h>

class ResourceCreator final
{

public:

	/*
	*	Creates a model.
	*/
	static void CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	static void CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCubeHandle *const RESTRICT texture) NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	static void CreateTexture2D(Texture2DData *const RESTRICT data, GlobalTexture2D *const RESTRICT texture) NOEXCEPT;

};