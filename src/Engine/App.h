#pragma once

class App;

struct WindowData
{
	int Width;
	int Height;
	Vec2f MousePos;
	App* App;
};

class App
{
public:
	App();
	~App();
	void start();

protected:
	virtual void onShutdown();
	virtual void onUpdate();
	virtual void onDraw();
	virtual void onMouseScrolled(double offsetX, double offsetY);
	virtual void onMousePressed(int button) {};
	virtual void onMouseReleased(int button) {};
	virtual void onKeyPressed(int key);
	virtual void onKeyReleased(int key);
	virtual void onWindowResize(int width, int height);
	virtual void onWindowClosed() {};
	virtual void onDrawImGui() {};

protected:
	WindowData windowData_;
	GLFWwindow* window_;
	bool isRunning_ = true;
	float aspectRatio = 16 / 9.0f;
};
