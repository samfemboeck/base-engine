#pragma once

class TextureManager
{
public:
	static void add(std::string path, std::string key);
	static Texture2D* get(std::string filename);
};