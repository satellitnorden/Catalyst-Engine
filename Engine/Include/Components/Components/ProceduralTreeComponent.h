#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Assets/ModelAsset.h>

//Forward declarations.
class ProceduralTree;
class ProceduralTreeIntermediateData;
class ProceduralTreePoint;

/*
*	Procedural tree parameters class definition.
*/
class ProceduralTreeParameters final
{

public:

	//The seed.
	uint64 _Seed{ 0 };

	//The step size.
	float32 _StepSize{ 0.5f };

	//The radius decay.
	float32 _RadiusDecay{ 0.05f };

	//The trunk radius.
	float32 _TrunkRadius{ 2.0f };

	//The trunk direction variation.
	float32 _TrunkDirectionVariation{ 0.025f };

	//The number of branches.
	uint32 _NumberOfBranches{ 8 };

	//The minimum branching height.
	float32 _MinimumBranchingHeight{ 8.0f };

	//The minimum branching angle.
	float32 _MinimumBranchingAngle{ 22.5f };

	//The maximum branching angle.
	float32 _MaximumBranchingAngle{ 67.5f };

	//The branch direction variation.
	float32 _BranchDirectionVariation{ 0.05f };

	//The number of circle segments.
	uint32 _CircleSegments{ 8 };

};

class ProceduralTreeInitializationData final : public ComponentInitializationData
{

public:

	//The parameters.
	ProceduralTreeParameters _Parameters;

};

class ProceduralTreeInstanceData final
{

public:

	//The parameters.
	ProceduralTreeParameters _Parameters;

	//The model.
	ModelAsset _Model;

};

class ProceduralTreeComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		ProceduralTree,
		COMPONENT_INITIALIZE()
	);

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

	/*
	*	Callback for after an editable field change happens.
	*/
	void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

private:

	/*
	*	Generates a procedural tree.
	*/
	void Generate(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Generates a procedural tree.
	*/
	void Generate(const ProceduralTreeParameters &parameters, ProceduralTreeIntermediateData *const RESTRICT intermediate_data, ProceduralTree *const RESTRICT tree) NOEXCEPT;

	/*
	*	Grows a branch.
	*/
	void GrowBranch
	(
		const ProceduralTreeParameters &parameters,
		ProceduralTreeIntermediateData *const RESTRICT intermediate_data,
		DynamicArray<ProceduralTreePoint> *const RESTRICT line,
		const Vector3<float32> &position,
		const Vector3<float32> &direction,
		const float32 radius,
		const float32 direction_variation
	) NOEXCEPT;

	/*
	*	Chooses a branching point.
	*/
	void ChooseBranchingPoint
	(
		const ProceduralTreeParameters &parameters,
		ProceduralTreeIntermediateData *const RESTRICT intermediate_data,
		const DynamicArray<DynamicArray<ProceduralTreePoint>> &lines,
		Vector3<float32> *const RESTRICT position,
		Vector3<float32> *const RESTRICT direction,
		float32 *const RESTRICT radius
	) NOEXCEPT;

	/*
	*	Builds a procedural tree from a set of lines.
	*/
	void Build(const ProceduralTreeParameters &parameters, const DynamicArray<DynamicArray<ProceduralTreePoint>> &lines, ProceduralTree *const RESTRICT tree) NOEXCEPT;

};