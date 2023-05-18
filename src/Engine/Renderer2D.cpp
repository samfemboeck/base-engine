#include "../pch.h"
#include "Renderer2D.h"
#include "Time.h"

static Renderer2DStorage s_data;

static std::map<char, Character> s_characters;

void Renderer2D::init()
{
	s_data.quadVertexArray = new VertexArray;

	s_data.quadVertexBuffer = new VertexBuffer(s_data.MaxVertices * sizeof(Vertex));
	s_data.quadVertexBuffer->setLayout({
		{ ShaderDataType::Float3, "bPosition" },
		{ ShaderDataType::Float4, "bColor" },
		{ ShaderDataType::Float2, "bTexCoord" },
		{ ShaderDataType::Float, "bTexIndex" }
		});

	s_data.quadVertexArray->addVertexBuffer(s_data.quadVertexBuffer);

	s_data.quadVertexBufferBase = new Vertex[s_data.MaxVertices];
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

	uint32_t* quadIndices = new uint32_t[s_data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_data.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	IndexBuffer* quadIB = new IndexBuffer(quadIndices, s_data.MaxIndices);
	s_data.quadVertexArray->setIndexBuffer(quadIB);
	delete[] quadIndices;

	s_data.textureShader = new Shader("Shaders/texture.glsl");
	s_data.textureShader->bind();

	int32_t samplers[s_data.MaxTextureSlots];
	for (uint32_t i = 0; i < s_data.MaxTextureSlots; i++)
		samplers[i] = i;

	s_data.textureShader->uploadUniformIntArray("uTextures", samplers, s_data.MaxTextureSlots);
	glm::vec2 lightPositions[] = { {0.0f, 0.0f} };
	s_data.textureShader->uploadUniformFloat2Array("uLightPositions", &lightPositions->x, 1);
	s_data.textureShader->uploadUniformInt("uNumLights", 1);

	s_data.flatColorShader = new Shader("Shaders/flat_color.glsl");

	int32_t channels[2];
	channels[0] = 0;
	channels[1] = 1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Text Rendering

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, "../Fonts/AmaticSC-Regular.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		s_characters.insert(std::pair<char, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &s_data.VAOText);
	glGenBuffers(1, &s_data.VBOText);
	glBindVertexArray(s_data.VAOText);
	glBindBuffer(GL_ARRAY_BUFFER, s_data.VBOText);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	s_data.textShader = new Shader("Shaders/text.glsl");
}

void Renderer2D::shutdown()
{
}

void Renderer2D::setClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer2D::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D::beginTextures(const glm::mat4& view, const glm::mat4& projection)
{
	s_data.quadVertexArray->bind();
	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.textureShader->bind();
	s_data.textureShader->uploadUniformMat4("uView", view);
	s_data.textureShader->uploadUniformMat4("uProjection", projection);
	s_data.textureShader->uploadUniformFloat3("uLightAttenuation", Renderer2DStorage::LightAttenuation);
}

void Renderer2D::beginFlatColor(const glm::mat4& viewProjection)
{
	s_data.quadVertexArray->bind();
	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.flatColorShader->bind();
	s_data.flatColorShader->uploadUniformMat4("uViewProjection", viewProjection);
}

void Renderer2D::beginLines(const glm::mat4& view, const glm::mat4& projection)
{
	s_data.quadVertexArray->bind();
	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.flatColorShader->bind();
	s_data.flatColorShader->uploadUniformMat4("uViewProjection", projection * view);
}

void Renderer2D::endLines()
{
	uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
	s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);
	glDrawArrays(GL_LINES, 0, s_data.quadIndexCount);
}

void Renderer2D::endTextures()
{
	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
		s_data.textureSlots[i]->bind(i);

	uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
	s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);
	glDrawElements(GL_TRIANGLES, s_data.quadIndexCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::endFlatColor()
{
	uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
	s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);
	glDrawElements(GL_TRIANGLES, s_data.quadIndexCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer2D::pushQuad(const glm::mat4& transform, Texture2D* texture, const glm::vec4& color, bool includeTextureSize)
{
	if (s_data.quadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
	{
		if (s_data.textureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.textureSlotIndex;
		s_data.textureSlots[s_data.textureSlotIndex] = texture;
		s_data.textureSlotIndex++;
	}

	auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ texture->getWidth(), texture->getHeight(), 1 })) : transform;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[0] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 0.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[1] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 0.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[2] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 1.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[3] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 1.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadIndexCount += 6;
}

void Renderer2D::pushQuad(const glm::mat4& transform, SubTexture2D* subTexture, const glm::vec4& color, bool includeTextureSize)
{
	if (s_data.quadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	auto& texture = subTexture->getTexture();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
	{
		if (*s_data.textureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.textureSlotIndex;
		s_data.textureSlots[s_data.textureSlotIndex] = &texture;
		s_data.textureSlotIndex++;
	}

	auto spriteSize = subTexture->getSize();
	auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ spriteSize.x, spriteSize.y, 1 })) : transform;
	auto texCoords = subTexture->getTexCoords();

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[0] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = texCoords[0];
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;
	//

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[1] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = texCoords[1];
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[2] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = texCoords[2];
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = { tf * s_data.unitQuad[3] };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = texCoords[3];
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadIndexCount += 6;
}

void Renderer2D::pushQuad(const glm::vec2& min, const glm::vec2& max, Texture2D* texture, const glm::vec4& color)
{
	if (s_data.quadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
	{
		if (s_data.textureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.textureSlotIndex;
		s_data.textureSlots[s_data.textureSlotIndex] = texture;
		s_data.textureSlotIndex++;
	}

	s_data.quadVertexBufferPtr->position = glm::vec3{ min.x, min.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 0.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ max.x, min.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 0.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ max.x, max.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 1.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ min.x, max.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 1.0f };
	s_data.quadVertexBufferPtr->texIndex = textureIndex;
	s_data.quadVertexBufferPtr++;

	s_data.quadIndexCount += 6;
}

void Renderer2D::pushQuad(const glm::vec2& min, const glm::vec2& max, const glm::vec4& color)
{
	s_data.quadVertexBufferPtr->position = glm::vec3{ min.x, min.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 0.0f };
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ max.x, min.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 0.0f };
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ max.x, max.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 1.0f, 1.0f };
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ min.x, max.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr->texCoord = glm::vec2{ 0.0f, 1.0f };
	s_data.quadVertexBufferPtr++;

	s_data.quadIndexCount += 6;
}

void Renderer2D::pushLine(const glm::vec2& a, const glm::vec2& b, const glm::vec4& color)
{
	s_data.quadVertexBufferPtr->position = glm::vec3{ a.x, a.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr++;

	s_data.quadVertexBufferPtr->position = glm::vec3{ b.x, b.y, 0 };
	s_data.quadVertexBufferPtr->color = color;
	s_data.quadVertexBufferPtr++;

	s_data.quadIndexCount += 2;
}

void Renderer2D::renderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	s_data.textShader->bind();
	s_data.textShader->uploadUniformFloat3("textColor", color);
	s_data.textShader->uploadUniformMat4("projection", glm::ortho(0.0f, 1920.0f, 0.0f, 1017.0f));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(s_data.VAOText);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = s_characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, s_data.VBOText);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer2D::nextBatch()
{
	endTextures();
	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.textureSlotIndex = 0;
}