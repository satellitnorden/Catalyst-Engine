#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/Platforms/Linux/Internal/X11/X11WindowManager.h>

//Third party.
#include <GL/glx.h>
#include <GL/glxext.h>

/*
*   X11 OpenGL context class definition.
*/
class X11OpenGLContext final
{

public:

    //The context.
    GLXContext _Context;

};

/*
*	Prepares OpenGL for the given window.
*/
void X11WindowManager::PrepareOpenGL(const Platform::WindowHandle window) NOEXCEPT
{
#if defined(CATALYST_HAS_OPEN_GL)
	//Cache the window data.
	WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

    //Set the current OpenGL context.
    glXMakeCurrent
    (
        _Context._Display,
        window_data->_X11WindowData._Window,
        window_data->_X11WindowData._OpenGLContext.Get<X11OpenGLContext>()->_Context
    );
#endif
}

/*
*	Finalizes OpenGL for the given window.
*/
void X11WindowManager::FinalizeOpenGL(const Platform::WindowHandle window) NOEXCEPT
{
#if defined(CATALYST_HAS_OPEN_GL)
	//Cache the window data.
	WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

    //Swap the buffers.
    glXSwapBuffers
    (
        _Context._Display,
        window_data->_X11WindowData._Window
    );
#endif
}

/*
*	Tries to create an OpenGL window.
*/
NO_DISCARD Window X11WindowManager::TryCreateOpenGLWindow
(
	const Platform::WindowManager::CreateParameters &parameters,
	const Window parent,
	Any<X11WindowData::X11_OPEN_GL_CONTEXT_SIZE> *const RESTRICT open_gl_context
) NOEXCEPT
{
    //Choose the framebuffer config.
    constexpr int32 FRAMEBUFFER_ATTRIBUTES[]
    {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,

        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,

        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,

        GLX_DOUBLEBUFFER, True,

        None
    };

    int32 number_of_framebuffer_configurations{ 0 };

    const GLXFBConfig *const RESTRICT framebuffer_configurations
    {
        glXChooseFBConfig
        (
            _Context._Display,
            _Context._DefaulScreen,
            FRAMEBUFFER_ATTRIBUTES,
            &number_of_framebuffer_configurations
        )
    };

    if (framebuffer_configurations == nullptr || number_of_framebuffer_configurations == 0)
    {
        return None;
    }

    //Retrieve the visual info.
    const XVisualInfo *const RESTRICT visual_info{ glXGetVisualFromFBConfig(_Context._Display, framebuffer_configurations[0]) };

    if (!visual_info)
    {
        return None;
    }

    //Create the colormap.
    const Colormap colormap
    {
        XCreateColormap
        (
            _Context._Display,
            parent,
            visual_info->visual,
            AllocNone
        )
    };

    if (colormap == None)
    {
        return None;
    }

    //Configure the window attributes.
    XSetWindowAttributes window_attributes{ };

    window_attributes.colormap = colormap;
    window_attributes.border_pixel = 0;
    window_attributes.event_mask = StructureNotifyMask;

    //Create the window!
    const Window window
    {
        XCreateWindow
        (
            _Context._Display,
            parent,
            0,
            0,
            parameters._Resolution._Width,
            parameters._Resolution._Height,
            0,
            visual_info->depth,
            InputOutput,
            visual_info->visual,
            CWBorderPixel | CWColormap | CWEventMask,
            &window_attributes
        )
    };

    if (window == None)
    {
        return None;
    }

    //Retrieve the create context function.
    const PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB
    {
        reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>
        (
            glXGetProcAddressARB
            (
                reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")
            )
        )
    };
    
    if (!glXCreateContextAttribsARB)
    {
        return None;
    }

    //Create the context.
    constexpr int32 CONTEXT_ATTRIBUTES[]
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    const GLXContext context
    {
        glXCreateContextAttribsARB
        (
            _Context._Display,
            framebuffer_configurations[0],
            nullptr,
            True,
            CONTEXT_ATTRIBUTES
        )
    };

    if (context == None)
    {
        return None;
    }

    //Set the current OpenGL context.
    glXMakeCurrent
    (
        _Context._Display,
        window,
        context
    );

    //Retrieve the function pointers for OpenGL.
    OpenGL::RetrieveOpenGLFunctionPointers();

    //Set the OpenGL context.
    open_gl_context->Get<X11OpenGLContext>()->_Context = context;

    //return the window.
    return window;
}
#endif