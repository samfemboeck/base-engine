#include "../pch.h"
#include "SubTexture2D.h"
#include "Texture.h"

SubTexture2D::SubTexture2D(Texture2D& texture, const glm::ivec2& spriteSize, const glm::ivec2 coords) :
	texture_(texture),
	spriteSize_(spriteSize)
{
	setCoords(coords);
}

void SubTexture2D::setCoords(const glm::vec2& coords)
{
	auto min = glm::vec2{ coords.x / texture_.getWidth(), coords.y / texture_.getHeight() };
	auto max = glm::vec2{ (coords.x + spriteSize_.x) / texture_.getWidth(), (coords.y + spriteSize_.y) / texture_.getHeight() };
	texCoords_[0] = glm::vec2{ min.x, min.y };
	texCoords_[1] = glm::vec2{ max.x, min.y };
	texCoords_[2] = glm::vec2{ max.x, max.y };
	texCoords_[3] = glm::vec2{ min.x, max.y };
}
