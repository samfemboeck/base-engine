#include "../pch.h"
#include "TextureManager.h"

static std::unordered_map<std::string, Texture2D*> s_textures;

void TextureManager::add(std::string path, std::string key)
{
	Texture2D* texture = new Texture2D(path, false, false);
	s_textures.insert({ key, texture });
}

Texture2D* TextureManager::get(std::string key)
{
	auto it = s_textures.find(key);

	if (it != s_textures.end())
		return it->second;
	else
		return nullptr;
}
