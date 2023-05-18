#pragma once

#include "Buffer.h"

class VertexArray
{
public:
	VertexArray();
	virtual ~VertexArray();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void addVertexBuffer(VertexBuffer* vertexBuffer);
	virtual void setIndexBuffer(IndexBuffer* indexBuffer);

	virtual const std::vector<VertexBuffer*>& getVertexBuffers() const { return vertexBuffers_; }
	virtual const IndexBuffer* getIndexBuffer() const { return indexBuffer_; }

private:
	uint32_t id_;
	std::vector<VertexBuffer*> vertexBuffers_;
	IndexBuffer* indexBuffer_;
};
