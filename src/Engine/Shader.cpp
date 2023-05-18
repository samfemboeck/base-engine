#include "../pch.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

static GLenum ShaderTypeFromString(const std::string& type)
{
	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel")
		return GL_FRAGMENT_SHADER;

	return 0;
}

Shader::Shader(const std::string& filepath)
{
	std::string source = readFile(filepath);
	auto shaderSources = preProcess(source);
	compile(shaderSources);

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	name_ = filepath.substr(lastSlash, count);
}

Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) :
	name_(name)
{
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSrc;
	sources[GL_FRAGMENT_SHADER] = fragmentSrc;
	compile(sources);
}

Shader::~Shader()
{
	glDeleteProgram(id_);
}

void Shader::bind() const
{
	glUseProgram(id_);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::uploadUniformInt(const std::string& name, int value) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform1i(location, value);
}

void Shader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform1iv(location, count, values);
}

void Shader::uploadUniformFloat(const std::string& name, float value) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform1f(location, value);
}

void Shader::uploadUniformFloat2(const std::string& name, const glm::vec2& value) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void Shader::uploadUniformFloat2Array(const std::string& name, float* values, uint32_t count) const
{	
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform2fv(location, count, values);
}

void Shader::uploadUniformFloat3(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::uploadUniformFloat3Array(const std::string& name, float* values, uint32_t count) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform3fv(location, count, values);
}

void Shader::uploadUniformFloat4(const std::string& name, const glm::vec4& value) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
	GLint location = glGetUniformLocation(id_, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::readFile(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);

	in.seekg(0, std::ios::end);
	result.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&result[0], result.size());
	in.close();
	return result;
}

std::unordered_map<GLenum, std::string> Shader::preProcess(const std::string& source)
{
	std::unordered_map<GLenum, std::string> shaderSources;
	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos);

		size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
	}

	return shaderSources;
}

void Shader::compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
	GLuint program = glCreateProgram();
	std::array<GLenum, 3> glShaderIDs;
	int glShaderIDIdx = 0;
	for (auto& kv : shaderSources)
	{
		GLenum type = kv.first;
		const std::string& source = kv.second;

		GLuint shader = glCreateShader(type);

		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			std::cout << "Shader compilation failed for '"  << kv.second << "':" << std::endl << &infoLog[0] << std::endl;

			glDeleteShader(shader);

			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[glShaderIDIdx++] = shader;
	}

	id_ = program;

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);

		for (auto id : glShaderIDs)
			glDeleteShader(id);

		return;
	}

	for (int i = 0; i < glShaderIDIdx; ++i)
		glDetachShader(program, glShaderIDs[i]);
}
