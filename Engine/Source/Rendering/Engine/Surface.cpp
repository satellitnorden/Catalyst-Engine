//Header file.
#include <Rendering/Engine/Surface.h>

//Systems.
#include <Systems/EngineSystem.h>

//Preprocessor defines.
#define WINDOW_WIDTH 1'920
#define WINDOW_HEIGHT 1'080

/*
*	Default constructor.
*/
Surface::Surface() NOEXCEPT
	:
	surface(nullptr)
{
	
}

/*
*	Default destructor.
*/
Surface::~Surface() NOEXCEPT
{

}

/*
*	Initializes this surface.
*/
void Surface::Initialize() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Initialize GLFW.
	glfwInit();

	//Tell GLFW to use Vulkan.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Tell GLFW to offer no resizability.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Create the window!
	surface = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, EngineSystem::Instance->GetProjectInformation().generalInformation.projectName.CString(), nullptr, nullptr);
#endif
}

/*
*	Updates this surface.
*/
void Surface::Update() const NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Poll events.
	glfwPollEvents();
#endif
}

/*
*	Releases this surface.
*/
void Surface::Release() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Destroy the window.
	glfwDestroyWindow(static_cast<GLFWwindow *const RESTRICT>(surface));

	//Terminate GLFW.
	glfwTerminate();
#endif
}

/*
*	Returns whether or not this surface should close.
*/
bool Surface::ShouldClose() const NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	return static_cast<bool>(glfwWindowShouldClose(static_cast<GLFWwindow *const RESTRICT>(surface)));
#else
	return false;
#endif
}