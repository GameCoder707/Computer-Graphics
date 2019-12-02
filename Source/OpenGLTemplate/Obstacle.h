#pragma once

#include "Asset.h"

class Obstacle : public Asset
{

	enum Type
	{
		Weak = 0,
		Normal = 1,
		Strong = 2
	};

private:
	// The second texture applied to
	// the original texture
	CTexture coverTexture;

	// The damage caused by this obstacle
	// Varies between types
	int damage;

public:
	Obstacle(int type);
	~Obstacle();

	CTexture GetTexture();

	int GetDamage();


};
