#pragma once

#include "VertexBufferObject.h"
#include "Texture.h"

// Class for generating a unit cube
class CCube
{
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	// p for points
	std::vector<glm::vec3> p;

public: CCube();
		~CCube();
		void Create(string fileName);
		void Render();
		void Release();


};
