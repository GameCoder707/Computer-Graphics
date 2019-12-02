#pragma once

enum Power
{
	Invulnerability = 0,
	PointsDoubler = 1
};


struct PowerUp
{
	Power power;
	float duration;

};
