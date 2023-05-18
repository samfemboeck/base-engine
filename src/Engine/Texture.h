#pragma once

class Texture2D
{
public:
	Texture2D(const std::string& path, bool flipVertically, bool mirroredRepeat);
	~Texture2D();

	uint32_t getWidth() const { return width_; }
	uint32_t getHeight() const { return height_; }
	uint32_t getId() const { return id_; }
	void bind(uint32_t slot = 0) const;
	std::string getFilePath() { return path_; }
	void loadFromPath(const std::string& path, bool mirrored_repeat, bool flipVertically);
	float getAspect() const { return width_ / (float)height_; }

	bool operator==(const Texture2D& other) const { return id_ == other.id_; }
private:
	std::string path_;
	uint32_t width_, height_;
	uint32_t id_;
};
