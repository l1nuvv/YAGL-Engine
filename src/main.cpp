#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

struct WindowState {
    int width;
    int height;
};


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    WindowState *state = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
    state->width = width;
    state->height = height;

    glViewport(0, 0, width, height);
    spdlog::info("Change window size to: {}x{}", state->width, state->height);
}

int main()
{
    // init GLFW
    glfwInit();

    // tell GLFW what version it should use
    spdlog::info("OpenGL version: 4.6(CORE_PROFILE).");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // define window size
    WindowState windowState{1200, 720};

    // create a GLFW window with OpenGL context
    spdlog::info("Creating GLFW window {}x{}...", windowState.width, windowState.height);
    spdlog::info("GLM vector size: {}", sizeof(glm::vec3));
    GLFWwindow *window =
            glfwCreateWindow(windowState.width, windowState.height, "Danek", nullptr, nullptr);

    glfwSetWindowUserPointer(window, &windowState);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!window)
    {
        // log error and terminate GLFW if window creation failed
        spdlog::error("Failed to create GLFW window!");
        glfwTerminate();
        return -1;
    }

    // make the OpenGl context of the window current on the calling thread
    glfwMakeContextCurrent(window);

    // load OpenGL function pointer by using GLAD
    spdlog::info("Loading OpenGl functions by GLAD...");
    if (!gladLoadGLLoader((GLADloadproc) (glfwGetProcAddress)))
    {
        spdlog::error("Failed to initialize GLAD!");
        return -1;
    }

    spdlog::info("OpenGL Vendor: {}", (const char *) glGetString(GL_VENDOR));
    spdlog::info("OpenGL Renderer: {}", (const char *) glGetString(GL_RENDERER));
    spdlog::info("OpenGL Version: {}", (const char *) glGetString(GL_VERSION));
    spdlog::info("GLSL Version: {}",
                 (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    // set the viewport to match the window size
    glViewport(0, 0, windowState.width, windowState.height);
    spdlog::debug("Viewport set to {}x{}...", windowState.width, windowState.height);

    // main application loop
    spdlog::info("Entering main loop...");
    while (!glfwWindowShouldClose(window))
    {
        WindowState *state = static_cast<WindowState *>(glfwGetWindowUserPointer(window));

        // set background color to specific color and clear the screen
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // dark blue background for now
        glClear(GL_COLOR_BUFFER_BIT); // clear the color buffer

        glfwPollEvents(); // process pending events like keyboard or mouse input
        glfwSwapBuffers(window);
    }

    // clean up resources before exiting
    spdlog::info("Destroy main loop!");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
