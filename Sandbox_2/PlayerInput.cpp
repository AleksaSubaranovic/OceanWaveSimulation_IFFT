#include "PlayerInput.h"
#include <GLFW/glfw3.h>
#include "Generic.h"

unordered_map<int, KeyEvent> PlayerInput::keyEventMap = unordered_map<int, KeyEvent>();
MouseEvent PlayerInput::lb = MouseEvent();
MouseEvent PlayerInput::rb = MouseEvent();

void PlayerInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key != KEY_(UNKNOWN))
	{
		auto eventIt = keyEventMap.find(key);
		if (eventIt != keyEventMap.end())
			eventIt->second(action, mods);
	}
}

void PlayerInput::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (lb) lb(action, mods);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (rb) rb(action, mods);
	}
}


void PlayerInput::BindKeyEvent(int key, KeyEvent event)
{
	try
	{
		//Returns a pair consisting of an iterator to the inserted element (or to the element that prevented the insertion) 
		// and a bool denoting whether the insertion took place (true if insertion happened, false if it did not). 
		bool KeyEventInserted = keyEventMap.insert({ key, event }).second;
		WARN_IF(not KeyEventInserted);
	}
	catch (std::exception e)
	{
		PRINT_ERROR("Bind Key error, key  \n");
		PRINT_ERROR(glfwGetKeyName(key, glfwGetKeyScancode(key)));
	}
}

void PlayerInput::UnbindKeyEvent(int key)
{
	try
	{
		bool KeyEventBinded = keyEventMap.find(key) == keyEventMap.end();
		WARN_IF(not KeyEventBinded); // error ?

		keyEventMap.erase(key);
	}
	catch (std::exception e)
	{
		PRINT_ERROR("Unbind Key error, key : %s \n");
		PRINT_ERROR(glfwGetKeyName(key, glfwGetKeyScancode(key)));
	}
}

void PlayerInput::BindMouseEvent(int button, MouseEvent event)
{
	try
	{
		if (button == LEFT_MOUSE_BUTTON())
		{
			if (!lb) lb = event;
			else WARN(lb);
		}
		else if (button == RIGHT_MOUSE_BUTTON())
		{
			if (!rb) rb = event;
			else WARN(rb)
		}
	}
	catch (std::exception e)
	{
		PRINT_ERROR("Bind mouse error \n");
	}
}

void PlayerInput::UnbindMouseEvent(int button)
{
	try
	{
		if (button == LEFT_MOUSE_BUTTON())
		{
			if (lb) lb = MouseEvent();
		}
		else if (button == RIGHT_MOUSE_BUTTON())
		{
			if (rb) rb = MouseEvent();
		}
	}
	catch (std::exception e)
	{
		PRINT_ERROR("Unbind mouse error \n");
	}
}
