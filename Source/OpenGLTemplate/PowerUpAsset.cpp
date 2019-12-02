#include "PowerUpAsset.h"
#include "Cube.h"
#include "Tetrahedron.h"

PowerUpAsset::PowerUpAsset(int type) : Asset()
{
	cubePowerUp = NULL;
	tetraPowerUp = NULL;

	// We create the shape based on the type of power up
	// And set the containing power attribute with appropriate values;
	switch (Power(type))
	{
	case Invulnerability:
		cubePowerUp = new CCube;

		cubePowerUp->Create("resources\\textures\\MetalWithShield.jpg");

		containingPower.power = Invulnerability;
		containingPower.duration = 3.0f;

		break;

	case PointsDoubler:
		tetraPowerUp = new CTetrahedron;

		tetraPowerUp->Create("resources\\textures\\PointsDoublerSoftFabric.jpg");

		containingPower.power = PointsDoubler;
		containingPower.duration = 4.0f;

		break;

	}

	canRender = true;

}

PowerUpAsset::~PowerUpAsset()
{

}

PowerUp PowerUpAsset::GetPowerUp()
{
	return containingPower;
}

// Updating the rotation values
void PowerUpAsset::Update()
{
	float deg = 0.1f;
	SetRotation(GetRotationDegrees() - deg);

}

void PowerUpAsset::Render()
{
	// Render the appropriate shape
	if (cubePowerUp != NULL)
	{
		cubePowerUp->Render();
	}
	else if (tetraPowerUp != NULL)
	{
		tetraPowerUp->Render();
	}

}