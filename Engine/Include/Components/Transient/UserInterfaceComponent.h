#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/EulerAngles.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldPosition.h>

class UserInterfaceComponent final
{

public:

	//The user interface scene.
	UserInterfaceScene *RESTRICT _UserInterfaceScene;

	//The world position.
	WorldPosition _WorldPosition;

	//The rotation.
	EulerAngles _Rotation;

	//The scale.
	Vector2<float32> _Scale;

	//The roughness.
	float32 _Roughness;

	//The metallic.
	float32 _Metallic;

	//The ambient occlusion.
	float32 _AmbientOcclusion;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//The uniform buffers.
	DynamicArray<BufferHandle> _UniformBuffers;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

};