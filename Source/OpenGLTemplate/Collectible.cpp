#include "Collectible.h"

Collectible::Collectible(int type) : Asset()
{
	// Loading in the texture and setting appropriate
	// point value depending on the type
	switch (Type(type))
	{
	case Weak: 
		points = 10;
		coverTexture.Load("resources\\textures\\RedCover.jpg", true);
		break;
	case Normal:
		points = 20;
		coverTexture.Load("resources\\textures\\YellowCover.jpg", true);
		break;
	case Strong:
		points = 30;
		coverTexture.Load("resources\\textures\\GreenCover.jpg", true);
		break;
	}

	// Setting the samplers for the texture
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	
}

Collectible::~Collectible()
{

}

CTexture Collectible::GetTexture()
{
	return coverTexture;
}

int Collectible::GetPoints()
{
	return points;
}