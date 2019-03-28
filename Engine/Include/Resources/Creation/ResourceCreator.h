#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/Model.h>
#include <Rendering/Engine/TextureCube.h>
#include <Rendering/Engine/Texture2D.h>

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
	static void CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCube *const RESTRICT texture) NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	static void CreateTexture2D(Texture2DData *const RESTRICT data, Texture2D<Vector4<byte>> *const RESTRICT texture) NOEXCEPT;

};