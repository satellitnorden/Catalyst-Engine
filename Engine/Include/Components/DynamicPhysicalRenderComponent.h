#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

class DynamicPhysicalRenderComponent final
{

public:

	//The physical flags.
	PhysicalFlag _PhysicalFlags;

	//The model.
	PhysicalModel _Model;

	//The model.
	PhysicalMaterial _Material;

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

};