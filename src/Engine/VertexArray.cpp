#include "../pch.h"
#include "VertexArray.h"
#include <glad/glad.h>

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return GL_FLOAT;
	case ShaderDataType::Float2:   return GL_FLOAT;
	case ShaderDataType::Float3:   return GL_FLOAT;
	case ShaderDataType::Float4:   return GL_FLOAT;
	case ShaderDataType::Mat3:     return GL_FLOAT;
	case ShaderDataType::Mat4:     return GL_FLOAT;
	case ShaderDataType::Int:      return GL_INT;
	case ShaderDataType::Int2:     return GL_INT;
	case ShaderDataType::Int3:     return GL_INT;
	case ShaderDataType::Int4:     return GL_INT;
	case ShaderDataType::Bool:     return GL_BOOL;
	}

	return 0;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &id_);
}

void VertexArray::bind() const
{
	glBindVertexArray(id_);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(VertexBuffer* vertexBuffer)
{
	glBindVertexArray(id_);
	vertexBuffer->bind();

	uint32_t index = 0;
	const auto& layout = vertexBuffer->getLayout();
	for (const auto& element : layout)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index,
			element.GetComponentCount(),
			ShaderDataTypeToOpenGLBaseType(element.type),
			element.normalized ? GL_TRUE : GL_FALSE,
			layout.GetStride(),
			(const void*)element.offset);
		index++;
	}

	vertexBuffers_.push_back(vertexBuffer);
}

void VertexArray::setIndexBuffer(IndexBuffer* indexBuffer)
{
	glBindVertexArray(id_);
	indexBuffer->bind();

	indexBuffer_ = indexBuffer;
}