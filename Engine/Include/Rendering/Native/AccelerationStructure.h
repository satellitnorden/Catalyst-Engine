#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Geometry/Ray.h>
#include <Math/Geometry/Triangle.h>

//Memory.
#include <Memory/PoolAllocator.h>

class AccelerationStructure final
{

public:

	/*
	*	Triangle data class definition.
	*/
	class TriangleData final
	{

	public:

		//The triangle.
		Triangle _Triangle;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE TriangleData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE TriangleData(const Triangle &initial_triangle) NOEXCEPT
			:
			_Triangle(initial_triangle)
		{

		}

	};

	/*
	*	Adds triangle data to the acceleration structure. Must be done before building.
	*/
	FORCE_INLINE void AddTriangleData(const TriangleData &triangle_data) NOEXCEPT
	{
		//Add the triangle data to the root.
		_Root._TriangleData.Emplace(triangle_data);
	}

	/*
	*	Builds the acceleration structure.
	*/
	FORCE_INLINE void Build(const uint64 maximum_triangles_per_node) NOEXCEPT
	{
		//First, calculate axis aligned bounding box for the root.
		AxisAlignedBoundingBox root_box;

		for (const TriangleData &triangle_data : _Root._TriangleData)
		{
			root_box.Expand(triangle_data._Triangle._Vertices[0]);
			root_box.Expand(triangle_data._Triangle._Vertices[1]);
			root_box.Expand(triangle_data._Triangle._Vertices[3]);
		}

		//If the root node has more than the maximum number of triangles, add it to the nodes to be split.
		if (_Root._TriangleData.Size() > maximum_triangles_per_node)
		{
			_NodesToBeSplit.Emplace(&_Root, root_box);
		}

		//Process all nodes to be split.
		while (!_NodesToBeSplit.Empty())
		{
			Node *const RESTRICT node{ _NodesToBeSplit.Back()._First };
			const AxisAlignedBoundingBox axis_aligned_bounding_box{ _NodesToBeSplit.Back()._Second };

			_NodesToBeSplit.PopFast();

			SplitNode(maximum_triangles_per_node, axis_aligned_bounding_box, node);
		}
	}

	/*
	*	Traces a ray through the acceleration structure.
	*	If an intersection is detected, it returns the intersected triangle data.
	*	If no interseaction was found, it returns nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TriangleData *const RESTRICT Trace(const Ray& ray, float *const RESTRICT intersection_distance) const NOEXCEPT
	{
		return Trace(ray, _Root, intersection_distance);
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

			//The triangle data.
			DynamicArray<TriangleData> _TriangleData;
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

	//The root.
	Node _Root;

	//The allocator.
	PoolAllocator<sizeof(Node) * 2> _Allocator;

	//The nodes to be split, and their axis aligned bounding boxes.
	DynamicArray<Pair<Node* const RESTRICT, AxisAlignedBoundingBox>> _NodesToBeSplit;

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

		//Set initial properties of the two new nodes.
		nodes[0]._HasTriangles = true;
		nodes[1]._HasTriangles = true;

		//Start filling up the two new nodes with the triangle data.
		for (const TriangleData &triangle_data : node->_TriangleData)
		{
			//Calculate the center of the triangle.
			const Vector3<float> triangle_center{ Triangle::CalculateCenter(triangle_data._Triangle) };

			//Decide which axis aligned bounding box this triangle data should go into, based on the triangle center.
			if (first.IsInside(triangle_center))
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
			_NodesToBeSplit.Emplace(&nodes[0], node->_AxisAlignedBoundingBoxes[0]);
		}

		if (node->_Nodes[1]._TriangleData.Size() > maximum_triangles_per_node)
		{
			_NodesToBeSplit.Emplace(&nodes[1], node->_AxisAlignedBoundingBoxes[1]);
		}
	}

	/*
	*	Traces a ray through the acceleration structure via the given node.
	*	If an intersection is detected, it returns the intersected triangle data.
	*	If no interseaction was found, it returns nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TriangleData *const RESTRICT Trace(const Ray& ray, const Node &node, float *const RESTRICT intersection_distance) const NOEXCEPT
	{
		const TriangleData *RESTRICT interscted_triangle_data{ nullptr };

		if (node._HasTriangles)
		{
			for (const TriangleData& triangle_data : node._TriangleData)
			{
				float intersection_distance_temporary{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle_data._Triangle, &intersection_distance_temporary)
					&& intersection_distance_temporary < *intersection_distance)
				{
					interscted_triangle_data = &triangle_data;
					*intersection_distance = intersection_distance_temporary;
				}
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 2; ++i)
			{
				if (const TriangleData *const RESTRICT interscted_triangle_data_temporary{ Trace(ray, node._Nodes[i], intersection_distance) })
				{
					interscted_triangle_data = interscted_triangle_data_temporary;
				}
			}
		}

		return interscted_triangle_data;
	}

};