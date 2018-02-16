//Header file.
#include <Window.h>

//Third party libraries.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
	//Initialize GLFW.
	glfwInit();

	//Tell GLFW to use Vulkan.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Tell GLFW to offer no resizability.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Create the window!
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PROJECT_NAME, nullptr, nullptr);
}

/*
*	Updates this window.
*/
void Window::Update() const NOEXCEPT
{
	//Poll events.
	glfwPollEvents();
}

/*
*	Releases this window.
*/
void Window::Release() NOEXCEPT
{
	//Destroy the window.
	glfwDestroyWindow(window);

	//Terminate GLFW.
	glfwTerminate();
}

/*
*	Returns whether or not this window should close.
*/
bool Window::ShouldClose() const NOEXCEPT
{
	return static_cast<bool>(glfwWindowShouldClose(window));
}