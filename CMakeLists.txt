cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)

project ("Game")

add_executable(game
	external/glad/include/glad/glad.h
	external/glad/src/glad.c
	external/stb/stb_image.h
	external/imgui/imgui.h
	external/imgui/imgui.cpp
	external/imgui/imconfig.h
	external/imgui/imgui_demo.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_internal.h
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/imstb_rectpack.h
	external/imgui/imstb_textedit.h
	external/imgui/imstb_truetype.h
	external/imgui/backends/imgui_impl_opengl3.h
	external/imgui/backends/imgui_impl_opengl3.cpp
	external/imgui/backends/imgui_impl_glfw.h
	external/imgui/backends/imgui_impl_glfw.cpp

	src/Engine/Shader.h
	src/Engine/Shader.cpp
	src/Engine/Buffer.h
	src/Engine/Buffer.cpp
	src/Engine/Camera.h
	src/Engine/Camera.cpp
	src/Engine/FrameBuffer.h
	src/Engine/FrameBuffer.cpp
	src/Engine/Renderer2D.h
	src/Engine/Renderer2D.cpp
	src/Engine/SubTexture2D.h
	src/Engine/SubTexture2D.cpp
	src/Engine/Texture.h
	src/Engine/Texture.cpp
	src/Engine/VertexArray.h
	src/Engine/VertexArray.cpp
	src/Engine/TextureManager.h
	src/Engine/TextureManager.cpp
	src/Engine/Component.h
	src/Engine/Component.cpp
	src/Engine/Physics.h
	src/Engine/Physics.cpp
	src/Engine/QuickMaths.h
	src/Engine/QuickMaths.cpp
	src/Engine/OrthoCamController.h
	src/Engine/OrthoCamController.cpp
	src/Engine/Timer.h
	src/Engine/Timer.cpp
	src/Engine/Time.h
	src/Engine/App.h
	src/Engine/App.cpp
	src/Engine/ImGuiHandler.h
	src/Engine/ImGuiHandler.cpp
	src/Engine/EntityGrid.h
	src/Engine/EntityGrid.cpp
	src/Engine/Util.h
	src/Engine/Util.cpp
	src/Engine/MiniAudio.h
	src/Engine/SoundManager.h
	src/Engine/SoundManager.cpp
	src/Engine/ImGuiMarkdown.h
	src/Engine/Sprite.h
	src/Engine/Sprite.cpp
	src/Engine/Input.h
	src/Engine/Input.cpp

	src/Game.h
	src/Game.cpp
 )

add_subdirectory ("external/glfw")
add_subdirectory ("external/glm")
add_subdirectory ("external/freetype-2.13.0")

target_precompile_headers(game
	INTERFACE src/pch.h
	)

target_include_directories (game
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glad/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include
	PUBLIC ${PROJECT_SOURCE_DIR}/external/glm
	PUBLIC ${PROJECT_SOURCE_DIR}/external/stb
	PUBLIC ${PROJECT_SOURCE_DIR}/external/imgui
	PUBLIC ${PROJECT_SOURCE_DIR}/external/freetype-2.13.0/include
	PUBLIC ${PROJECT_SOURCE_DIR}/Lua/include
	)

target_link_libraries (
	game
	glfw
	glm
	freetype
)

add_custom_command(TARGET game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Textures
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Audio ${CMAKE_BINARY_DIR}/Audio
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Lua ${CMAKE_BINARY_DIR}
	)
