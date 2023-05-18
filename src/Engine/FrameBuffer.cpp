#include "../pch.h"
#include "FrameBuffer.h"
#include <glad/glad.h>

FrameBuffer::FrameBuffer(float width, float height) :
	width_(width),
	height_(height)
{
	glGenTextures(1, &colorBufferRendererId_);
	glBindTexture(GL_TEXTURE_2D, colorBufferRendererId_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &rendererId_);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererId_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferRendererId_, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &rendererId_);
}

void FrameBuffer::bind()
{
	if (isDirty_)
	{
		loadFromPath();
		isDirty_ = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, rendererId_);
	glViewport(0, 0, width_, height_);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::setSize(float width, float height)
{
	width_ = width;
	height_ = height;
	isDirty_ = true;
}

void FrameBuffer::loadFromPath()
{
	glDeleteFramebuffers(1, &rendererId_);
	glDeleteTextures(1, &colorBufferRendererId_);
	glGenTextures(1, &colorBufferRendererId_);
	glBindTexture(GL_TEXTURE_2D, colorBufferRendererId_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &rendererId_);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererId_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferRendererId_, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
