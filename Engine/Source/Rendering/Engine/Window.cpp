//Header file.
#include <Rendering/Engine/Window.h>

//Systems.
#include <Systems/EngineSystem.h>

#if defined(CATALYST_WINDOWS)
//Third party libraries.
#include <ThirdParty/GLFW/glfw3.h>
#endif

//Preprocessor defines.
#define WINDOW_WIDTH 1'920
#define WINDOW_HEIGHT 1'080

/*
*	Default constructor.
*/
Window::Window() NOEXCEPT
	:
	window(nullptr),
	width(WINDOW_WIDTH),
	height(WINDOW_HEIGHT)
{
	
}

/*
*	Default destructor.
*/
Window::~Window() NOEXCEPT
{

}

/*
*	Initializes this window.
*/
void Window::Initialize() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Initialize GLFW.
	glfwInit();

	//Tell GLFW to use Vulkan.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Tell GLFW to offer no resizability.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Create the window!
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, EngineSystem::Instance->GetProjectInformation().generalInformation.projectName.CString(), nullptr, nullptr);
#endif
}

/*
*	Updates this window.
*/
void Window::Update() const NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Poll events.
	glfwPollEvents();
#endif
}

/*
*	Releases this window.
*/
void Window::Release() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Destroy the window.
	glfwDestroyWindow(window);

	//Terminate GLFW.
	glfwTerminate();
#endif
}

/*
*	Returns whether or not this window should close.
*/
bool Window::ShouldClose() const NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	return static_cast<bool>(glfwWindowShouldClose(window));
#else
	return false;
#endif
}