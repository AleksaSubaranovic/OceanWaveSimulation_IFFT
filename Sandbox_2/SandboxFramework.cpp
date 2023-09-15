#include "SanboxFramework.h"
#include <stdio.h>
#include <chrono>
#include "PlayerInput.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

GLFWwindow* Sandbox::window = NULL;
initEvent Sandbox::_init;
closeEvent Sandbox::_close;
updateEvent Sandbox::_update;
Clock::time_point Sandbox::lastTimePoint = Clock::now();
Camera Sandbox::mainCamera = Camera();

void Sandbox::initSandbox(int ScreenWidth, int ScreanHeigth, const char* name)
{
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4); // ANTI ALIASING

    Sandbox::window = glfwCreateWindow(ScreenWidth, ScreanHeigth, name, nullptr, nullptr); // windowed

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        // error
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // post init

    glfwSetKeyCallback(window, PlayerInput::keyCallback);
    glfwSetMouseButtonCallback(window, PlayerInput::mouseCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    Shader::initBasicShaders();

    if (_init) _init();
}

void Sandbox::updateSandbox()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        // calc delta time
        auto time = Clock::now() - lastTimePoint;
        lastTimePoint = Clock::now();
        float deltaTime = std::chrono::duration<float, std::ratio<1, 1>>(time).count();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw stuff
        if (_update) _update(deltaTime);


        if (glGetError())
        {
            fprintf(stderr, "Greska GL, end of update function\n");
            return;
        }
    }
}

void Sandbox::closeSandbox()
{
    if (_close) _close();

    glfwTerminate();
}

void Sandbox::BindEvents(initEvent init, updateEvent update, closeEvent close)
{
    _init = init;
    _close = close;
    _update = update;
}
bool firstMouse = true;
void Sandbox::cursorCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    static float lastX, lastY;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

   

    mainCamera.onCursorMove(glm::vec2(xoffset, yoffset));
}
