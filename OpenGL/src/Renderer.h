#pragma once
#include <glad/gl.h>

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer 
{
public:
	void Clear() const;
	void Draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
};