#include "Obstacle.h"

Obstacle::Obstacle(int type) : Asset()
{
	// Loading in a texture and setting appropritate
	// damage values depending on the type
	switch (Type(type))
	{
	case Weak:
		damage = 10;
		coverTexture.Load("resources\\textures\\GreenCover.jpg");
		break;
	case Normal:
		damage = 20;
		coverTexture.Load("resources\\textures\\YellowCover.jpg");
		break;
	case Strong:
		damage = 30;
		coverTexture.Load("resources\\textures\\RedCover.jpg");
		break;
		
	}

	// Setting the samplers for the texture
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	coverTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

}

Obstacle::~Obstacle()
{

}

CTexture Obstacle::GetTexture()
{
	return coverTexture;
}

int Obstacle::GetDamage()
{
	return damage;
}