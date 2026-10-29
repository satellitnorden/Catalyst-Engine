#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Task.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>

//Path tracing.
#include <PathTracing/AccelerationStructure.h>
#include <PathTracing/Camera.h>
#include <PathTracing/Light.h>
#include <PathTracing/Material.h>
#include <PathTracing/Object.h>

//Rendering.
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/Texture2D.h>

namespace PathTracing
{

	/*
	*	Scene class.
	*	Holds all of the scene data used to render the final image.
	*/
	class Scene final
	{

	public:

		//Type aliases.
		using PartialRenderCallback = void(*)
		(
			const uint32 number_of_samples,
			const Texture2D<Vector4<float32>> &texture,
			void *const RESTRICT user_data
		);

		/*
		*	Default destructor.
		*/
		~Scene() NOEXCEPT;

		/*
		*	Adds the camera.
		*/
		template <typename TYPE, class... Arguments>
		FORCE_INLINE void AddCamera(Arguments&&... arguments) NOEXCEPT
		{
			ASSERT(!_Camera, "Camera already added!");

			_Camera = new TYPE(std::forward<Arguments>(arguments)...);
		}
		
		/*
		*	Adds a material to the scene.
		*/
		FORCE_INLINE NO_DISCARD PathTracing::Material *const RESTRICT AddMaterial() NOEXCEPT
		{
			PathTracing::Material *const RESTRICT material{ new Material() };
			_Materials.Emplace(material);
			return material;
		}

		/*
		*	Adds an object to the scene.
		*/
		template <typename TYPE, class... Arguments>
		FORCE_INLINE void AddObject(Arguments&&... arguments) NOEXCEPT
		{
			//Add the object.
			_Objects.Emplace(new TYPE(std::forward<Arguments>(arguments)...));
		}

		/*
		*	Adds a light to the scene.
		*/
		template <typename TYPE, class... Arguments>
		FORCE_INLINE void AddLight(Arguments&&... arguments) NOEXCEPT
		{
			//Add the light.
			_Lights.Emplace(new TYPE(std::forward<Arguments>(arguments)...));
		}

		/*
		*	Sets the miss radiance.
		*/
		FORCE_INLINE void SetMissRadiance(const Vector4<float32> &value) NOEXCEPT
		{
			_MissRadiance = value;
		}

		/*
		*	Sets the maximum ambient occlusion distance.
		*/
		FORCE_INLINE void SetMaximumAmbientOcclusionDistance(const float32 value) NOEXCEPT
		{
			_MaximumAmbientOcclusionDistance = value;
		}

		/*
		*	Sets the partial render callback.
		*/
		FORCE_INLINE void SetPartialRenderCallback(const PartialRenderCallback callback, void *const RESTRICT user_data = nullptr) NOEXCEPT
		{
			_PartialRenderCallback = callback;
			_PartialRenderCallbackUserData = user_data;
		}

		/*
		*	Sets whether or not to do multi-threading.
		*/
		FORCE_INLINE void SetDoMultithreading(const bool value) NOEXCEPT
		{
			_DoMultithreading = value;
		}

		/*
		*	Sets whether or not to do logging.
		*/
		FORCE_INLINE void SetDoLogging(const bool value) NOEXCEPT
		{
			_DoLogging = value;
		}

		/*
		*	Returns the bounding box of this scene.
		*/
		NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT;

		/*
		*	Renders this scene.
		*/
		NO_DISCARD Texture2D<Vector4<float32>> Render
		(
			const Resolution resolution,
			const uint32 number_of_samples
		) NOEXCEPT;

	private:

		/*
		*	Task data class definition.
		*/
		class TaskData final
		{

		public:

			//The scene.
			Scene *RESTRICT _Scene;

			//The task.
			Task _Task;

			//The resolution.
			Resolution _Resolution;

			//The Y.
			uint32 _Y;

			//The intermediate render target.
			Texture2D<Vector4<float64>> *RESTRICT _IntermediateRenderTarget;

		};

		/*
		*	Radiance result class definition.
		*	Stores the result of a call to the 'Radiance' function.
		*/
		class RadianceResult final
		{

		public:
			
			//Denotes whether or not there was a hit.
			bool _Hit{ false };

			//The radiance.
			Vector3<float32> _Radiance{ 0.0f, 0.0f, 0.0f };

			//The opacity.
			float32 _Opacity{ 0.0f };

		};

		//The camera.
		PathTracing::Camera *RESTRICT _Camera{ nullptr };

		//The materials.
		DynamicArray<PathTracing::Material *RESTRICT> _Materials;

		//The objects.
		DynamicArray<PathTracing::Object *RESTRICT> _Objects;

		//The lights.
		DynamicArray<PathTracing::Light *RESTRICT> _Lights;

		//The miss radiance.
		Vector4<float32> _MissRadiance{ 0.0f, 0.0f, 0.0f, 0.0f };

		//The maximum ambient occlusion distance.
		float32 _MaximumAmbientOcclusionDistance{ FLOAT32_MAXIMUM };

		//The partial render callback.
		PartialRenderCallback _PartialRenderCallback{ nullptr };

		//The partial render callback user data.
		void *RESTRICT _PartialRenderCallbackUserData{ nullptr };

		//Denotes whether or not to do multi-threading.
		bool _DoMultithreading{ true };

		//Denotes whether or not to do logging.
		bool _DoLogging{ true };

		//The acceleration structure.
		PathTracing::AccelerationStructure _AccelerationStructure;

		/*
		*	Returns if this scene is ready for rendering.
		*/
		NO_DISCARD bool IsReadyForRendering() const NOEXCEPT;

		/*
		*	Renders for the given task data.
		*/
		void RenderTask(const TaskData &task_data) NOEXCEPT;

		/*
		*	Returns the outgoing radiance in the opposite direction of the given ray.
		*/
		NO_DISCARD RadianceResult Radiance(const Ray &ray, const uint32 depth, const uint32 max_depth) NOEXCEPT;

		/*
		*	Offsets the given hit position.
		*/
		NO_DISCARD Vector3<float32> OffsetHitPosition(const Vector3<float32> &hit_position, const Vector3<float32> &geometry_normal) NOEXCEPT;

		/*
		*	Returns if the partial render callback should be called.
		*/
		NO_DISCARD bool ShouldCallPartialRenderCallback(const uint32 number_of_samples) NOEXCEPT;

		/*
		*	Averages the given render target into a texture.
		*/
		void AverageRenderTarget
		(
			const Texture2D<Vector4<float64>> &render_target,
			const uint32 number_of_samples,
			Texture2D<Vector4<float32>> *const RESTRICT texture
		) NOEXCEPT;

	};

}