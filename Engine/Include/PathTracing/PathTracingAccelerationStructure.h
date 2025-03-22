#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Pair.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Triangle.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Path tracing.
#include <PathTracing/PathTracingCore.h>

//Systems.
#include <Systems/LogSystem.h>

class PathTracingAccelerationStructure final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PathTracingAccelerationStructure() NOEXCEPT
	{
		//Create the build data.
		_BuildData = static_cast<BuildData * RESTRICT>(Memory::Allocate(sizeof(BuildData)));
		Memory::Set(_BuildData, 0, sizeof(BuildData));

		//Reset the triangles.
		Memory::Set(&_Root._Triangles, 0, sizeof(DynamicArray<PathTracingTriangle>));
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~PathTracingAccelerationStructure() NOEXCEPT
	{
		//Destroy the build data.
		if (_BuildData)
		{
			_BuildData->~BuildData();
			Memory::Free(_BuildData);
			_BuildData = nullptr;
		}

		//Destroy the acceleration structure memory.
		if (_AccelerationStructureMemory)
		{
			Memory::Free(_AccelerationStructureMemory);
			_AccelerationStructureMemory = nullptr;
		}
	}

	/*
	*	Adds a vertex to the acceleration structure. Must be done before building.
	*/
	FORCE_INLINE void AddVertex(const Vertex &vertex) NOEXCEPT
	{
		//Add the vertex to the acceleration structure.
		_BuildData->_Vertices.Emplace(vertex);
	}

	/*
	*	Adds a triangle to the acceleration structure. Must be done before building.
	*/
	FORCE_INLINE void AddTriangle(const PathTracingTriangle &triangle) NOEXCEPT
	{
		//Add the triangle to the root.
		_Root._Triangles.Emplace(triangle);
	}

	/*
	*	Builds the acceleration structure.
	*/
	FORCE_INLINE void Build(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Execute the pre build step.
		ExecutePreBuild(maximum_triangles_per_node);

		//Execute the build step.
		ExecuteBuild(maximum_triangles_per_node);

		//Execute the post build step.
		ExecutePostBuild(maximum_triangles_per_node);
	}

	/*
	*	Traces a surface ray through the acceleration structure.
	*	If an intersection is detected, it returns the intersected triangle.
	*	If no interseaction was found, it returns nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const PathTracingTriangle *const RESTRICT TraceSurface(const Ray &ray, float32 *const RESTRICT intersection_distance) const NOEXCEPT
	{
		return TraceSurface(ray, _Root, intersection_distance);
	}

	/*
	*	Traces a shadow ray through the acceleration structure.
	*	If an intersection is detected, it returns true
	*	If no interseaction was found, it returns false.
	*/
	FORCE_INLINE NO_DISCARD bool TraceShadow(const Ray& ray, const float32 maximum_distance) const NOEXCEPT
	{
		return TraceShadow(ray, maximum_distance, _Root);
	}

	/*
	*	Returns the vertex at the given index.
	*/
	FORCE_INLINE NO_DISCARD const Vertex &GetVertex(const uint64 index) const NOEXCEPT
	{
		return _VerticesMemory[index];
	}

private:

	/*
	*	Node class definition.
	*/
	class Node final
	{

	public:

		//Denotes whether or not this node holdes axis aligned bounding boxes or triangles.
		bool _HasTriangles{ true };

		union
		{
			struct
			{
				//The axis aligned bounding boxes.
				StaticArray<AxisAlignedBoundingBox3D, 2> _AxisAlignedBoundingBoxes;

				//The nodes.
				Node *RESTRICT _Nodes;
			};

			union
			{
				//The triangles.
				DynamicArray<PathTracingTriangle> _Triangles;

				struct
				{
					//The triangles memory.
					PathTracingTriangle *RESTRICT _TrianglesMemory;

					//The size of the triangles.
					uint64 _TrianglesSize;
				};
			};

		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Node() NOEXCEPT
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~Node() NOEXCEPT
		{

		}

	};

	/*
	*	Build data class definition.
	*/
	class BuildData final
	{

	public:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The nodes to be split, and their axis aligned bounding boxes.
		DynamicArray<Pair<Node *const RESTRICT, AxisAlignedBoundingBox3D>> _NodesToBeSplit;

		//The bytes needed for the vertices.
		uint64 _NumberBytesNeededForVertices{ 0 };

		//The bytes needed for the triangle´s.
		uint64 _NumberBytesNeededForTriangles{ 0 };

		//The number of bytes needed for nodes.
		uint64 _NumberOfBytesNeededForNodes{ 0 };

		//The current acceleration structure memory offset.
		uint64 _AccelerationStructureMemoryOffset{ 0 };

	};

	//The root.
	Node _Root;

	//The acceleration structure memory.
	void *RESTRICT _AccelerationStructureMemory{ nullptr };

	//The vertices memory.
	Vertex *RESTRICT _VerticesMemory;

	//The build data.
	BuildData *RESTRICT _BuildData;

	/*
	*	Executes the pre build step.
	*/
	FORCE_INLINE void ExecutePreBuild(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Record the number of bytes needed for the vertices.
		_BuildData->_NumberBytesNeededForVertices = _BuildData->_Vertices.Size() * sizeof(Vertex);

		//Record the number of bytes needed for the triangles.
		_BuildData->_NumberBytesNeededForTriangles = _Root._Triangles.Size() * sizeof(PathTracingTriangle);

		//First, calculate axis aligned bounding box for the root.
		AxisAlignedBoundingBox3D root_box;

		for (const PathTracingTriangle &triangle : _Root._Triangles)
		{
			root_box.Expand(_BuildData->_Vertices[triangle._Indices[0]]._Position);
			root_box.Expand(_BuildData->_Vertices[triangle._Indices[1]]._Position);
			root_box.Expand(_BuildData->_Vertices[triangle._Indices[2]]._Position);
		}

		//If the root node has more than the maximum number of triangles, add it to the nodes to be split.
		if (_Root._Triangles.Size() > maximum_triangles_per_node)
		{
			_BuildData->_NodesToBeSplit.Emplace(&_Root, root_box);
		}
	}

	/*
	*	Executes the build step.
	*/
	FORCE_INLINE void ExecuteBuild(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Process all nodes to be split.
		while (!_BuildData->_NodesToBeSplit.Empty())
		{
			Node* const RESTRICT node{ _BuildData->_NodesToBeSplit.Back()._First };
			const AxisAlignedBoundingBox3D axis_aligned_bounding_box{ _BuildData->_NodesToBeSplit.Back()._Second };

			_BuildData->_NodesToBeSplit.Pop();

			SplitNode(maximum_triangles_per_node, axis_aligned_bounding_box, node);
		}
	}

	/*
	*	Executes the post build step.
	*/
	FORCE_INLINE void ExecutePostBuild(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Allocate the acceleration structure memory.
		_AccelerationStructureMemory = Memory::Allocate(_BuildData->_NumberBytesNeededForVertices + _BuildData->_NumberBytesNeededForTriangles + _BuildData->_NumberOfBytesNeededForNodes);

		LOG_INFORMATION("Acceleration structure memory usage: %llu", _BuildData->_NumberBytesNeededForVertices + _BuildData->_NumberBytesNeededForTriangles + _BuildData->_NumberOfBytesNeededForNodes);

		//Copy the vertices to the acceleration structure memory.
		Memory::Copy(_AccelerationStructureMemory, _BuildData->_Vertices.Data(), _BuildData->_NumberBytesNeededForVertices);

		_VerticesMemory = static_cast<Vertex *const RESTRICT>(_AccelerationStructureMemory);

		//Update the acceleration structure memory offset.
		_BuildData->_AccelerationStructureMemoryOffset += _BuildData->_NumberBytesNeededForVertices;

		//Traverse the acceleration tree and refit all of the axis aligned bounding boxes.
		if (!_Root._HasTriangles)
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				Refit(&_Root._Nodes[i], &_Root._AxisAlignedBoundingBoxes[i]);
			}
		}

		else
		{
			CopyNodeToAccelerationStructureMemory(&_Root);
		}

		//Destroy the build data.
		_BuildData->~BuildData();
		Memory::Free(_BuildData);
		_BuildData = nullptr;
	}

	/*
	*	Splits a node.
	*/
	FORCE_INLINE void SplitNode(const uint64 maximum_triangles_per_node, const AxisAlignedBoundingBox3D& box, Node* const RESTRICT node) NOEXCEPT
	{
		//Decide on which axis the box should be split.
		AxisAlignedBoundingBox3D first;
		AxisAlignedBoundingBox3D second;

		{
			const float32 x_axis{ box._Maximum._X - box._Minimum._X };
			const float32 y_axis{ box._Maximum._Y - box._Minimum._Y };
			const float32 z_axis{ box._Maximum._Z - box._Minimum._Z };

			if (x_axis > y_axis && x_axis > z_axis) //X axis is the longest.
			{
				first._Minimum = Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float32>(box._Minimum._X + (x_axis * 0.5f), box._Maximum._Y, box._Maximum._Z);

				second._Minimum = Vector3<float32>(box._Minimum._X + (x_axis * 0.5f), box._Minimum._Y, box._Minimum._Z);
				second._Maximum = Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}

			else if (y_axis > z_axis) //Y axis is the longest.
			{
				first._Minimum = Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float32>(box._Maximum._X, box._Minimum._Y + (y_axis * 0.5f), box._Maximum._Z);

				second._Minimum = Vector3<float32>(box._Minimum._X, box._Minimum._Y + (y_axis * 0.5f), box._Minimum._Z);
				second._Maximum = Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}

			else //Z axis is the longest.
			{
				first._Minimum = Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Minimum._Z + (z_axis * 0.5f));

				second._Minimum = Vector3<float32>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z + (z_axis * 0.5f));
				second._Maximum = Vector3<float32>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}
		}

		//Allocate the two new nodes.
		Node* const RESTRICT nodes{ static_cast<Node *const RESTRICT>(Memory::Allocate(sizeof(Node) * 2)) };

		//Initialize the new nodes.
		Memory::Set(nodes, 0, sizeof(Node) * 2);

		//Update the number of bytes needed for nodes.
		_BuildData->_NumberOfBytesNeededForNodes += sizeof(Node) * 2;

		//Set initial properties of the two new nodes.
		nodes[0]._HasTriangles = true;
		nodes[1]._HasTriangles = true;

		//Preallocate for the worst case to avoid many rellocations later.
		nodes[0]._Triangles.Reserve(node->_Triangles.Size());
		nodes[1]._Triangles.Reserve(node->_Triangles.Size());

		//Start filling up the two new nodes with the triangle data.
		for (const PathTracingTriangle &triangle : node->_Triangles)
		{
			//Cache the triangle.
			Triangle _triangle;

			_triangle._Vertices[0] = _BuildData->_Vertices[triangle._Indices[0]]._Position;
			_triangle._Vertices[1] = _BuildData->_Vertices[triangle._Indices[1]]._Position;
			_triangle._Vertices[2] = _BuildData->_Vertices[triangle._Indices[2]]._Position;

			//Calculate the center of the triangle.
			const Vector3<float32> triangle_center{ Triangle::CalculateCenter(_triangle) };

			//Decide which axis aligned bounding box this triangle data should go into, based on the triangle center.
			if (first.IsInside(triangle_center))
			{
				//Add the triangle.
				nodes[0]._Triangles.Emplace(triangle);
			}

			else if (second.IsInside(triangle_center))
			{
				//Add the triangle.
				nodes[1]._Triangles.Emplace(triangle);
			}

			else
			{
				if (nodes[0]._Triangles.Size() < nodes[1]._Triangles.Size())
				{
					//Add the triangle data.
					nodes[0]._Triangles.Emplace(triangle);
				}

				else
				{
					//Add the triangle data.
					nodes[1]._Triangles.Emplace(triangle);
				}
			}
		}

		//Destroy the original nodes triangles.
		node->_HasTriangles = false;
		node->_Triangles.~DynamicArray<PathTracingTriangle>();

		//Add the new nodes.
		node->_AxisAlignedBoundingBoxes[0] = first;
		node->_AxisAlignedBoundingBoxes[1] = second;

		node->_Nodes = nodes;

		//Check if either of the two new nodes need to be split.
		if (node->_Nodes[0]._Triangles.Size() > maximum_triangles_per_node)
		{
			_BuildData->_NodesToBeSplit.Emplace(&nodes[0], node->_AxisAlignedBoundingBoxes[0]);
		}

		if (node->_Nodes[1]._Triangles.Size() > maximum_triangles_per_node)
		{
			_BuildData->_NodesToBeSplit.Emplace(&nodes[1], node->_AxisAlignedBoundingBoxes[1]);
		}
	}

	/*
	*	Copies a node´s data to the acceleration structure memory.
	*/
	FORCE_INLINE void CopyNodeToAccelerationStructureMemory(Node* const RESTRICT node) NOEXCEPT
	{
		byte* RESTRICT acceleration_structure_memory{ static_cast<byte * RESTRICT>(_AccelerationStructureMemory) + _BuildData->_AccelerationStructureMemoryOffset };

		if (node->_HasTriangles)
		{
			//Copy over the old triangles to the new triangles memory.
			const uint64 triangles_size{ node->_Triangles.Size() };
			Memory::Copy(acceleration_structure_memory, node->_Triangles.Data(), triangles_size * sizeof(PathTracingTriangle));

			node->_Triangles.Destroy();

			node->_TrianglesMemory = static_cast<PathTracingTriangle *RESTRICT>(static_cast<void *RESTRICT>(acceleration_structure_memory));
			node->_TrianglesSize = triangles_size;

			_BuildData->_AccelerationStructureMemoryOffset += triangles_size * sizeof(PathTracingTriangle);
		}

		else
		{
			//Copy over the new node data to the new node data memory.
			Memory::Copy(acceleration_structure_memory, node->_Nodes, sizeof(Node) * 2);

			Memory::Free(node->_Nodes);

			node->_Nodes = static_cast<Node *RESTRICT>(static_cast<void *RESTRICT>(acceleration_structure_memory));

			_BuildData->_AccelerationStructureMemoryOffset += sizeof(Node) * 2;
		}
	}

	/*
	*	Refit the node´s axis aligned bounding box(es).
	*/
	FORCE_INLINE void Refit(Node *const RESTRICT node, AxisAlignedBoundingBox3D *const RESTRICT box) NOEXCEPT
	{
		CopyNodeToAccelerationStructureMemory(node);

		if (node->_HasTriangles)
		{
			box->Invalidate();

			for (uint64 triangle_index{ 0 }; triangle_index < node->_TrianglesSize; ++triangle_index)
			{
				for (uint8 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					box->Expand(_VerticesMemory[node->_TrianglesMemory[triangle_index]._Indices[vertex_index]]._Position);
				}
			}
		}

		else
		{
			box->Invalidate();

			for (uint8 i{ 0 }; i < 2; ++i)
			{
				Refit(&node->_Nodes[i], &node->_AxisAlignedBoundingBoxes[i]);

				if (node->_AxisAlignedBoundingBoxes[i].IsValid())
				{
					box->Expand(node->_AxisAlignedBoundingBoxes[i]);
				}
			}
		}
	}

	/*
	*	Traces a surface ray through the acceleration structure via the given node.
	*	If an intersection is detected, it returns the intersected triangle data.
	*	If no interseaction was found, it returns nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const PathTracingTriangle *const RESTRICT TraceSurface(const Ray &ray, const Node &node, float32 *const RESTRICT intersection_distance) const NOEXCEPT
	{
		float32 maximum_distance_squared{ *intersection_distance == FLOAT32_MAXIMUM ? FLOAT32_MAXIMUM : *intersection_distance * *intersection_distance };

		const PathTracingTriangle *RESTRICT intersected_triangle{ nullptr };

		if (node._HasTriangles)
		{
			for (uint64 i{ 0 }; i < node._TrianglesSize; ++i)
			{
				const PathTracingTriangle &triangle{ node._TrianglesMemory[i] };

				float32 intersection_distance_temporary{ FLOAT32_MAXIMUM };

				//Cache the triangle.
				Triangle _triangle;

				_triangle._Vertices[0] = _VerticesMemory[triangle._Indices[0]]._Position;
				_triangle._Vertices[1] = _VerticesMemory[triangle._Indices[1]]._Position;
				_triangle._Vertices[2] = _VerticesMemory[triangle._Indices[2]]._Position;

				if (CatalystGeometryMath::RayTriangleIntersection(ray, _triangle, &intersection_distance_temporary)
					&& intersection_distance_temporary < *intersection_distance)
				{
					bool is_actually_a_hit{ true };

					/*
					if (triangle_data._DiscardFunction)
					{
						is_actually_a_hit = triangle_data._DiscardFunction(*this, ray, triangle_data._Indices[0], triangle_data._Indices[1], triangle_data._Indices[2], intersection_distance_temporary);
					}
					*/

					if (is_actually_a_hit)
					{
						intersected_triangle = &triangle;
						*intersection_distance = intersection_distance_temporary;
						maximum_distance_squared = intersection_distance_temporary * intersection_distance_temporary;
					}
				}
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				//Check first if this box is close enough.
				const Vector3<float32> closest_point
				{
					BaseMath::Clamp(ray._Origin._X, node._AxisAlignedBoundingBoxes[i]._Minimum._X, node._AxisAlignedBoundingBoxes[i]._Maximum._X),
					BaseMath::Clamp(ray._Origin._Y, node._AxisAlignedBoundingBoxes[i]._Minimum._Y, node._AxisAlignedBoundingBoxes[i]._Maximum._Y),
					BaseMath::Clamp(ray._Origin._Z, node._AxisAlignedBoundingBoxes[i]._Minimum._Z, node._AxisAlignedBoundingBoxes[i]._Maximum._Z)
				};

				if (Vector3<float32>::LengthSquared(ray._Origin - closest_point) >= maximum_distance_squared)
				{
					continue;
				}

				float intersection_distance_temporary{ FLOAT32_MAXIMUM };

				if (node._AxisAlignedBoundingBoxes[i].IsValid()
					&& CatalystGeometryMath::RayBoxIntersection(ray, node._AxisAlignedBoundingBoxes[i], &intersection_distance_temporary)
					&& intersection_distance_temporary < *intersection_distance)
				{
					if (const PathTracingTriangle *const RESTRICT intersected_triangle_temporary{ TraceSurface(ray, node._Nodes[i], intersection_distance) })
					{
						intersected_triangle = intersected_triangle_temporary;
						maximum_distance_squared = *intersection_distance * *intersection_distance;
					}
				}
			}
		}

		return intersected_triangle;
	}

	/*
	*	Traces a shadow ray through the acceleration structure.
	*	If an intersection is detected, it returns true
	*	If no interseaction was found, it returns false.
	*/
	FORCE_INLINE NO_DISCARD bool TraceShadow(const Ray &ray, const float32 maximum_distance, const Node &node) const NOEXCEPT
	{
		const float32 maximum_distance_squared{ maximum_distance * maximum_distance };

		if (node._HasTriangles)
		{
			for (uint64 i{ 0 }; i < node._TrianglesSize; ++i)
			{
				const PathTracingTriangle &triangle{ node._TrianglesMemory[i] };

				float32 intersection_distance_temporary{ FLOAT32_MAXIMUM };

				//Cache the triangle.
				Triangle _triangle;

				_triangle._Vertices[0] = _VerticesMemory[triangle._Indices[0]]._Position;
				_triangle._Vertices[1] = _VerticesMemory[triangle._Indices[1]]._Position;
				_triangle._Vertices[2] = _VerticesMemory[triangle._Indices[2]]._Position;

				if (CatalystGeometryMath::RayTriangleIntersection(ray, _triangle, &intersection_distance_temporary)
					&& intersection_distance_temporary < maximum_distance)
				{
					bool is_actually_a_hit{ true };

					/*
					if (triangle_data._DiscardFunction)
					{
						is_actually_a_hit = triangle_data._DiscardFunction(*this, ray, triangle_data._Indices[0], triangle_data._Indices[1], triangle_data._Indices[2], intersection_distance_temporary);
					}
					*/

					if (is_actually_a_hit)
					{
						return true;
					}
				}
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				if (!node._AxisAlignedBoundingBoxes[i].IsValid())
				{
					continue;
				}

				//Check first if this box is close enough.
				const Vector3<float32> closest_point
				{
					BaseMath::Clamp(ray._Origin._X, node._AxisAlignedBoundingBoxes[i]._Minimum._X, node._AxisAlignedBoundingBoxes[i]._Maximum._X),
					BaseMath::Clamp(ray._Origin._Y, node._AxisAlignedBoundingBoxes[i]._Minimum._Y, node._AxisAlignedBoundingBoxes[i]._Maximum._Y),
					BaseMath::Clamp(ray._Origin._Z, node._AxisAlignedBoundingBoxes[i]._Minimum._Z, node._AxisAlignedBoundingBoxes[i]._Maximum._Z)
				};

				if (Vector3<float32>::LengthSquared(ray._Origin - closest_point) >= maximum_distance_squared)
				{
					continue;
				}

				float intersection_distance_temporary{ FLOAT32_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, node._AxisAlignedBoundingBoxes[i], &intersection_distance_temporary)
					&& intersection_distance_temporary < maximum_distance)
				{
					if (TraceShadow(ray, maximum_distance, node._Nodes[i]))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

};