#include "Tetrahedron.h"

CTetrahedron::CTetrahedron()
{

}

CTetrahedron::~CTetrahedron()
{
	Release();
}

void CTetrahedron::Create(string fileName)
{
	m_texture.Load(fileName, true);


	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();

	vector<glm::vec2> s;
	vector<glm::vec3> n;

	// TEX COORDS

	s.push_back(glm::vec2(0.0f, 0.0f));
	s.push_back(glm::vec2(1.0f, 0.0f));
	s.push_back(glm::vec2(0.5f, 1.0f));

	// FACE 1

	p.push_back(glm::vec3(-1, 0, 1));
	p.push_back(glm::vec3(1, 0, 1));
	p.push_back(glm::vec3(0, 2, 0));

	n.push_back(glm::vec3(0, 0, 1));

	// FACE 2

	p.push_back(glm::vec3(1, 0, 1));
	p.push_back(glm::vec3(0, 0, -1));
	p.push_back(glm::vec3(0, 2, 0));

	n.push_back(glm::vec3(1, 0, -1));

	// FACE 3

	p.push_back(glm::vec3(0, 0, -1));
	p.push_back(glm::vec3(-1, 0, 1));
	p.push_back(glm::vec3(0, 2, 0));

	n.push_back(glm::vec3(-1, 0, -1));

	// BASE

	p.push_back(glm::vec3(-1, 0, 1));
	p.push_back(glm::vec3(1, 0, 1));
	p.push_back(glm::vec3(0, 0, -1));
	

	n.push_back(glm::vec3(0, -1, 0));

	for (int i = 0, j = 0; i < p.size(); i++)
	{
		if (i % 3 == 0 && i != 0)
			j++;

		m_vbo.AddData(&p[i], sizeof(glm::vec3));
		m_vbo.AddData(&s[i % s.size()], sizeof(glm::vec2));
		m_vbo.AddData(&n[j], sizeof(glm::vec3));
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CTetrahedron::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, p.size());

}

void CTetrahedron::Release()
{
	m_texture.Release();

	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();

}