#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class EditorPerceiverSystem final
{

public:

	/*
	*	Updates the editor perceiver system.
	*/
	void Update() NOEXCEPT;

private:

	//The position.
	Vector3<float32> _Position;

	//The rotation.
	EulerAngles _Rotation;

	//The speed.
	float32 _MovementSpeed{ 8.0f };

};

#endif
