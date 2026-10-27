#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Path tracing.
#include <PathTracing/Primitive.h>

namespace PathTracing
{

	/*
	*	Acceleration structure class.
	*	Holds all of the primitives of the scene, and accelerates tracing against them.
	*/
	class AccelerationStructure final
	{

	public:

		/*
		*	Adds a primitive.
		*/
		void AddPrimitive(const PathTracing::Primitive *const RESTRICT primitive) NOEXCEPT;

		/*
		*	Builds this acceleration structure.
		*/
		void Build() NOEXCEPT;

		/*
		*	Traces a surface ray against this acceleration structure.
		*	Returns if it hits, and if so, fills in the surface information.
		*/
		NO_DISCARD bool TraceSurface(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT;
		
		/*
		*	Traces an occlusion ray against this acceleration structure.
		*	Returns if it hits.
		*/
		NO_DISCARD bool TraceOcclusion(const Ray &ray, const float32 maximum_distance) const NOEXCEPT;

	private:

		/*
		*	Build state class definition.
		*/
		class BuildState final
		{

		public:

			//The nodes to split.
			DynamicArray<uint64> _NodesToSplit;

		};

		/*
		*	Node class definition.
		*/
		class Node final
		{

		public:

			//The bounding box of this node.
			AxisAlignedBoundingBox3D _BoundingBox;

			//The child nodes of this node.
			StaticArray<uint64, 2> _ChildNodes{ UINT64_MAXIMUM, UINT64_MAXIMUM };

			//The primitives.
			DynamicArray<const PathTracing::Primitive *RESTRICT> _Primitives;

		};

		//The primitives.
		DynamicArray<const PathTracing::Primitive *RESTRICT> _Primitives;

		//The build state.
		BuildState _BuildState;

		//The nodes.
		DynamicArray<Node> _Nodes;

		/*
		*	Splits the given node.
		*/
		void SplitNode(const uint64 node_index) NOEXCEPT;

		/*
		*	Traces a surface ray against this node.
		*	Returns if it hits, and if so, fills in the surface information.
		*/
		NO_DISCARD bool TraceNodeSurface
		(
			const Node *const RESTRICT node,
			const Ray &ray,
			PathTracing::SurfaceInformation *const RESTRICT surface_information
		) const NOEXCEPT;

		/*
		*	Traces an occlusion ray against this node.
		*	Returns if it hits.
		*/
		NO_DISCARD bool TraceNodeOcclusion
		(
			const Node *const RESTRICT node,
			const Ray &ray,
			const float32 maximum_distance
		) const NOEXCEPT;

	};
}