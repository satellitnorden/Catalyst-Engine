#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorEntitySystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Creates an entity.
*/
void EditorEntitySystem::CreateEntity() NOEXCEPT
{
	//Retrieve the current camera.
	const Camera *const RESTRICT current_camera{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera() };

	//Calculate the spawn position.
	Vector3<float32> spawn_position;

	{
		//Just do in front of the camera right now.
		const WorldTransform &camera_world_transform{ current_camera->GetWorldTransform() };

		spawn_position = camera_world_transform.GetAbsolutePosition() + camera_world_transform.GetForwardVector() * 8.0f;
	}
}
#endif