#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"
#include "Cubemap.h"

// This is a class for creating and rendering a skybox
class CSkybox
{

	enum TimeOfDay
	{
		Day = 0,
		Night = 1
	};

public:
	CSkybox();
	~CSkybox();
	void Create(float size, int timeOfDay);
	void Render(int textureUnit);
	void Release();

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CCubemap m_cubemapTexture;
	
};