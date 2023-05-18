#pragma once

#define _USE_MATH_DEFINES
#include <string>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <format>
#include <unordered_map>
#include <array>
#include <functional>
#include <random>
#include <numbers>
#include <algorithm>
#include <iterator>

#include <chrono>
using namespace std::literals;

#include <../../external/imgui/imgui.h>
#include "../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"
#include "Engine/ImGuiMarkdown.h"

#include <glad/glad.h> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Engine/Texture.h"
#include "Engine/SubTexture2D.h"
#include "Engine/VertexArray.h"
#include "Engine/Shader.h"
#include "Engine/TextureManager.h"
#include "Engine/QuickMaths.h"
#include "Engine/App.h"
#include "Engine/Input.h"

#define LOG(...) std::cout << std::format(__VA_ARGS__) << std::endl
#define PROFILE(methodName) ScopeTimer timer(methodName)
#define TRANSLATE(x, y) glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0))
#define ROTATE(angle) glm::rotate(glm::mat4(1.0f), angle, {0, 0, 1})
#define SCALE(x, y) glm::scale(glm::mat4(1.0f), glm::vec3(x, y, 1))
