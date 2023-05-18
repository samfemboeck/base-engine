#pragma once

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat3:     return 4 * 3 * 3;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Int:      return 4;
	case ShaderDataType::Int2:     return 4 * 2;
	case ShaderDataType::Int3:     return 4 * 3;
	case ShaderDataType::Int4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	return 0;
}

struct BufferElement
{
	std::string name;
	ShaderDataType type;
	uint32_t size;
	uint32_t offset;
	bool normalized;

	BufferElement() :
		name("default"),
		size(0),
		offset(0),
		normalized(false)
	{}

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
	{
	}

	uint32_t GetComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3 * 3;
		case ShaderDataType::Mat4:    return 4 * 4;
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 1;
		}

		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: elements_(elements)
	{
		CalculateOffsetsAndStride();
	}

	inline uint32_t GetStride() const { return stride_; }
	inline const std::vector<BufferElement>& GetElements() const { return elements_; }

	std::vector<BufferElement>::iterator begin() { return elements_.begin(); }
	std::vector<BufferElement>::iterator end() { return elements_.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return elements_.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return elements_.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		stride_ = 0;
		for (auto& element : elements_)
		{
			element.offset = offset;
			offset += element.size;
			stride_ += element.size;
		}
	}
private:
	std::vector<BufferElement> elements_;
	uint32_t stride_ = 0;
};

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size);
	VertexBuffer(const void* vertices, uint32_t size);
	virtual ~VertexBuffer();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void setData(const void* data, uint32_t size);

	virtual const BufferLayout& getLayout() const { return layout_; }
	virtual void setLayout(const BufferLayout& layout) { layout_ = layout; }

private:
	uint32_t id_;
	BufferLayout layout_;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~IndexBuffer();

	virtual void bind() const;
	virtual void unbind() const;

	virtual uint32_t getCount() const { return count_; }
private:
	uint32_t id_;
	uint32_t count_;
};
