#pragma once

#include <unordered_map>
#include <functional>

struct GLFWwindow;
namespace Sandbox
{
	extern GLFWwindow* window;
}

// USE CAPS LOCK
#define KEY_(key) GLFW_KEY_ ## key
#define IS_PRESSED(action) (action == GLFW_PRESS)
#define IS_RELEASED(action) (action == GLFW_RELEASE)
#define IS_REPEATING(action) (action == GLFW_REPEAT)

#define GET_KEY_STATE(code) glfwGetKey(Sandbox::window, KEY_(code))

#define LEFT_MOUSE_BUTTON() GLFW_MOUSE_BUTTON_LEFT
#define RIGHT_MOUSE_BUTTON() GLFW_MOUSE_BUTTON_RIGHT
#define GET_MOUSE_POS(x, y) glfwGetCursorPos(&x, &y);
#define DISABLE_CURSOR() glfwSetInputMode(Sandbox::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#define HIDE_CURSOR() glfwSetInputMode(Sandbox::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#define RESET_CURSOR() glfwSetInputMode(Sandbox::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

#define GET_TIME() glfwGetTime()

#define IS_SHIFT(mod) (mod & GLFW_MOD_SHIFT)
#define IS_ALT(mod) (mod & GLFW_MOD_ALT)
#define IS_CTRL(mod) (mod & GLFW_MOD_CTRL)
#define IS_CAPS(mod) (mod & GLFW_MOD_CAPS_LOCK)
// cursor objects to do


using std::unordered_map;
using std::function;

typedef function<void(int, int)> KeyEvent;
typedef function<void(int, int)> MouseEvent;
class PlayerInput
{
	static unordered_map<int, KeyEvent> keyEventMap;
	static MouseEvent lb, rb;

public:
	// Bind key event to key (get key with KEY_(KEY)), event => void(int action, int mods)
	static void BindKeyEvent(int key, KeyEvent event);

	// Unbind key event
	static void UnbindKeyEvent(int key);

	// Bind mouse event to mouse button (get button with LET/RIGHT_MOUSE_BUTTON), event => void(int action, int mods)
	static void BindMouseEvent(int button, MouseEvent event);

	static void UnbindMouseEvent(int button);

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
};

