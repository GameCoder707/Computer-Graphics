#pragma once

#include "Common.h"
#include "Texture.h"

class Asset
{
protected:
	// Position Vector
	glm::vec3 assetPos;

	// Rotation Vector
	glm::mat4 assetRot;

	// Local Axis of the Asset
	glm::vec3 assetAxis;

	// Rotation in Degrees
	float assetRotDeg;

	// If this asset can be rendered or not
	bool canRender;

public:
	Asset();
	~Asset();

	// SET AND GET FUNCTIONS

	void SetPos(glm::vec3 pos);
	glm::vec3 GetPos();

	void SetRotation(glm::mat4 mat);
	void SetRotation(float degrees);

	void SetAxis(glm::vec3 axis);
	glm::vec3 GetAxis();

	glm::mat4 GetRotation();
	float GetRotationDegrees();

	void SetRenderState(bool state);
	bool GetRenderState();

};
