#include "../pch.h"
#include "Input.h"
#include <unordered_map>

static std::unordered_map<int, bool> inputMap_s;
static WindowData* window_s;

void Input::init(WindowData* window)
{
	window_s = window;
}

void Input::updateKey(int key, bool isDown)
{
    inputMap_s[key] = isDown;
}

bool Input::isKeyDown(int key)
{
    if (!inputMap_s.contains(key))
        return false;

    return inputMap_s[key];
}

Vec2f Input::getMousePos()
{
    return window_s->MousePos;
}
