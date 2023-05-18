#include "../pch.h"
#include "SoundManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include "MiniAudio.h"

static ma_engine* s_engine;
static std::unordered_map<std::string, ma_sound*> s_sounds;

void SoundManager::init()
{	
	ma_result result;
	s_engine = new ma_engine;
	result = ma_engine_init(NULL, s_engine);
	if (result != MA_SUCCESS)
		std::cout << "Failed to initialize audio engine.";
	else
		std::cout << "Audio engine initialized.";
}

void SoundManager::add(std::string path, std::string name)
{	
	ma_sound* sound = new ma_sound;
	ma_result result = ma_sound_init_from_file(s_engine, path.c_str(), 0, NULL, NULL, sound);
	s_sounds[name] = sound;
}

void SoundManager::oneShot(std::string path)
{
	ma_engine_play_sound(s_engine, path.c_str(), NULL);
}

void SoundManager::start(std::string filename)
{
	ma_sound_start(s_sounds[filename]);
}

void SoundManager::stop(std::string filename)
{
	ma_sound_stop(s_sounds[filename]);
}

bool SoundManager::isPlaying(std::string filename)
{
	return ma_sound_is_playing(s_sounds[filename]);
}

bool SoundManager::isEnd(std::string filename)
{
	return ma_sound_at_end(s_sounds[filename]);
}

void SoundManager::reset(std::string filename)
{
	ma_sound_seek_to_pcm_frame(s_sounds[filename], 0);
}

void SoundManager::fade(std::string filename, float begin, float end, float millis)
{
	ma_sound_set_fade_in_milliseconds(s_sounds[filename], begin, end, millis);
}
