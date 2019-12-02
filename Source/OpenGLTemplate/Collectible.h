#pragma once

#include "Asset.h"

class Collectible : public Asset
{
	enum Type
	{
		Weak = 0,
		Normal = 1,
		Strong = 2
	};

private:
	// Second texture to be applied
	// to the original texture
	CTexture coverTexture;

	// Points awarded to the player
	// Varies between types
	int points;

public:
	
	Collectible(int type);
	~Collectible();

	CTexture GetTexture();

	int GetPoints();

};
