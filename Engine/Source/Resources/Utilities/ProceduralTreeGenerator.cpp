//Header file.
#include <Resources/Utilities/ProceduralTreeGenerator.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

/*
*	Generates a tree with the given parameters.
*	Outputs vertices and indices.
*/
void ProceduralTreeGenerator::GenerateTree(const Parameters &parameters, Output *const RESTRICT output) NOEXCEPT
{
	/*
	*	Line segment point class definition.
	*/
	class LineSegmentPoint final
	{

	public:

		//The position.
		Vector3<float32> _Position;

		//The normalized height.
		float32 _NormalizedHeight;

		//The rotation.
		EulerAngles _Rotation;

	};

	/*
	*	Line segment class definition.
	*/
	class LineSegment final
	{

	public:

		//The parent line segment index.
		uint64 _ParentLineSegmentIndex;

		//The radius.
		float32 _Radius;

		//The points.
		DynamicArray<LineSegmentPoint> _Points;

	};

	//Define some constants.
	constexpr float32 RANDOM_BASE_TRUNK_ROTATION_RANGE{ CatalystBaseMathConstants::PI * 0.125f * 0.125f };
	constexpr float32 RANDOM_CONTINUOUS_RANGE{ CatalystBaseMathConstants::PI * 0.125f * 0.25f };
	constexpr float32 RANDOM_BRANCHING_RANGE{ CatalystBaseMathConstants::PI * 0.125f };
	constexpr float32 MINIMUM_BRANCHING_DIVERGENCE{ CatalystBaseMathConstants::PI * 0.125f * 0.125f };
	constexpr float32 BASE_RADIUS{ 0.5f };
	constexpr float32 RADIUS_DECAY_MULTIPLIER{ 0.9f };
	constexpr float32 HEIGHT{ 24.0f };
	constexpr uint32 NUMBER_OF_POINTS{ 32 };
	constexpr uint32 NUMBER_OF_CIRCLE_SEGMENTS{ 8 };
	constexpr uint32 NUMBER_OF_COLLISION_CIRCLE_SEGMENTS{ NUMBER_OF_CIRCLE_SEGMENTS / 2 };
	constexpr uint8 BRANCHING_PASSES{ 12 };
	constexpr float32 MINIMUM_BRANCHING_HEIGHT{ 0.25f };
	constexpr float32 MINIMUM_CROWN_HEIGHT{ 0.375f };

	/*
	*	Create line segments.
	*	One array for each "branch", with position of each point, and the normalized height in the _W component.
	*/
	DynamicArray<LineSegment> line_segments;

	//Add the base trunk.
	line_segments.Emplace();

	//Add the root line segment point.
	line_segments.Back()._ParentLineSegmentIndex = UINT64_MAXIMUM;
	line_segments.Back()._Radius = BASE_RADIUS;

	line_segments.Back()._Points.Emplace();
	LineSegmentPoint &new_line_segment_point{ line_segments.Back()._Points.Back() };

	new_line_segment_point._Position = Vector3<float32>(0.0f, 0.0f, 0.0f);
	new_line_segment_point._NormalizedHeight = 0.0f;
	new_line_segment_point._Rotation = EulerAngles(CatalystRandomMath::RandomFloatInRange(-RANDOM_BASE_TRUNK_ROTATION_RANGE, RANDOM_BASE_TRUNK_ROTATION_RANGE), 0.0f, CatalystRandomMath::RandomFloatInRange(-RANDOM_BASE_TRUNK_ROTATION_RANGE, RANDOM_BASE_TRUNK_ROTATION_RANGE));

	//Grow the root trunk.
	for (uint32 i{ 1 }; i < NUMBER_OF_POINTS; ++i)
	{
		//Cache the previous line segment point.
		const LineSegmentPoint previous_line_segment_point{ line_segments.Back()._Points.Back() };

		//Calculate the normalized height.
		const float32 normalized_height{ static_cast<float32>(i) / static_cast<float32>(NUMBER_OF_POINTS - 1) };

		//Calculate the previous direction.
		Vector3<float32> previous_direction{ 0.0f, 1.0f, 0.0f };
		previous_direction.Rotate(previous_line_segment_point._Rotation);

		//Add the new point.
		line_segments.Back()._Points.Emplace();
		LineSegmentPoint &new_line_segment_point{ line_segments.Back()._Points.Back() };

		new_line_segment_point._Position = previous_line_segment_point._Position + previous_direction * (HEIGHT / static_cast<float32>(NUMBER_OF_POINTS));
		new_line_segment_point._NormalizedHeight = normalized_height;
		new_line_segment_point._Rotation = EulerAngles(previous_line_segment_point._Rotation._Roll + CatalystRandomMath::RandomFloatInRange(-RANDOM_BASE_TRUNK_ROTATION_RANGE, RANDOM_BASE_TRUNK_ROTATION_RANGE), 0.0f, previous_line_segment_point._Rotation._Pitch + CatalystRandomMath::RandomFloatInRange(-RANDOM_BASE_TRUNK_ROTATION_RANGE, RANDOM_BASE_TRUNK_ROTATION_RANGE));
	}

	//Grow new branches.
	for (uint8 branching_index{ 0 }; branching_index < BRANCHING_PASSES; ++branching_index)
	{
		//Iterate over all line segments and branch them!
		const uint64 line_segments_size{ line_segments.Size() };

		for (uint64 line_segment_index{ 0 }; line_segment_index < line_segments_size; ++line_segment_index)
		{
			//Cache the line segment.
			LineSegment &line_segment{ line_segments[line_segment_index] };

			//Randomize at which height to branch this off.
			const float32 branch_height{ CatalystBaseMath::LinearlyInterpolate(MINIMUM_BRANCHING_HEIGHT, 1.0f, CatalystRandomMath::RandomFloat()) };

			if (branch_height < line_segment._Points[0]._NormalizedHeight)
			{
				continue;
			}

			const uint64 line_segment_end{ line_segment._Points.LastIndex() };

			for (uint64 line_segment_point_index{ 1 }; line_segment_point_index < line_segment_end; ++line_segment_point_index)
			{
				LineSegmentPoint &line_segment_point{ line_segment._Points[line_segment_point_index] };

				if (line_segment_point._NormalizedHeight >= branch_height)
				{
					line_segments.Emplace();
					LineSegment &new_line_segment{ line_segments.Back() };

					new_line_segment._ParentLineSegmentIndex = line_segment_index;
					new_line_segment._Radius = line_segment._Radius * RADIUS_DECAY_MULTIPLIER;

					//Add the root line segment point.
					new_line_segment._Points.Emplace();
					LineSegmentPoint &new_line_segment_point{ new_line_segment._Points.Back() };

					new_line_segment_point._Position = line_segment_point._Position;
					new_line_segment_point._NormalizedHeight = line_segment_point._NormalizedHeight;
					new_line_segment_point._Rotation = EulerAngles(CatalystRandomMath::RandomFloatInRange(-RANDOM_BRANCHING_RANGE, RANDOM_BRANCHING_RANGE), 0.0f, CatalystRandomMath::RandomFloatInRange(-RANDOM_BRANCHING_RANGE, RANDOM_BRANCHING_RANGE) * 2.0f);

					//Enforce a minimum rotation divergence.
					const float32 roll_difference{ CatalystBaseMath::Absolute(new_line_segment_point._Rotation._Roll - line_segment_point._Rotation._Roll) };
					const float32 roll_divergence_fixer{ CatalystBaseMath::Maximum(MINIMUM_BRANCHING_DIVERGENCE - roll_difference, 0.0f) };
					new_line_segment_point._Rotation._Roll += roll_divergence_fixer * (new_line_segment_point._Rotation._Roll >= line_segment_point._Rotation._Roll ? -1.0f : 1.0f);

					const float32 pitch_difference{ CatalystBaseMath::Absolute(new_line_segment_point._Rotation._Pitch - line_segment_point._Rotation._Pitch) };
					const float32 pitch_divergence_fixer{ CatalystBaseMath::Maximum(MINIMUM_BRANCHING_DIVERGENCE - pitch_difference, 0.0f) };
					new_line_segment_point._Rotation._Pitch += pitch_divergence_fixer * (new_line_segment_point._Rotation._Pitch >= line_segment_point._Rotation._Pitch ? -1.0f : 1.0f);

					//Calculate the start index.
					const uint64 start_index{ NUMBER_OF_POINTS - line_segment_end + 1 + line_segment_point_index + 1 };

					//Grow the rest of the branch.
					for (uint64 i{ start_index }; i < NUMBER_OF_POINTS; ++i)
					{
						//Cache the previous line segment point.
						const LineSegmentPoint previous_line_segment_point{ new_line_segment._Points.Back() };

						//Calculate the normalized height.
						const float32 normalized_height{ static_cast<float32>(i) / static_cast<float32>(NUMBER_OF_POINTS - 1) };

						//Calculate the previous direction.
						Vector3<float32> previous_direction{ 0.0f, 1.0f, 0.0f };
						previous_direction.Rotate(previous_line_segment_point._Rotation);

						//Add the new point.
						new_line_segment._Points.Emplace();
						LineSegmentPoint &branched_line_segment_point{ new_line_segment._Points.Back() };

						branched_line_segment_point._Position = previous_line_segment_point._Position + previous_direction * (HEIGHT / static_cast<float32>(NUMBER_OF_POINTS));
						branched_line_segment_point._NormalizedHeight = normalized_height;
						branched_line_segment_point._Rotation = EulerAngles(previous_line_segment_point._Rotation._Roll + CatalystRandomMath::RandomFloatInRange(-RANDOM_CONTINUOUS_RANGE, RANDOM_CONTINUOUS_RANGE), 0.0f, previous_line_segment_point._Rotation._Pitch + CatalystRandomMath::RandomFloatInRange(-RANDOM_CONTINUOUS_RANGE, RANDOM_CONTINUOUS_RANGE));
					}

					break;
				}
			}
		}
	}

	//Do some cleanup - Remove any line segments that are intersecting with another.
	DynamicArray<bool> delete_line_segment;
	delete_line_segment.Upsize<false>(line_segments.Size());
	Memory::Set(delete_line_segment.Data(), 0, sizeof(bool) * delete_line_segment.Size());

	for (int64 line_segment_index{ static_cast<int64>(line_segments.LastIndex()) }; line_segment_index >= 0; --line_segment_index)
	{
		const LineSegment &line_segment{ line_segments[line_segment_index] };

		for (uint64 line_segment_point_index{ 1 }; line_segment_point_index < line_segment._Points.Size(); ++line_segment_point_index)
		{
			//Break if already marked for deletion.
			if (delete_line_segment[line_segment_index])
			{
				break;
			}

			const LineSegmentPoint &line_segment_point{ line_segment._Points[line_segment_point_index] };

			for (int64 other_line_segment_index{ static_cast<int64>(line_segments.LastIndex()) }; other_line_segment_index >= 0; --other_line_segment_index)
			{
				const LineSegment &other_line_segment{ line_segments[other_line_segment_index] };

				//Break if already marked for deletion.
				if (delete_line_segment[line_segment_index])
				{
					break;
				}

				//Ignore parent.
				if (line_segment._ParentLineSegmentIndex == other_line_segment_index)
				{
					continue;
				}

				//Ignore self.
				if (line_segment_index == other_line_segment_index)
				{
					continue;
				}

				//Ignore if this is already marked for deletion.
				if (delete_line_segment[other_line_segment_index])
				{
					continue;
				}

				for (uint64 other_line_segment_point_index{ 1 }; other_line_segment_point_index < other_line_segment._Points.Size(); ++other_line_segment_point_index)
				{
					const LineSegmentPoint &other_line_segment_point{ other_line_segment._Points[other_line_segment_point_index] };

					//Ignore parent.
					if (other_line_segment._ParentLineSegmentIndex == line_segment_index)
					{
						continue;
					}

					const float32 line_segment_point_radius{ line_segment._Radius * (1.0f - CatalystBaseMath::Square(line_segment_point._NormalizedHeight)) };
					const float32 other_line_segment_point_radius{ other_line_segment._Radius * (1.0f - CatalystBaseMath::Square(other_line_segment_point._NormalizedHeight)) };
					
					if (Vector3<float32>::Length(line_segment_point._Position - other_line_segment_point._Position) < (line_segment_point_radius + other_line_segment_point_radius))
					{
						delete_line_segment[line_segment_index] = true;

						break;
					}
				}
			}
		}
	}

	//Delete orphaned line segments.
	for (uint64 line_segment_index{ 0 }; line_segment_index < line_segments.Size(); ++line_segment_index)
	{
		bool any_parent_deleted{ false };

		uint64 current_line_segment_index{ line_segment_index };

		for (;;)
		{
			const uint64 parent_line_segment_index{ line_segments[current_line_segment_index]._ParentLineSegmentIndex };

			if (parent_line_segment_index == UINT64_MAXIMUM)
			{
				break;
			}

			if (delete_line_segment[parent_line_segment_index])
			{
				any_parent_deleted = true;

				break;
			}

			current_line_segment_index = parent_line_segment_index;
		}
		
		if (any_parent_deleted)
		{
			delete_line_segment[line_segment_index] = true;
		}
	}

	//Delete the marke line segments.
	for (uint64 line_segment_index{ 0 }; line_segment_index < line_segments.Size();)
	{
		if (delete_line_segment[line_segment_index])
		{
			line_segments.EraseAt<false>(line_segment_index);
			delete_line_segment.EraseAt<false>(line_segment_index);
		}

		else
		{
			++line_segment_index;
		}
	}

	/*
	*	Polygonize the line segments for the trunk.
	*	Gather list of vertices that we can spawn crown branches on.
	*/
	output->_Vertices.Emplace();
	output->_Indices.Emplace();
	DynamicArray<uint32> crown_branch_candidates;

	for (const LineSegment &line_segment : line_segments)
	{
		//Calculate the number of circle segments for this line segment.
		const uint32 number_of_circle_segments{ static_cast<uint32>(CatalystBaseMath::LinearlyInterpolate(static_cast<float32>(NUMBER_OF_CIRCLE_SEGMENTS), 4.0f, line_segment._Points[0]._NormalizedHeight)) };

		for (uint64 line_segment_index{ 0 }; line_segment_index < line_segment._Points.Size(); ++line_segment_index)
		{
			//Calculate the radius at this point.
			const float32 radius_at_point{ line_segment._Radius * (1.0f - CatalystBaseMath::Square(line_segment._Points[line_segment_index]._NormalizedHeight)) };

			//Calculate the circumference at this point.
			const float32 circumference_at_point{ 2.0f * CatalystBaseMathConstants::PI * line_segment._Radius };

			for (uint32 circle_segment_index{ 0 }; circle_segment_index < number_of_circle_segments; ++circle_segment_index)
			{
				//Calculate the circle percent.
				const float32 circle_percent{ static_cast<float32>(circle_segment_index) / static_cast<float32>(number_of_circle_segments - 1) };

				//Calculate the rotation.
				const float32 rotation{ CatalystBaseMathConstants::DOUBLE_PI * circle_percent };

				//Cache the vertex index.
				const uint32 vertex_index{ static_cast<uint32>(output->_Vertices.Back().Size()) };

				//Add the vertex.
				output->_Vertices.Back().Emplace();
				Vertex &new_vertex{ output->_Vertices.Back().Back() };

				//Set the position.
				if (circle_segment_index == number_of_circle_segments - 1)
				{
					new_vertex._Position = output->_Vertices.Back()[vertex_index - number_of_circle_segments + 1]._Position;
				}

				else
				{
					Vector3<float32> position_offset{ 0.0f, 0.0f, radius_at_point };
					
					for (uint8 i{ 0 }; i < 3; ++i)
					{
						position_offset[i] += CatalystRandomMath::RandomFloatInRange(-radius_at_point * 0.125f * 0.25f, radius_at_point * 0.125f * 0.25f);
					}
					
					position_offset.Rotate(EulerAngles(0.0f, rotation, 0.0f));
					position_offset.Rotate(line_segment._Points[line_segment_index]._Rotation);
					new_vertex._Position = line_segment._Points[line_segment_index]._Position + position_offset;
				}
				
				//Set the normal.
				new_vertex._Normal = Vector3<float32>(0.0f, 0.0f, 1.0f);
				new_vertex._Normal.Rotate(EulerAngles(0.0f, rotation, 0.0f));
				new_vertex._Normal.Rotate(line_segment._Points[line_segment_index]._Rotation);

				//Set the tangent.
				new_vertex._Tangent = Vector3<float32>(0.0f, 1.0f, 0.0f);
				new_vertex._Tangent.Rotate(EulerAngles(0.0f, rotation, 0.0f));
				new_vertex._Tangent.Rotate(line_segment._Points[line_segment_index]._Rotation);

				//Set the texture coordinate.
				new_vertex._TextureCoordinate = Vector2<float32>(circle_percent, line_segment._Points[line_segment_index]._Position._Y / circumference_at_point);
			
				//If this is not the first line segment, and not the first line segment, add the indices.
				if (line_segment_index > 0 && circle_segment_index > 0)
				{
					output->_Indices.Back().Emplace(vertex_index - number_of_circle_segments - 1);
					output->_Indices.Back().Emplace(vertex_index);
					output->_Indices.Back().Emplace(vertex_index - 1);

					output->_Indices.Back().Emplace(vertex_index - number_of_circle_segments - 1);
					output->_Indices.Back().Emplace(vertex_index - number_of_circle_segments);
					output->_Indices.Back().Emplace(vertex_index);
				}

				//Calculate the crown branch weight.
				const float32 crown_branch_weight{ CatalystBaseMath::Square(CatalystBaseMath::Maximum(line_segment._Points[line_segment_index]._NormalizedHeight - MINIMUM_CROWN_HEIGHT, 0.0f) * (1.0f / (1.0f - MINIMUM_CROWN_HEIGHT))) };

				if (CatalystRandomMath::RandomChance(crown_branch_weight))
				{
					crown_branch_candidates.Emplace(vertex_index);
				}
			}
		}
	}

	//Set up the collision vertices/indices.
	for (const LineSegment &line_segment : line_segments)
	{
		for (uint64 line_segment_index{ 0 }; line_segment_index < line_segment._Points.Size(); ++line_segment_index)
		{
			//Calculate the radius at this point.
			const float32 radius_at_point{ line_segment._Radius * (1.0f - CatalystBaseMath::Square(line_segment._Points[line_segment_index]._NormalizedHeight)) };

			//Don't care about really thin branches after a certain point.
			if (radius_at_point < (line_segment._Radius * 0.5f))
			{
				continue;
			}

			//Calculate the circumference at this point.
			const float32 circumference_at_point{ 2.0f * CatalystBaseMathConstants::PI * line_segment._Radius };

			for (uint32 circle_segment_index{ 0 }; circle_segment_index < NUMBER_OF_COLLISION_CIRCLE_SEGMENTS; ++circle_segment_index)
			{
				//Calculate the circle percent.
				const float32 circle_percent{ static_cast<float32>(circle_segment_index) / static_cast<float32>(NUMBER_OF_COLLISION_CIRCLE_SEGMENTS - 1) };

				//Calculate the rotation.
				const float32 rotation{ CatalystBaseMathConstants::DOUBLE_PI * circle_percent };

				//Cache the vertex index.
				const uint32 vertex_index{ static_cast<uint32>(output->_CollisionVertices.Size()) };

				//Add the vertex.
				output->_CollisionVertices.Emplace();
				Vertex &new_vertex{ output->_CollisionVertices.Back() };

				//Set the position.
				Vector3<float32> position_offset{ 0.0f, 0.0f, radius_at_point };
				position_offset.Rotate(EulerAngles(0.0f, rotation, 0.0f));
				position_offset.Rotate(line_segment._Points[line_segment_index]._Rotation);
				new_vertex._Position = line_segment._Points[line_segment_index]._Position + position_offset;

				//Set the normal.
				new_vertex._Normal = Vector3<float32>(0.0f, 0.0f, 1.0f);
				new_vertex._Normal.Rotate(EulerAngles(0.0f, rotation, 0.0f));
				new_vertex._Normal.Rotate(line_segment._Points[line_segment_index]._Rotation);

				//Set the tangent.
				new_vertex._Tangent = Vector3<float32>(0.0f, 1.0f, 0.0f);
				new_vertex._Tangent.Rotate(EulerAngles(0.0f, rotation, 0.0f));
				new_vertex._Tangent.Rotate(line_segment._Points[line_segment_index]._Rotation);

				//Set the texture coordinate.
				new_vertex._TextureCoordinate = Vector2<float32>(circle_percent, line_segment._Points[line_segment_index]._Position._Y / circumference_at_point);

				//If this is not the first line segment, and not the first line segment, add the indices.
				if (line_segment_index > 0 && circle_segment_index > 0)
				{
					output->_CollisionIndices.Emplace(vertex_index - NUMBER_OF_COLLISION_CIRCLE_SEGMENTS - 1);
					output->_CollisionIndices.Emplace(vertex_index);
					output->_CollisionIndices.Emplace(vertex_index - 1);

					output->_CollisionIndices.Emplace(vertex_index - NUMBER_OF_COLLISION_CIRCLE_SEGMENTS - 1);
					output->_CollisionIndices.Emplace(vertex_index - NUMBER_OF_COLLISION_CIRCLE_SEGMENTS);
					output->_CollisionIndices.Emplace(vertex_index);
				}
			}
		}
	}

	//Spawn crown branches!
	output->_Vertices.Emplace();
	output->_Indices.Emplace();
	DynamicArray<Vector3<float32>> crown_point_cloud;

	for (uint32 i{ 0 }; i < 256; ++i)
	{
		//Pick a random vertex to spawn on.
		const uint64 random_candidate_index{ CatalystRandomMath::RandomIntegerInRange<uint64>(0, crown_branch_candidates.LastIndex()) };
		const uint32 random_vertex_index{ crown_branch_candidates[random_candidate_index] };
		const Vertex &spawn_vertex{ output->_Vertices[0][random_vertex_index] };

		//Remember the start index.
		const uint32 start_index{ static_cast<uint32>(output->_Vertices.Back().Size()) };

		//Calculate the random rotation.
		const EulerAngles random_rotation
		{
			CatalystRandomMath::RandomFloatInRange(-CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) * 0.25f,
			CatalystRandomMath::RandomFloatInRange(-CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI),
			CatalystRandomMath::RandomFloatInRange(-CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) * 0.25f
		};

		//Add the vertices.
		for (uint8 vertex_index{ 0 }; vertex_index < 4; ++vertex_index)
		{
			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate{ static_cast<float32>(vertex_index / 2), static_cast<float32>(vertex_index & 1) };

			//Add the vertex.
			output->_Vertices.Back().Emplace();
			Vertex& new_vertex{ output->_Vertices.Back().Back() };

			//Set the position.
			Vector3<float32> position_offset{ normalized_coordinate._X * 2.0f - 1.0f, normalized_coordinate._Y * 2.0f, 0.0f };
			position_offset *= 4.0f;
			position_offset.Rotate(random_rotation);
			new_vertex._Position = spawn_vertex._Position + position_offset;

			crown_point_cloud.Emplace(new_vertex._Position);

			//Set the normal.
			new_vertex._Normal = Vector3<float32>(0.0f, 0.0f, -1.0f);
			new_vertex._Normal.Rotate(random_rotation);

			//Set the tangent.
			new_vertex._Tangent = Vector3<float32>(0.0f, 1.0f, 0.0f);
			new_vertex._Tangent.Rotate(random_rotation);

			//Set the texture coordinate.
			new_vertex._TextureCoordinate = Vector2<float32>(normalized_coordinate._X, 1.0f - normalized_coordinate._Y);
		}

		//Add the indices.
		output->_Indices.Back().Emplace(start_index);
		output->_Indices.Back().Emplace(start_index + 3);
		output->_Indices.Back().Emplace(start_index + 2);

		output->_Indices.Back().Emplace(start_index);
		output->_Indices.Back().Emplace(start_index + 1);
		output->_Indices.Back().Emplace(start_index + 3);
	}

#if 0
	//Augment the normals/tangents of the crown.
	{
		Vector3<float32> crown_center{ 0.0f, 0.0f, 0.0f };

		for (const Vector3<float32> &crown_point : crown_point_cloud)
		{
			crown_center += crown_point / static_cast<float32>(crown_point_cloud.Size());
		}

		for (Vertex &vertex : output->_Vertices.Back())
		{
			const float32 distance_from_center{ Vector3<float32>::Length(vertex._Position - crown_center) };

			vertex._Normal = (vertex._Position - crown_center) / distance_from_center;
			vertex._Tangent = Vector3<float32>::Normalize(Vector3<float32>::CrossProduct(Vector3<float32>(0.0f, 1.0f, 0.0f), (vertex._Position - crown_center)));
		}
	}
#endif
}