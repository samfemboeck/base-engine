#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/App.h"
#include "Engine/OrthoCamController.h"
#include "Engine/Timer.h"
#include <thread>
#include "Engine/Renderer2D.h"
#include "Engine/FrameBuffer.h"
#include "Engine/SoundManager.h"
#include "Engine/Sprite.h"
#include "Game.h"
#include "Windows.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#pragma comment(lib, "../Lua/liblua54.a")

void Game::onUpdate()
{
	App::onUpdate();
}

void Game::onDraw()
{	
}

void Game::onMousePressed(int button)
{
}

void Game::onKeyPressed(int key)
{
	App::onKeyPressed(key);
}

Game::Game()
{
	SoundManager::init();
};

void checkLua(lua_State* L, bool status)
{
	if (status != LUA_OK)
	{
		std::string msg = lua_tostring(L, -1);
		std::cout << msg << std::endl;
	}
}

int cpp(lua_State* l)
{
	double a = lua_tonumber(l, 1);
	double b = lua_tonumber(l, 2);
	std::cout << "cpp called with: " << a << ", " << b << std::endl;
	lua_pushnumber(l, 420);
	return 1;
}

int main()
{
	try
	{
		lua_State* l = luaL_newstate();
		luaL_openlibs(l);

		lua_register(l, "cpp", cpp);

		checkLua(l, luaL_dofile(l, "../Lua/scripts/test.lua"));

		lua_getglobal(l, "a");
		if (lua_isnumber(l, -1))
		{
			float aInCPP = lua_tonumber(l, -1);
			std::cout << aInCPP << std::endl;
		}

		lua_getglobal(l, "update");
		if (lua_isfunction(l, -1))
		{
			checkLua(l, lua_pcall(l, 0, 1, 0));
			auto result = lua_tonumber(l, -1);
			std::cout << result << std::endl;
		}

		Game game;
		game.start();
	}
	catch (std::exception e)
	{
		LOG("{}", e.what());
	}

	return 0;
}
