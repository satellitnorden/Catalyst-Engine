//Header file.
#include <PathTracing/AccelerationStructure.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//STD.
#include <algorithm>

namespace PathTracing
{

	/*
	*	Adds a primitive.
	*/
	void AccelerationStructure::AddPrimitive(const PathTracing::Primitive* const RESTRICT primitive) NOEXCEPT
	{
		//Add the primitive.
		_Primitives.Emplace(primitive);
	}

	/*
	*	Builds this acceleration structure.
	*/
	void AccelerationStructure::Build() NOEXCEPT
	{
		//Set up the first node.
		_Nodes.Emplace();
		Node &node{ _Nodes.Back() };

		//Calculate the bounding box.
		for (const PathTracing::Primitive *const RESTRICT primitive : _Primitives)
		{
			node._BoundingBox.Expand(primitive->BoundingBox());
		}

		//Add all of the primitives to this node.
		node._Primitives.Reserve(_Primitives.Size());

		for (const PathTracing::Primitive *const RESTRICT primitive : _Primitives)
		{
			node._Primitives.Emplace(primitive);
		}

		//Add the node to split.
		_BuildState._NodesToSplit.Emplace(0);

		//Split nodes until there's none left.
		DynamicArray<uint64> nodes_to_split;

		while (!_BuildState._NodesToSplit.Empty())
		{
			nodes_to_split = _BuildState._NodesToSplit;
			_BuildState._NodesToSplit.Clear();

			for (const uint64 node_to_split : nodes_to_split)
			{
				SplitNode(node_to_split);
			}
		}
	}

	/*
	*	Traces a surface ray against this acceleration structure.
	*	Returns if it hits, and if so, fills in the surface information.
	*/
	NO_DISCARD bool AccelerationStructure::TraceSurface(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT
	{
		//Trace against the root node.
		return TraceNodeSurface(&_Nodes[0], ray, surface_information);
	}

	/*
	*	Traces an occlusion ray against this acceleration structure.
	*	Returns if it hits.
	*/
	NO_DISCARD bool AccelerationStructure::TraceOcclusion(const Ray &ray, const float32 maximum_distance) const NOEXCEPT
	{
		//Trace against the root node.
		return TraceNodeOcclusion(&_Nodes[0], ray, maximum_distance);
	}

	/*
	*	Splits the given node.
	*/
	void AccelerationStructure::SplitNode(const uint64 node_index) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 MAXIMUM_NUMBER_OF_PRIMITIVES_PER_NODE{ 4 };

		//No need to split if this node has below (or equal to) the maximum number of primitives per node.
		if (_Nodes[node_index]._Primitives.Size() <= MAXIMUM_NUMBER_OF_PRIMITIVES_PER_NODE)
		{
			return;
		}

		//Add the two new nodes.
		_Nodes[node_index]._ChildNodes[0] = _Nodes.Size();
		_Nodes.Emplace();

		_Nodes[node_index]._ChildNodes[1] = _Nodes.Size();
		_Nodes.Emplace();

		//Calculate the split axis.
		uint32 split_axis{ 0 };
		{
			const Vector3<float32> dimensions{ _Nodes[node_index]._BoundingBox.Dimensions() };

			if (dimensions._X > dimensions._Y && dimensions._X > dimensions._Z)
			{
				split_axis = 0;
			}

			else if (dimensions._Y > dimensions._Z)
			{
				split_axis = 1;
			}

			else
			{
				split_axis = 2;
			}
		}

		//Calculate the median index.
		const uint64 median_index{ _Nodes[node_index]._Primitives.Size() / 2 };

		//Sort the primitives around the median.
		std::nth_element
		(
			_Nodes[node_index]._Primitives.Begin(),
			_Nodes[node_index]._Primitives.Begin() + median_index,
			_Nodes[node_index]._Primitives.End(),
			[&](const PathTracing::Primitive *const RESTRICT A, const PathTracing::Primitive *const RESTRICT B)
			{
				return A->Center()[split_axis] < B->Center()[split_axis];
			}
		);

		//Assign the primitives to the child nodes.
		_Nodes[_Nodes[node_index]._ChildNodes[0]]._Primitives.Reserve(median_index);
		_Nodes[_Nodes[node_index]._ChildNodes[1]]._Primitives.Reserve(_Nodes[node_index]._Primitives.Size() - median_index);

		for (uint64 primitive_index{ 0 }; primitive_index < _Nodes[node_index]._Primitives.Size(); ++primitive_index)
		{
			if (primitive_index < median_index)
			{
				_Nodes[_Nodes[node_index]._ChildNodes[0]]._Primitives.Emplace(_Nodes[node_index]._Primitives[primitive_index]);
			}

			else
			{
				_Nodes[_Nodes[node_index]._ChildNodes[1]]._Primitives.Emplace(_Nodes[node_index]._Primitives[primitive_index]);
			}
		}

		//Destroy the original node's primitives, no longer needed.
		_Nodes[node_index]._Primitives.Destroy();

		//Shrink the bounding boxes of each child node to more closely encapsulate the combined bounding box of the primitives.
		for (uint32 child_index{ 0 }; child_index < 2; ++child_index)
		{
			Node *const RESTRICT child_node{ child_index == 0 ? &_Nodes[_Nodes[node_index]._ChildNodes[0]] : &_Nodes[_Nodes[node_index]._ChildNodes[1]] };

			child_node->_BoundingBox.Invalidate();

			for (const PathTracing::Primitive *const RESTRICT primitive : child_node->_Primitives)
			{
				child_node->_BoundingBox.Expand(primitive->BoundingBox());
			}
		}

		//Split each child node.
		_BuildState._NodesToSplit.Emplace(_Nodes[node_index]._ChildNodes[0]);
		_BuildState._NodesToSplit.Emplace(_Nodes[node_index]._ChildNodes[1]);
	}

	/*
	*	Traces a surface ray against this node.
	*	Returns if it hits, and if so, fills in the surface information.
	*/
	NO_DISCARD bool AccelerationStructure::TraceNodeSurface
	(
		const Node *const RESTRICT node,
		const Ray &ray,
		PathTracing::SurfaceInformation *const RESTRICT surface_information
	) const NOEXCEPT
	{
		//Remember if there was a hit.
		bool found_hit{ false };

		//First trace against this node's bounding box.
		float32 bounding_box_hit_distance{ FLOAT32_MAXIMUM };

		if (CatalystGeometryMath::RayBoxIntersection(ray, node->_BoundingBox, &bounding_box_hit_distance) && bounding_box_hit_distance < surface_information->_HitDistance)
		{
			//If this node has primitives, trace against them.
			if (!node->_Primitives.Empty())
			{
				for (const PathTracing::Primitive *const RESTRICT primitive : node->_Primitives)
				{
					if (primitive->TraceSurface(ray, surface_information))
					{
						found_hit = true;
					}
				}
			}

			//Otherwise, trace against the child nodes.
			else
			{
				for (const uint64 child_node_index : node->_ChildNodes)
				{
					if (TraceNodeSurface(&_Nodes[child_node_index], ray, surface_information))
					{
						found_hit = true;
					}
				}
			}
		}

		//Return if there was a hit.
		return found_hit;
	}

	/*
	*	Traces an occlusion ray against this node.
	*	Returns if it hits.
	*/
	NO_DISCARD bool AccelerationStructure::TraceNodeOcclusion
	(
		const Node *const RESTRICT node,
		const Ray &ray,
		const float32 maximum_distance
	) const NOEXCEPT
	{
		//First trace against this node's bounding box.
		float32 hit_distance{ FLOAT32_MAXIMUM };

		if (CatalystGeometryMath::RayBoxIntersection(ray, node->_BoundingBox, &hit_distance) && hit_distance < maximum_distance)
		{
			//If this node has primitives, trace against them.
			if (!node->_Primitives.Empty())
			{
				for (const PathTracing::Primitive *const RESTRICT primitive : node->_Primitives)
				{
					if (primitive->TraceOcclusion(ray, maximum_distance))
					{
						return true;
					}
				}
			}

			//Otherwise, trace against the child nodes.
			else
			{
				for (const uint64 child_node_index : node->_ChildNodes)
				{
					if (TraceNodeOcclusion(&_Nodes[child_node_index], ray, maximum_distance))
					{
						return true;
					}
				}
			}
		}

		//No hit.
		return false;
	}

}