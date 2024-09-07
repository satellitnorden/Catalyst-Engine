//Header file.
#include <Components/Components/ProceduralTreeComponent.h>

//Components.
#include <Components/Components/StaticModelComponent.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Procedural tree class definition.
*/
class ProceduralTree final
{

public:

	//The identifier.
	uint64 _Identifier;

	//The vertices.
	DynamicArray<Vertex> _Vertices;

	//The indices.
	DynamicArray<uint32> _Indices;

};

/*
*	Procedural tree intermediate data class definition.
*/
class ProceduralTreeIntermediateData final
{

public:

	//The random number generator.
	RandomNumberGenerator _RandomNumberGenerator;

};

/*
*	Procedural tree point class definition.
*/
class ProceduralTreePoint final
{

public:

	//The position.
	Vector3<float32> _Position;

	//The direction.
	Vector3<float32> _Direction;

	//The radius.
	float32 _Radius;

};

/*
*	Initializes this component.
*/
void ProceduralTreeComponent::Initialize() NOEXCEPT
{
	//Add editable fields.
	AddEditableUint64Field
	(
		"Seed",
		offsetof(ProceduralTreeInitializationData, _Parameters._Seed),
		offsetof(ProceduralTreeInstanceData, _Parameters._Seed)
	);

	AddEditableFloatField
	(
		"Step Size",
		offsetof(ProceduralTreeInitializationData, _Parameters._StepSize),
		offsetof(ProceduralTreeInstanceData, _Parameters._StepSize)
	);

	AddEditableFloatField
	(
		"Radius Decay",
		offsetof(ProceduralTreeInitializationData, _Parameters._RadiusDecay),
		offsetof(ProceduralTreeInstanceData, _Parameters._RadiusDecay)
	);

	AddEditableFloatField
	(
		"Trunk Radius",
		offsetof(ProceduralTreeInitializationData, _Parameters._TrunkRadius),
		offsetof(ProceduralTreeInstanceData, _Parameters._TrunkRadius)
	);

	AddEditableFloatField
	(
		"Trunk Direction Variation",
		offsetof(ProceduralTreeInitializationData, _Parameters._TrunkDirectionVariation),
		offsetof(ProceduralTreeInstanceData, _Parameters._TrunkDirectionVariation)
	);

	AddEditableUint32Field
	(
		"Number Of Branches",
		offsetof(ProceduralTreeInitializationData, _Parameters._NumberOfBranches),
		offsetof(ProceduralTreeInstanceData, _Parameters._NumberOfBranches)
	);

	AddEditableFloatField
	(
		"Minimum Branching Height",
		offsetof(ProceduralTreeInitializationData, _Parameters._MinimumBranchingHeight),
		offsetof(ProceduralTreeInstanceData, _Parameters._MinimumBranchingHeight)
	);

	AddEditableFloatField
	(
		"Minimum Branching Angle",
		offsetof(ProceduralTreeInitializationData, _Parameters._MinimumBranchingAngle),
		offsetof(ProceduralTreeInstanceData, _Parameters._MinimumBranchingAngle)
	);

	AddEditableFloatField
	(
		"Maximum Branching Angle",
		offsetof(ProceduralTreeInitializationData, _Parameters._MaximumBranchingAngle),
		offsetof(ProceduralTreeInstanceData, _Parameters._MaximumBranchingAngle)
	);

	AddEditableFloatField
	(
		"Branch Direction Variation",
		offsetof(ProceduralTreeInitializationData, _Parameters._BranchDirectionVariation),
		offsetof(ProceduralTreeInstanceData, _Parameters._BranchDirectionVariation)
	);

	AddEditableUint32Field
	(
		"Circle Segments",
		offsetof(ProceduralTreeInitializationData, _Parameters._CircleSegments),
		offsetof(ProceduralTreeInstanceData, _Parameters._CircleSegments)
	);
}

/*
*	Creates an instance.
*/
void ProceduralTreeComponent::CreateInstance(Entity *const RESTRICT entity, ProceduralTreeInitializationData *const RESTRICT initialization_data, ProceduralTreeInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Parameters = initialization_data->_Parameters;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void ProceduralTreeComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(StaticModelComponent::Instance->Has(entity), "Procedural tree component needs a static model component!");

	//Generate. (:
	Generate(entity);
}

/*
*	Callback for after an editable field change happens.
*/
void ProceduralTreeComponent::PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{
	//Generate again. (:
	Generate(entity);
}

/*
*	Generates a procedural tree.
*/
void ProceduralTreeComponent::Generate(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance data.
	ProceduralTreeInstanceData &instance_data{ InstanceData(entity) };

	//Destroy the old model, if there is one.
	for (Mesh &mesh : instance_data._Model._Meshes)
	{
		for (Mesh::MeshLevelOfDetail &level_of_detail : mesh._MeshLevelOfDetails)
		{
			RenderingSystem::Instance->DestroyBuffer(&level_of_detail._VertexBuffer);
			RenderingSystem::Instance->DestroyBuffer(&level_of_detail._IndexBuffer);
		}

		mesh._MeshLevelOfDetails.Clear();
	}

	instance_data._Model._Meshes.Clear();

	//Set up the intermediate data.
	ProceduralTreeIntermediateData intermediate_data;

	intermediate_data._RandomNumberGenerator.SetSeed(instance_data._Parameters._Seed);

	//Generate. (:
	ProceduralTree tree;
	Generate(instance_data._Parameters, &intermediate_data, &tree);

	//Create a fake model asset and set that on the static model
	{
		//Calculate the model space axis aligned bounding box.
		instance_data._Model._ModelSpaceAxisAlignedBoundingBox.Invalidate();

		for (const Vertex &vertex : tree._Vertices)
		{
			instance_data._Model._ModelSpaceAxisAlignedBoundingBox.Expand(vertex._Position);
		}
	}

	{
		//Create the mesh.
		instance_data._Model._Meshes.Emplace();
		Mesh &mesh{ instance_data._Model._Meshes.Back() };

		mesh._MeshLevelOfDetails.Emplace();
		Mesh::MeshLevelOfDetail &level_of_detail{ mesh._MeshLevelOfDetails.Back() };

		level_of_detail._Vertices = tree._Vertices;
		level_of_detail._Indices = tree._Indices;

		//Create the buffers.
		{
			const void *const RESTRICT data_chunks[]{ level_of_detail._Vertices.Data() };
			const uint64 data_sizes[]{ sizeof(Vertex) * level_of_detail._Vertices.Size() };
			RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &level_of_detail._VertexBuffer);
			RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &level_of_detail._VertexBuffer);
		}

		{
			const void *const RESTRICT data_chunks[]{ level_of_detail._Indices.Data() };
			const uint64 data_sizes[]{ sizeof(uint32) * level_of_detail._Indices.Size() };
			RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &level_of_detail._IndexBuffer);
			RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &level_of_detail._IndexBuffer);
		}

		//Write the index count.
		level_of_detail._IndexCount = static_cast<uint32>(level_of_detail._Indices.Size());
	}

	StaticModelComponent::Instance->InstanceData(entity)._Model = AssetPointer<ModelAsset>(&instance_data._Model);
}

/*
*	Destroys an instance.
*/
void ProceduralTreeComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Generates a procedural tree.
*/
void ProceduralTreeComponent::Generate(const ProceduralTreeParameters &parameters, ProceduralTreeIntermediateData *const RESTRICT intermediate_data, ProceduralTree *const RESTRICT tree) NOEXCEPT
{
	//Calculate the identifier.
	const uint64 identifier{ HashAlgorithms::MurmurHash64(&parameters, sizeof(ProceduralTreeParameters)) };

	//Set the identifier.
	tree->_Identifier = identifier;

	//Generate all of the lines.
	DynamicArray<DynamicArray<ProceduralTreePoint>> lines;

	//Grow the trunk.
	lines.Emplace();

	GrowBranch
	(
		parameters,
		intermediate_data,
		&lines.Back(),
		Vector3<float32>(0.0f, 0.0f, 0.0f),
		Vector3<float32>(0.0f, 1.0f, 0.0f),
		parameters._TrunkRadius,
		parameters._TrunkDirectionVariation
	);

	//Grow branches.
	for (uint32 i{ 0 }; i < parameters._NumberOfBranches; ++i)
	{
		Vector3<float32> position;
		Vector3<float32> direction;
		float32 radius;

		ChooseBranchingPoint
		(
			parameters,
			intermediate_data,
			lines,
			&position,
			&direction,
			&radius
		);

		lines.Emplace();

		GrowBranch
		(
			parameters,
			intermediate_data,
			&lines.Back(),
			position,
			direction,
			radius,
			parameters._BranchDirectionVariation
		);
	}


	//Build the procedural tree.
	Build(parameters, lines, tree);
}

/*
*	Grows a branch.
*/
void ProceduralTreeComponent::GrowBranch
(
	const ProceduralTreeParameters &parameters,
	ProceduralTreeIntermediateData *const RESTRICT intermediate_data,
	DynamicArray<ProceduralTreePoint> *const RESTRICT line,
	const Vector3<float32> &position,
	const Vector3<float32> &direction,
	const float32 radius,
	const float32 direction_variation
) NOEXCEPT
{
	//Remember the position.
	Vector3<float32> _position{ position };

	//Remember the direction.
	Vector3<float32> _direction{ direction };

	//Remember the radius.
	float32 _radius{ radius };

	for (;;)
	{
		//Add the new point.
		line->Emplace();
		ProceduralTreePoint &new_point{ line->Back() };

		//Set the position.
		new_point._Position = _position;

		//Set the direction.
		new_point._Direction = _direction;

		//Set the radius.
		new_point._Radius = _radius;

		//Break when the radius becomes too small. (:
		if (new_point._Radius == 0.0f)
		{
			break;
		}

		else
		{
			//Update the position.
			_position += new_point._Direction * parameters._StepSize;

			//Update the direction.
			if (direction_variation > 0.0f)
			{
				_direction = Vector3<float32>::Normalize(_direction + intermediate_data->_RandomNumberGenerator.RandomPointOnSphere() * direction_variation);
			}

			//Update the radius.
			_radius = BaseMath::Maximum<float32>(_radius - parameters._RadiusDecay * parameters._StepSize, 0.0f);
		}
	}
}

/*
*	Chooses a branching point.
*/
void ProceduralTreeComponent::ChooseBranchingPoint
(
	const ProceduralTreeParameters &parameters,
	ProceduralTreeIntermediateData *const RESTRICT intermediate_data,
	const DynamicArray<DynamicArray<ProceduralTreePoint>> &lines,
	Vector3<float32> *const RESTRICT position,
	Vector3<float32> *const RESTRICT direction,
	float32 *const RESTRICT radius
) NOEXCEPT
{
	//Candidate point class definition.
	class CandidatePoint final
	{

	public:

		//The position.
		Vector3<float32> _Position;

		//The direction.
		Vector3<float32> _Direction;

		//The radius.
		float32 _Radius;

	};

	//Generate a list of candidate points.
	DynamicArray<CandidatePoint> candidate_points;

	while (candidate_points.Empty())
	{
		for (const DynamicArray<ProceduralTreePoint> &line : lines)
		{
			for (uint64 point_index{ 1 }; point_index < (line.LastIndex() - 1); ++point_index)
			{
				const ProceduralTreePoint &current_point{ line[point_index] };
				const ProceduralTreePoint &next_point{ line[point_index + 1] };

				const float32 blend_factor{ intermediate_data->_RandomNumberGenerator.RandomFloat<float32>() };

				const Vector3<float32> _position{ BaseMath::LinearlyInterpolate(current_point._Position, next_point._Position, blend_factor) };
				const Vector3<float32> _direction{ Vector3<float32>::Normalize(BaseMath::LinearlyInterpolate(current_point._Direction, next_point._Direction, blend_factor)) };
				const float32 _radius{ BaseMath::LinearlyInterpolate(current_point._Radius, next_point._Radius, blend_factor) };

				//Discard if below the minimum branching height.
				if (_position._Y < parameters._MinimumBranchingHeight)
				{
					continue;
				}

				//Generate the random direction.
				Vector3<float32> random_direction{ intermediate_data->_RandomNumberGenerator.RandomPointOnSphere() };

				if (Vector3<float32>::DotProduct(_direction, random_direction) < 0.0f)
				{
					random_direction = -random_direction;
				}

				//Calculate the angle.
				const float32 angle{ (1.0f - (Vector3<float32>::DotProduct(_direction, random_direction) * 0.5f + 0.5f)) * 360.0f };

				//Discard based on the branching angles.
				if (angle < parameters._MinimumBranchingAngle || angle > parameters._MaximumBranchingAngle)
				{
					continue;
				}

				candidate_points.Emplace();
				CandidatePoint &candidate_point{ candidate_points.Back() };

				candidate_point._Position = _position;
				candidate_point._Direction = random_direction;
				candidate_point._Radius = _radius * 0.8f;
			}
		}
	}

	//Choose the candidate point!
	CandidatePoint chosen_candidate_point{ candidate_points[intermediate_data->_RandomNumberGenerator.RandomIntegerInRange<uint64>(0, candidate_points.LastIndex())] };

	//Fill in the data.
	*position = chosen_candidate_point._Position;
	*direction = chosen_candidate_point._Direction;
	*radius = chosen_candidate_point._Radius;
}

/*
*	Builds a procedural tree from a set of lines.
*/
void ProceduralTreeComponent::Build(const ProceduralTreeParameters &parameters, const DynamicArray<DynamicArray<ProceduralTreePoint>> &lines, ProceduralTree *const RESTRICT tree) NOEXCEPT
{
	//Polgyonize all lines.
	for (const DynamicArray<ProceduralTreePoint> &line : lines)
	{
		//Calculate the cross vector.
		Vector3<float32> cross_vector;

		{
			const Vector3<float32> absolutes
			{
				BaseMath::Absolute(line[0]._Direction._X),
				BaseMath::Absolute(line[0]._Direction._Y),
				BaseMath::Absolute(line[0]._Direction._Z)
			};

			float32 least_value{ absolutes[0] };
			uint8 least_index{ 0 };

			for (uint8 i{ 1 }; i < 3; ++i)
			{
				if (least_value > absolutes[i])
				{
					least_value = absolutes[i];
					least_index = i;
				}
			}

			cross_vector = Vector3<float32>
			(
				static_cast<float32>(least_index == 0),
				static_cast<float32>(least_index == 1),
				static_cast<float32>(least_index == 2)
			);
		}

		//Remember the distance along the line.
		float32 distance_along_line{ 0.0f };

		for (uint64 point_index{ 0 }; point_index < line.Size(); ++point_index)
		{
			//Cache the point.
			const ProceduralTreePoint &point{ line[point_index] };

			//Update the distance along the line.
			if (point_index > 0)
			{
				distance_along_line += Vector3<float32>::Length(line[point_index - 1]._Position - point._Position);
			}

			//Calculate the circumference at this point.
			const float32 circumference_at_point{ 2.0f * BaseMathConstants::PI * point._Radius };

			//Find the tangent.
			const Vector3<float32> tangent
			{
				Vector3<float32>::CrossProduct
				(
					point._Direction,
					cross_vector - point._Direction * Vector3<float32>::DotProduct(cross_vector, point._Direction)
				)
			};

			for (uint32 circle_segment_index{ 0 }; circle_segment_index < parameters._CircleSegments; ++circle_segment_index)
			{
				//Calculate the circle factor.
				const float32 circle_factor{ static_cast<float32>(circle_segment_index) / static_cast<float32>(parameters._CircleSegments - 1) };

				//Add the new vertex.
				tree->_Vertices.Emplace();
				Vertex &new_vertex{ tree->_Vertices.Back() };

				//Calculate the normal.
				new_vertex._Normal = tangent;
				new_vertex._Normal = Quaternion(point._Direction, circle_factor * BaseMathConstants::DOUBLE_PI) * new_vertex._Normal;

				//Calculate the position.
				new_vertex._Position = point._Position + new_vertex._Normal * point._Radius;
			
				//Set the tangent.
				new_vertex._Tangent = point._Direction;

				//Set the texture coordinate.
				new_vertex._TextureCoordinate = Vector2<float32>(circle_factor, distance_along_line / circumference_at_point);

				//Add indices, maybe.
				if (point_index > 0 && circle_segment_index > 0)
				{
					const uint32 vertex_index{ static_cast<uint32>(tree->_Vertices.LastIndex()) };

					tree->_Indices.Emplace(vertex_index - parameters._CircleSegments - 1);
					tree->_Indices.Emplace(vertex_index);
					tree->_Indices.Emplace(vertex_index - 1);

					tree->_Indices.Emplace(vertex_index - parameters._CircleSegments - 1);
					tree->_Indices.Emplace(vertex_index - parameters._CircleSegments);
					tree->_Indices.Emplace(vertex_index);
				}
			}
		}
	}
}