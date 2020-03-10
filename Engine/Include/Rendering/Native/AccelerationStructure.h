#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Pair.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Triangle.h>

//Memory.
#include <Memory/PoolAllocator.h>

template <typename TYPE>
class AccelerationStructure final
{

public:

	/*
	*	Triangle data class definition.
	*/
	class TriangleData final
	{

	public:

		//The indices.
		StaticArray<uint32, 3> _Indices;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE TriangleData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE TriangleData(const StaticArray<uint32, 3> &initial_indices) NOEXCEPT
			:
			_Indices(initial_indices)
		{

		}

	};

	/*
	*	Vertex data class definition.
	*/
	class VertexData final
	{

	public:

		//The position.
		Vector3<float32> _Position;

		//The user data.
		TYPE _UserData;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE VertexData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE VertexData(const Vector3<float32> &initial_position, const TYPE &initial_user_data) NOEXCEPT
			:
			_Position(initial_position),
			_UserData(initial_user_data)
		{

		}

	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AccelerationStructure() NOEXCEPT
	{
		//Create the build data.
		_BuildData = static_cast<BuildData *RESTRICT>(Memory::Allocate(sizeof(BuildData)));
		Memory::Set(_BuildData, 0, sizeof(BuildData));

		//Reset the triangle data.
		Memory::Set(&_Root._TriangleData, 0, sizeof(DynamicArray<TriangleData>));
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~AccelerationStructure() NOEXCEPT
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
	*	Adds triangle data to the acceleration structure. Must be done before building.
	*/
	FORCE_INLINE void AddTriangleData(const TriangleData &triangle_data) NOEXCEPT
	{
		//Add the triangle data to the root.
		_Root._TriangleData.Emplace(triangle_data);
	}

	/*
	*	Adds vertex data to the acceleration structure. Must be done before building.
	*/
	FORCE_INLINE void AddVertexData(const VertexData &vertex_data) NOEXCEPT
	{
		//Add the vertex data to the acceleration structure.
		_BuildData->_VertexData.Emplace(vertex_data);
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
	*	If an intersection is detected, it returns the intersected triangle data.
	*	If no interseaction was found, it returns nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TriangleData *const RESTRICT TraceSurface(const Ray& ray, float *const RESTRICT intersection_distance) const NOEXCEPT
	{
		return TraceSurface(ray, _Root, intersection_distance);
	}

	/*
	*	Traces a shadow ray through the acceleration structure.
	*	If an intersection is detected, it returns true
	*	If no interseaction was found, it returns false.
	*/
	FORCE_INLINE NO_DISCARD bool TraceShadow(const Ray& ray) const NOEXCEPT
	{
		return TraceShadow(ray, _Root);
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
				StaticArray<AxisAlignedBoundingBox, 2> _AxisAlignedBoundingBoxes;

				//The nodes.
				Node *RESTRICT _Nodes;
			};

			union
			{
				//The triangle data.
				DynamicArray<TriangleData> _TriangleData;

				struct
				{
					//The triangle data memory.
					TriangleData *RESTRICT _TriangleDataMemory;

					//The size of the triangle data.
					uint64 _TriangleDataSize;
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

		//The vertex data.
		DynamicArray<VertexData> _VertexData;

		//The nodes to be split, and their axis aligned bounding boxes.
		DynamicArray<Pair<Node* const RESTRICT, AxisAlignedBoundingBox>> _NodesToBeSplit;

		//The bytes needed for the vertex data.
		uint64 _NumberBytesNeededForVertexData{ 0 };

		//The bytes needed for the triangle data.
		uint64 _NumberBytesNeededForTriangleData{ 0 };

		//The number of bytes needed for nodes.
		uint64 _NumberOfBytesNeededForNodes{ 0 };

		//The current acceleration structure memory offset.
		uint64 _AccelerationStructureMemoryOffset{ 0 };

	};

	//The root.
	Node _Root;

	//The acceleration structure memory.
	void *RESTRICT _AccelerationStructureMemory{ nullptr };

	//The vertex data memory.
	VertexData* RESTRICT _VertexDataMemory;

	//The build data.
	BuildData *RESTRICT _BuildData;

	/*
	*	Executes the pre build step.
	*/
	FORCE_INLINE void ExecutePreBuild(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Record the number of bytes needed for the vertex data.
		_BuildData->_NumberBytesNeededForVertexData = _BuildData->_VertexData.Size() * sizeof(VertexData);

		//Record the number of bytes needed for the triangle data.
		_BuildData->_NumberBytesNeededForTriangleData = _Root._TriangleData.Size() * sizeof(TriangleData);

		//First, calculate axis aligned bounding box for the root.
		AxisAlignedBoundingBox root_box;

		for (const TriangleData& triangle_data : _Root._TriangleData)
		{
			root_box.Expand(_BuildData->_VertexData[triangle_data._Indices[0]]._Position);
			root_box.Expand(_BuildData->_VertexData[triangle_data._Indices[1]]._Position);
			root_box.Expand(_BuildData->_VertexData[triangle_data._Indices[2]]._Position);
		}

		//If the root node has more than the maximum number of triangles, add it to the nodes to be split.
		if (_Root._TriangleData.Size() > maximum_triangles_per_node)
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
			const AxisAlignedBoundingBox axis_aligned_bounding_box{ _BuildData->_NodesToBeSplit.Back()._Second };

			_BuildData->_NodesToBeSplit.PopFast();

			SplitNode(maximum_triangles_per_node, axis_aligned_bounding_box, node);
		}
	}

	/*
	*	Executes the post build step.
	*/
	FORCE_INLINE void ExecutePostBuild(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//Allocate the acceleration structure memory.
		_AccelerationStructureMemory = Memory::Allocate(_BuildData->_NumberBytesNeededForVertexData + _BuildData->_NumberBytesNeededForTriangleData + _BuildData->_NumberOfBytesNeededForNodes);

		PRINT_TO_OUTPUT("Acceleration structure memory usage: " << _BuildData->_NumberBytesNeededForVertexData + _BuildData->_NumberBytesNeededForTriangleData + _BuildData->_NumberOfBytesNeededForNodes);

		//Copy the vertex data to the acceleration structure memory.
		Memory::Copy(_AccelerationStructureMemory, _BuildData->_VertexData.Data(), _BuildData->_NumberBytesNeededForVertexData);

		_VertexDataMemory = static_cast<VertexData *const RESTRICT>(_AccelerationStructureMemory);

		//Update the acceleration structure memory offset.
		_BuildData->_AccelerationStructureMemoryOffset += _BuildData->_NumberBytesNeededForVertexData;

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
	FORCE_INLINE void SplitNode(const uint64 maximum_triangles_per_node, const AxisAlignedBoundingBox &box, Node* const RESTRICT node) NOEXCEPT
	{
		//Decide on which axis the box should be split.
		AxisAlignedBoundingBox first;
		AxisAlignedBoundingBox second;

		{
			const float x_axis{ box._Maximum._X - box._Minimum._X };
			const float y_axis{ box._Maximum._Y - box._Minimum._Y };
			const float z_axis{ box._Maximum._Z - box._Minimum._Z };

			if (x_axis > y_axis&& x_axis > z_axis) //X axis is the longest.
			{
				first._Minimum = Vector3<float>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float>(box._Minimum._X + (x_axis * 0.5f), box._Maximum._Y, box._Maximum._Z);

				second._Minimum = Vector3<float>(box._Minimum._X + (x_axis * 0.5f), box._Minimum._Y, box._Minimum._Z);
				second._Maximum = Vector3<float>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}

			else if (y_axis > z_axis) //Y axis is the longest.
			{
				first._Minimum = Vector3<float>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float>(box._Maximum._X, box._Minimum._Y + (y_axis * 0.5f), box._Maximum._Z);

				second._Minimum = Vector3<float>(box._Minimum._X, box._Minimum._Y + (y_axis * 0.5f), box._Minimum._Z);
				second._Maximum = Vector3<float>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}

			else //Z axis is the longest.
			{
				first._Minimum = Vector3<float>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z);
				first._Maximum = Vector3<float>(box._Maximum._X, box._Maximum._Y, box._Minimum._Z + (z_axis * 0.5f));

				second._Minimum = Vector3<float>(box._Minimum._X, box._Minimum._Y, box._Minimum._Z + (z_axis * 0.5f));
				second._Maximum = Vector3<float>(box._Maximum._X, box._Maximum._Y, box._Maximum._Z);
			}
		}

		//Allocate the two new nodes.
		Node *const RESTRICT nodes{ static_cast<Node* const RESTRICT>(Memory::Allocate(sizeof(Node) * 2)) };

		//Initialize the new nodes.
		Memory::Set(nodes, 0, sizeof(Node) * 2);

		//Update the number of bytes needed for nodes.
		_BuildData->_NumberOfBytesNeededForNodes += sizeof(Node) * 2;

		//Set initial properties of the two new nodes.
		nodes[0]._HasTriangles = true;
		nodes[1]._HasTriangles = true;

		//Preallocate for the worst case to avoid many rellocations later.
		nodes[0]._TriangleData.Reserve(node->_TriangleData.Size());
		nodes[1]._TriangleData.Reserve(node->_TriangleData.Size());

		//Start filling up the two new nodes with the triangle data.
		for (const TriangleData &triangle_data : node->_TriangleData)
		{
			//Cache the triangle.
			Triangle triangle;

			triangle._Vertices[0] = _BuildData->_VertexData[triangle_data._Indices[0]]._Position;
			triangle._Vertices[1] = _BuildData->_VertexData[triangle_data._Indices[1]]._Position;
			triangle._Vertices[2] = _BuildData->_VertexData[triangle_data._Indices[2]]._Position;

			//Calculate the center of the triangle.
			const Vector3<float> triangle_center{ Triangle::CalculateCenter(triangle) };

			//Decide which axis aligned bounding box this triangle data should go into, based on the triangle center.
			if (first.IsInside(triangle_center))
			{
				//Add the triangle data.
				nodes[0]._TriangleData.Emplace(triangle_data);
			}

			else if (second.IsInside(triangle_center))
			{
				//Add the triangle data.
				nodes[1]._TriangleData.Emplace(triangle_data);
			}

			else
			{
				if (nodes[0]._TriangleData.Size() < nodes[1]._TriangleData.Size())
				{
					//Add the triangle data.
					nodes[0]._TriangleData.Emplace(triangle_data);
				}

				else
				{
					//Add the triangle data.
					nodes[1]._TriangleData.Emplace(triangle_data);
				}
			}
		}

		//Shrink the allocated size to fit.
		nodes[0]._TriangleData.Reserve(nodes[0]._TriangleData.Size());
		nodes[1]._TriangleData.Reserve(nodes[1]._TriangleData.Size());

		//Destroy the original nodes triangles.
		node->_HasTriangles = false;
		node->_TriangleData.~DynamicArray<TriangleData>();

		//Add the new nodes.
		node->_AxisAlignedBoundingBoxes[0] = first;
		node->_AxisAlignedBoundingBoxes[1] = second;

		node->_Nodes = nodes;

		//Check if either of the two new nodes need to be split.
		if (node->_Nodes[0]._TriangleData.Size() > maximum_triangles_per_node)
		{
			_BuildData->_NodesToBeSplit.Emplace(&nodes[0], node->_AxisAlignedBoundingBoxes[0]);
		}

		if (node->_Nodes[1]._TriangleData.Size() > maximum_triangles_per_node)
		{
			_BuildData->_NodesToBeSplit.Emplace(&nodes[1], node->_AxisAlignedBoundingBoxes[1]);
		}
	}

	/*
	*	Copies a node´s data to the acceleration structure memory.
	*/
	FORCE_INLINE void CopyNodeToAccelerationStructureMemory(Node *const RESTRICT node) NOEXCEPT
	{
		byte *RESTRICT acceleration_structure_memory{ static_cast<byte *RESTRICT>(_AccelerationStructureMemory) + _BuildData->_AccelerationStructureMemoryOffset };

		if (node->_HasTriangles)
		{
			//Copy over the old triangle data to the new triangle data memory.
			const uint64 triangle_data_size{ node->_TriangleData.Size() };
			Memory::Copy(acceleration_structure_memory, node->_TriangleData.Data(), triangle_data_size * sizeof(TriangleData));

			node->_TriangleData.Destroy();

			node->_TriangleDataMemory = static_cast<TriangleData *RESTRICT>(static_cast<void *RESTRICT>(acceleration_structure_memory));
			node->_TriangleDataSize = triangle_data_size;

			_BuildData->_AccelerationStructureMemoryOffset += triangle_data_size * sizeof(TriangleData);
		}

		else
		{
			//Copy over the new node data to the new node data memory.
			Memory::Copy(acceleration_structure_memory, node->_Nodes, sizeof(Node) * 2);

			Memory::Free(node->_Nodes);

			node->_Nodes = static_cast<Node *RESTRICT>(static_cast<void * RESTRICT>(acceleration_structure_memory));

			_BuildData->_AccelerationStructureMemoryOffset += sizeof(Node) * 2;
		}
	}

	/*
	*	Refit the node´s axis aligned bounding box(es).
	*/
	FORCE_INLINE void Refit(Node* const RESTRICT node, AxisAlignedBoundingBox *const RESTRICT box) NOEXCEPT
	{
		CopyNodeToAccelerationStructureMemory(node);

		if (node->_HasTriangles)
		{
			box->Invalidate();

			for (uint64 i{ 0 }; i < node->_TriangleDataSize; ++i)
			{
				for (uint8 j{ 0 }; j < 3; ++j)
				{
					box->Expand(_VertexDataMemory[node->_TriangleDataMemory[i]._Indices[j]]._Position);
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
	FORCE_INLINE RESTRICTED NO_DISCARD const TriangleData *const RESTRICT TraceSurface(const Ray& ray, const Node &node, float *const RESTRICT intersection_distance) const NOEXCEPT
	{
		const TriangleData *RESTRICT intersected_triangle_data{ nullptr };

		if (node._HasTriangles)
		{
			for (uint64 i{ 0 }; i < node._TriangleDataSize; ++i)
			{
				const TriangleData& triangle_data{ node._TriangleDataMemory[i] };

				float intersection_distance_temporary{ FLOAT_MAXIMUM };

				//Cache the triangle.
				Triangle triangle;

				triangle._Vertices[0] = _VertexDataMemory[triangle_data._Indices[0]]._Position;
				triangle._Vertices[1] = _VertexDataMemory[triangle_data._Indices[1]]._Position;
				triangle._Vertices[2] = _VertexDataMemory[triangle_data._Indices[2]]._Position;

				if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &intersection_distance_temporary)
					&& intersection_distance_temporary < *intersection_distance)
				{
					intersected_triangle_data = &triangle_data;
					*intersection_distance = intersection_distance_temporary;
				}
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				float intersection_distance_temporary{ FLOAT_MAXIMUM };

				if (node._AxisAlignedBoundingBoxes[i].IsValid()
					&& CatalystGeometryMath::RayBoxIntersection(ray, node._AxisAlignedBoundingBoxes[i], &intersection_distance_temporary)
					&& intersection_distance_temporary < *intersection_distance)
				{
					if (const TriangleData* const RESTRICT intersected_triangle_data_temporary{ TraceSurface(ray, node._Nodes[i], intersection_distance) })
					{
						intersected_triangle_data = intersected_triangle_data_temporary;
					}
				}
			}
		}

		return intersected_triangle_data;
	}

	/*
	*	Traces a shadow ray through the acceleration structure.
	*	If an intersection is detected, it returns true
	*	If no interseaction was found, it returns false.
	*/
	FORCE_INLINE NO_DISCARD bool TraceShadow(const Ray &ray, const Node &node) const NOEXCEPT
	{
		if (node._HasTriangles)
		{
			for (uint64 i{ 0 }; i < node._TriangleDataSize; ++i)
			{
				const TriangleData& triangle_data{ node._TriangleDataMemory[i] };

				float intersection_distance_temporary{ FLOAT_MAXIMUM };

				//Cache the triangle.
				Triangle triangle;

				triangle._Vertices[0] = _VertexDataMemory[triangle_data._Indices[0]]._Position;
				triangle._Vertices[1] = _VertexDataMemory[triangle_data._Indices[1]]._Position;
				triangle._Vertices[2] = _VertexDataMemory[triangle_data._Indices[2]]._Position;

				if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &intersection_distance_temporary))
				{
					return true;
				}
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				float intersection_distance_temporary{ FLOAT_MAXIMUM };

				if (node._AxisAlignedBoundingBoxes[i].IsValid()
					&& CatalystGeometryMath::RayBoxIntersection(ray, node._AxisAlignedBoundingBoxes[i], &intersection_distance_temporary))
				{
					if (TraceShadow(ray, node._Nodes[i]))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

};