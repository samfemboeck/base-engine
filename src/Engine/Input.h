#pragma once

class Input
{
public:
	static void init(WindowData*);
	static void updateKey(int key, bool isDown);
	static bool isKeyDown(int key);
	static Vec2f getMousePos();
};