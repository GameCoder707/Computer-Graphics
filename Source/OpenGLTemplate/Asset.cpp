#include "Asset.h"


Asset::Asset()
{
	assetPos = glm::vec3(1.0f);
	assetRot = glm::mat4(1.0f);
	assetRotDeg = 0.0f;

	canRender = true;

}

Asset::~Asset()
{

}

void Asset::SetPos(glm::vec3 pos)
{
	assetPos = pos;
}

glm::vec3 Asset::GetPos()
{
	return assetPos;
}

void Asset::SetAxis(glm::vec3 axis)
{
	assetAxis = axis;
}

glm::vec3 Asset::GetAxis()
{
	return assetAxis;
}

void Asset::SetRotation(glm::mat4 mat)
{
	assetRot = mat;
}

void Asset::SetRotation(float degrees)
{
	assetRotDeg = degrees;

	// To prevent from continuously
	// increasing or decreasing
	if (assetRotDeg >= 360 || assetRotDeg <= -360)
		assetRotDeg = 0;
}

glm::mat4 Asset::GetRotation()
{
	return assetRot;
}

float Asset::GetRotationDegrees()
{
	return assetRotDeg;
}

void Asset::SetRenderState(bool state)
{
	canRender = state;
}

bool Asset::GetRenderState()
{
	return canRender;
}