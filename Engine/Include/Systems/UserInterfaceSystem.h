#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Math.
#include <Math/General/Curve.h>

//Systems.
#include <Systems/System.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>
#include <UserInterface/UserInterfacePrimitiveDescription.h>
#include <UserInterface/UserInterfaceScene.h>

class UserInterfaceSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		UserInterfaceSystem,
		SYSTEM_INITIALIZE()
		SYSTEM_UPDATE(RANGE(USER_INTERFACE, PRE_RENDER))
		SYSTEM_TERMINATE()
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UserInterfaceSystem() NOEXCEPT
	{

	}

	/*
	*	Creates a user interface primitive.
	*/
	RESTRICTED NO_DISCARD UserInterfacePrimitive *const RESTRICT CreateUserInterfacePrimitive(const UserInterfacePrimitiveDescription *const RESTRICT description) NOEXCEPT;

	/*
	*	Destroys a user interface primitive.
	*/
	void DestroyUserInterfacePrimitive(UserInterfacePrimitive *const RESTRICT primitive) NOEXCEPT;

	/*
	*	Activates the given scene.
	*/
	FORCE_INLINE void ActivateScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_ActivationQueue.Push(scene);
	}

	/*
	*	Deactivates the given scene.
	*/
	FORCE_INLINE void DeactivateScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_DeactivationQueue.Push(scene);
	}

	/*
	*	Destroys a user interface scene.
	*/
	FORCE_INLINE void DestroyUserInterfaceScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_DestructionQueue.Push(scene);
	}

	/*
	*	Deactivates all active scenes.
	*/
	FORCE_INLINE void DeactivateAllActiveScenes() NOEXCEPT
	{
		for (UserInterfaceScene *const RESTRICT scene : _ActiveUserInterfaceScenes)
		{
			DeactivateScene(scene);
		}
	}

private:

	/*
	*	User interface instance class definition.
	*	This is the data that is sent to the GPU.
	*/
	class ALIGN(16) UserInterfaceInstance final
	{

	public:

		//The positions.
		StaticArray<Vector4<float32>, 4> _Positions;

		//The texture coordinates.
		StaticArray<Vector2<float32>, 4> _TextureCoordinates;

		//The flags.
		uint32 _Flags;

		//The color/texture.
		uint32 _ColorOrTexture;

		//The color/opacity.
		uint32 _ColorOpacity;

		//The smoothing factor.
		float32 _SmoothingFactor;

	};

	/*
	*	User interface render information.
	*/
	class UserInterfaceRenderInformation final
	{

	public:

		//The transformation.
		Matrix4x4 _Transformation;

		//The start instance.
		uint32 _StartInstance;

		//The number of instances.
		uint32 _NumberOfInstances;

	};

	//The text smoothing factor curve.
	Curve<float32, 5> _TextSmoothingFactorCurve;

	//The active user interface scenes.
	DynamicArray<UserInterfaceScene *RESTRICT> _ActiveUserInterfaceScenes;

	//The activation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _ActivationQueue;

	//The deactivation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DeactivationQueue;

	//The destruction queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DestructionQueue;

	//Container for all user interface primitives.
	DynamicArray<UserInterfacePrimitive *RESTRICT> _UserInterfacePrimitives;

	//The instances.
	DynamicArray<UserInterfaceInstance> _Instances;

	//The render information.
	UserInterfaceRenderInformation _RenderInformation;

	//The render informations for the 3D user interface scenes.
	DynamicArray<UserInterfaceRenderInformation> _3DRenderInformations;

	/*
	*	The storage buffer update.
	*/
	void StorageBufferUpdate(DynamicArray<byte> *const RESTRICT data) NOEXCEPT;

	/*
	*	Adds instances for the given primitive.
	*/
	void AddInstances(const UserInterfacePrimitive *const RESTRICT primitive, const bool is_3D, const float32 opacity) NOEXCEPT;

	/*
	*	The render input stream update.
	*/
	void RenderInputStreamUpdate(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

	/*
	*	The render input stream 3Dupdate.
	*/
	void RenderInputStream3DUpdate(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};