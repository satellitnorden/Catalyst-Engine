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
	*	Initializes the editor perceiver system.
	*/
	void Initialize() NOEXCEPT;

private:

	//The position.
	Vector3<float32> _Position;

	//The rotation.
	Vector3<float32> _Rotation;

	/*
	*	Updates the editor perceiver system during the LOGIC update phase.
	*/
	void LogicUpdate() NOEXCEPT;

};

#endif
