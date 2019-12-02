#pragma once

#include "VertexBufferObject.h"
#include "Texture.h"

class CTetrahedron
{
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	// p for points
	std::vector<glm::vec3> p;

public:
	CTetrahedron();
	~CTetrahedron();
	void Create(string fileName);
	void Render();
	void Release();


};
