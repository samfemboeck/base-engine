#pragma once

#include "Texture.h"

class FrameBuffer
{
public:
	FrameBuffer(float width, float height);
	~FrameBuffer();
	void bind();
	void unbind();
	void setSize(float width, float height);
	void loadFromPath();
	int getColorBufferRendererId() { return colorBufferRendererId_; }

private:
	float width_ = 0, height_ = 0;
	bool isDirty_ = false;
	uint32_t rendererId_;
	uint32_t colorBufferRendererId_;
};
