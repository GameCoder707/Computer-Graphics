#pragma once

#include "Asset.h"
#include "Gameplay.h"

class CCube;
class CTetrahedron;

class PowerUpAsset : public Asset
{

private:
	// Cube Power Up which offers the player
	// invulnerability
	CCube* cubePowerUp;

	// Tetrahedron Power Up which offers the player
	// to earn double points
	CTetrahedron* tetraPowerUp;

	// The power containing in this asset
	PowerUp containingPower;

public:
	PowerUpAsset(int type);
	~PowerUpAsset();

	void Update();
	void Render();

	PowerUp GetPowerUp();

};
