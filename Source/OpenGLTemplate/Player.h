#pragma once

#include "Asset.h"
#include "Gameplay.h"

class Player : public Asset
{

private:

	// Integer to store player points
	int p_points;

	// A points multiplier for a power up
	int points_multiplier;

	// Boolean to check if the player
	// can be collided with an obstacle
	bool canCollide;

	// Is the player invulnerable 
	// because of a power up
	bool isInvulnerable;

	// A timer and duration
	// to avoid collision
	float nonCollisionTimer;
	float nonCollisionDuration;

	// A timer and duration
	// to keep the power up active
	float powerUpTimer;
	float powerUpDuration;

	// To check if the player
	// is under a powerUp
	bool powerUpActive;

	// The current Power Up in use
	Power currentPower;

	// SIDEWAYS MOVEMENT
	// Direction to move
	float dir;

	// The destination points
	float dest;

	// Current Distance travalled
	// in local X axis
	float currX;

	// Current distance travelled
	// in local Y axis
	float currY;

	// Movement speed of the car
	float moveSpeed;

	// To check if the player needs to be moved
	bool movePlayer;

	// JUMP VALUES

	// Height to jump to
	float jumpHeight;

	// Speed of the jump and fall
	float jumpSpeed;

	// To check if the player
	// is jumping and falling
	bool isJumping;
	bool isFalling;

public:

	// Constructor and Destructor
	Player();
	~Player();

	// Set and Get of the collidable boolean
	void SetCollidableState(bool state);
	bool GetCollidableState();

	// Update Player's Transformations
	void Update(glm::vec3 point, glm::vec3 T, glm::vec3 N, glm::vec3 B,
		float trackHalfWidth, float offset, float dt);

	// Update the power up timer
	void PowerUpUpdate();

	// Set and Get of Points value
	void SetPoints(int points);
	int GetPoints();

	// Set and Get the Points multiplier
	int GetPointsMultiplier();

	// Set the collided power as the power up in use
	void SetPowerUpInfo(PowerUp collidedPowerUp);

	// Get the boolean of the power up active state
	bool GetPowerActiveState();

	// Get the state of invulnerability
	bool GetInvulnerabilityState();

};
