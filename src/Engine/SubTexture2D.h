#pragma once

class Texture2D;

class SubTexture2D
{
public:
	SubTexture2D(Texture2D& texture, const glm::ivec2& spriteSize, const glm::ivec2 coords = {0, 0});

	Texture2D& getTexture() const { return texture_; }
	const glm::vec2* getTexCoords() { return texCoords_; }
	const glm::vec2 getSize() { return spriteSize_; }
	void setCoords(const glm::vec2& coords);

private:
	Texture2D& texture_;
	glm::vec2 texCoords_[4];
	glm::vec2 spriteSize_;
};
