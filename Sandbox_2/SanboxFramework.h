#pragma once

#include"glew_glfw.h"
#include <functional>
#include <chrono>

typedef std::function<void()> initEvent;
typedef std::function<void()> closeEvent;
typedef std::function<void(float)> updateEvent;

using namespace std::chrono;
typedef high_resolution_clock Clock;

class Camera;

namespace Sandbox
{
	extern GLFWwindow* window;
	extern initEvent _init;
	extern closeEvent _close;
	extern updateEvent _update;
	extern Clock::time_point lastTimePoint;
	extern Camera mainCamera;

	void initSandbox(int ScreenWidth, int ScreanHeigth, const char* name = "Sandbox");

	void updateSandbox();

	void closeSandbox();

	void BindEvents(initEvent init, updateEvent update, closeEvent close);

	void cursorCallback(GLFWwindow* window, double xpos, double ypos);
}