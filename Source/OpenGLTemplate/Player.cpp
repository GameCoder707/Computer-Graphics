#include "Player.h"

Player::Player() : Asset()
{

	// INITIALISING VALUES

	canCollide = true;
	isInvulnerable = false;

	p_points = 0;
	points_multiplier = 1;

	nonCollisionTimer = 0.0f;
	nonCollisionDuration = 1.0f;

	powerUpDuration = 0.0f;
	powerUpTimer = 0.0f;

	powerUpActive = false;

	dir = 0.0f;
	dest = 0.0f;
	currX = 0.0f;
	currY = 1.0f;
	moveSpeed = 0.06f;
	movePlayer = false;

	jumpHeight = 8.0f;
	jumpSpeed = 0.025f;
	isJumping = false;
	isFalling = false;

}

Player::~Player()
{

}

// Update Method
void Player::Update(glm::vec3 point, glm::vec3 T, glm::vec3 N, glm::vec3 B, float trackHalfWidth, float offset, float dt)
{

	// If the player presses A or D and if it's
	// not moving, then move the player in the
	// respective direction
	if ((GetKeyState('A') & 0x80) && movePlayer == false)
	{
		if (dest > -trackHalfWidth + offset)
		{
			dest += (-trackHalfWidth + offset);
			dir = -1;
			movePlayer = true;
		}
	}

	if ((GetKeyState('D') & 0x80) && movePlayer == false)
	{
		if (dest < trackHalfWidth - offset)
		{
			dest += (trackHalfWidth - offset);
			dir = 1;
			movePlayer = true;
		}
	}

	// Player presses W and if it's not already jumping or
	// if it's falling, then make the player jump
	if (GetKeyState('W') & 0x80 || GetKeyState(VK_SPACE) & 0x80)
	{
		if (isJumping == false && isFalling == false)
		{
			isJumping = true;
		}
	}

	// Set the rotation using T, B and N vectors
	SetRotation(glm::mat4(glm::mat3(T, B, N)));

	// Set the player's position with the point
	SetPos(point);

	// If the player is made to move
	if (movePlayer == true)
	{
		// This checks if we're close enough
		// and sets the current position to the
		// destination
		if (glm::abs(currX - dest) <= 2.f)
		{
			currX = dest;
			movePlayer = false;
		}
		else
		{
			// If we haven't reached the destination
		// then, move the player in relative X axis
			currX += (dir * moveSpeed * dt);
		}

	}

	// If the player decided to jump
	if (isJumping == true)
	{
		// If we reached the jump height, then start falling
		if (currY >= jumpHeight)
		{
			isFalling = true;
			isJumping = false;
		}
		else
		{
			currY += (jumpSpeed * dt);

		}

	}
	else if (isFalling == true)
	{
		// If we reached the ground, then stop falling
		if (currY <= 1.0f)
		{
			currY = 1.0f;
			isFalling = false;
		}
		else
		{
			currY -= (jumpSpeed * dt);
		}
	}

	// Set the position of the player accordingly
	SetPos(GetPos() + (currX * N));
	SetPos(GetPos() + (currY * B));

	// If the player is in non collidable state,
	// then update the timer to revert back
	if (canCollide == false)
	{
		if (nonCollisionTimer >= nonCollisionDuration)
		{
			canCollide = true;
			nonCollisionTimer = 0.0f;
		}
		else
		{
			nonCollisionTimer += 0.01f;
		}
	}

}

// Update the timer of the active power up
void Player::PowerUpUpdate()
{
	if (powerUpActive == true)
	{
		switch (currentPower)
		{
		case Invulnerability:

			powerUpTimer += 0.015f;

			if (powerUpTimer >= powerUpDuration)
			{
				isInvulnerable = false;
				powerUpActive = false;
				powerUpTimer = 0.0f;
			}

			break;
		case PointsDoubler:

			powerUpTimer += 0.015f;

			if (powerUpTimer >= powerUpDuration)
			{
				points_multiplier = 1;
				powerUpActive = false;
				powerUpTimer = 0.0f;
			}

			break;
		}
	}
}

// Set the collided power up to current power up
void Player::SetPowerUpInfo(PowerUp collidedPowerUp)
{

	switch (collidedPowerUp.power)
	{
		// Set the values from the parameter
		// and do the necessary actions
	case Invulnerability:
		isInvulnerable = true;
		currentPower = collidedPowerUp.power;
		powerUpDuration = collidedPowerUp.duration;
		break;
	case PointsDoubler:
		points_multiplier = 2;
		currentPower = collidedPowerUp.power;
		powerUpDuration = collidedPowerUp.duration;
		break;

	}

	powerUpActive = true;

}

// SET AND GET FUNCTIONS

void Player::SetCollidableState(bool state)
{
	canCollide = state;
}

bool Player::GetCollidableState()
{
	return canCollide;
}

bool Player::GetPowerActiveState()
{
	return powerUpActive;
}

bool Player::GetInvulnerabilityState()
{
	return isInvulnerable;
}

void Player::SetPoints(int points)
{
	p_points = points;
}

int Player::GetPoints()
{
	return p_points;
}

int Player::GetPointsMultiplier()
{
	return points_multiplier;
}