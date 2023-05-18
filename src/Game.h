#pragma once

#include "Engine/App.h"
#include "Engine/FrameBuffer.h"

class Game : public App
{
public:
	Game();
	void onUpdate() override;
	void onDraw() override;
	void onMousePressed(int button) override;
	void onKeyPressed(int key) override;
};
