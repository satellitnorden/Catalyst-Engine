#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Entities.
#include <Entities/CameraEntity.h>

//Math.
#include <Math/Vector3.h>

class SoakCamera final : public CameraEntity
{

public:

	/*
	*	Initializes the soak camera.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the soak camera.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//The rotation timer.
	float rotationTimer{ 0.0f };

	//The rotation amount.
	float rotationAmount{ 0.0f };
};