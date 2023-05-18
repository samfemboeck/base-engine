#pragma once

typedef unsigned int GLenum;

class Shader
{
public:
	Shader(const std::string& filepath);
	Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	~Shader();

	const std::string& getName() const { return name_; };

	void bind() const;
	void unbind() const;

	void uploadUniformInt(const std::string& name, int value) const;
	void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);
	void uploadUniformFloat(const std::string& name, float value) const;
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
	void uploadUniformFloat2Array(const std::string& name, float* values, uint32_t count) const;
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
	void uploadUniformFloat3Array(const std::string& name, float* values, uint32_t count) const;
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value) const;
	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	std::string readFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
	void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

private:
	std::string name_;
	uint32_t id_;
};
