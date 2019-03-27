#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/Model.h>

//Resources.
#include <Resources/Data/ModelData.h>
#include <Resources/Data/Texture2DData.h>

class ResourceCreator final
{

public:

	/*
	*	Creates a model.
	*/
	static void CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	static void CreateTexture2D(Texture2DData *const RESTRICT data, CPUTexture2D<Vector4<byte>> *const RESTRICT texture) NOEXCEPT;

};