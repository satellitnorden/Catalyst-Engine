#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Any.h>
#include <Core/General/DynamicString.h>
#include <Core/General/Pair.h>

//Math.
#include <Math/General/Vector.h>

class NodeEditor final
{

public:

	//Constants.
	constexpr static uint64 IMPLEMENTATION_SIZE{ 8 };

	//Enumeration covering all pin types.
	enum class PinType : uint8
	{
		INPUT,
		OUTPUT
	};

	//Forward declarations.
	class Node;
	class Pin;

	/*
	*	Node class definition.
	*/
	class Node
	{

	public:

		//The editor.
		NodeEditor *RESTRICT _Editor;

		//The name.
		DynamicString _Name;

		//The unique ID.
		uint64 _UniqueID;

		//The input pins.
		DynamicArray<Pin *RESTRICT> _InputPins;

		//The output pins.
		DynamicArray<Pin *RESTRICT> _OutputPins;

		//Denotes whether or not this node is selected.
		bool _IsSelected;

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Node() NOEXCEPT
		{

		}

		/*
		*	Initializes this node.
		*/
		FORCE_INLINE virtual void Initialize() NOEXCEPT
		{

		}

		/*
		*	Pre updates this node.
		*/
		FORCE_INLINE virtual void PreUpdate() NOEXCEPT
		{

		}

		/*
		*	Updates this node.
		*/
		FORCE_INLINE virtual void Update() NOEXCEPT
		{

		}

		/*
		*	Post updates this node.
		*/
		FORCE_INLINE virtual void PostUpdate() NOEXCEPT
		{

		}

		/*
		*	Returns the number of input pins.
		*/
		FORCE_INLINE NO_DISCARD uint64 NumberOfInputPins() const NOEXCEPT
		{
			return _InputPins.Size();
		}

		/*
		*	Returns the input pin at the given index.
		*/
		FORCE_INLINE NO_DISCARD Pin *const RESTRICT InputPin(const uint64 index) NOEXCEPT
		{
			return _InputPins[index];
		}

		/*
		*	Returns the number of output pins.
		*/
		FORCE_INLINE NO_DISCARD uint64 NumberOfOutputPins() const NOEXCEPT
		{
			return _OutputPins.Size();
		}

		/*
		*	Returns the output pin at the given index.
		*/
		FORCE_INLINE NO_DISCARD Pin *const RESTRICT OutputPin(const uint64 index) NOEXCEPT
		{
			return _OutputPins[index];
		}

	protected:

		/*
		*	Adds an input pin.
		*/
		FORCE_INLINE void AddInputPin(Pin *const RESTRICT pin) NOEXCEPT
		{
			//Generate the unique ID.
			pin->_UniqueID = _Editor->GenerateUniqueID();

			//Set the node.
			pin->_Node = this;

			//Set the pin type.
			pin->_PinType = PinType::INPUT;

			//Add the input pin.
			_InputPins.Emplace(pin);
		}

		/*
		*	Adds an output pin.
		*/
		FORCE_INLINE void AddOutputPin(Pin* const RESTRICT pin) NOEXCEPT
		{
			//Generate the unique ID.
			pin->_UniqueID = _Editor->GenerateUniqueID();

			//Set the node.
			pin->_Node = this;

			//Set the pin type.
			pin->_PinType = PinType::OUTPUT;

			//Add the output pin.
			_OutputPins.Emplace(pin);
		}

	};

	/*
	*	Pin class definition.
	*/
	class Pin
	{

	public:

		//The name.
		DynamicString _Name;

		//The unique ID.
		uint64 _UniqueID;

		//The node.
		Node *RESTRICT _Node;

		//The pin type.
		PinType _PinType;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Pin(const char *const RESTRICT name) NOEXCEPT
			:
			_Name(name)
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Pin() NOEXCEPT
		{

		}

	};

	/*
	*	Link class definition.
	*/
	class Link final
	{

	public:

		//The link ID.
		uint64 _LinkID;

		//The start pin ID.
		uint64 _StartPinID;

		//The end pin ID.
		uint64 _EndPinID;

	};

	/*
	*	Initializes this node editor.
	*/
	void Initialize(const char *const RESTRICT name) NOEXCEPT;

	/*
	*	Terminates this node editor.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Generates a unique ID.
	*/
	FORCE_INLINE NO_DISCARD uint64 GenerateUniqueID() NOEXCEPT
	{
		return _IDCounter++;
	}

	/*
	*	Adds a node.
	*/
	void AddNode(Node *const RESTRICT node, const Vector2<float32> position = Vector2<float32>(0.0f, 0.0f)) NOEXCEPT;

	/*
	*	Adds a link between the given pins.
	*/
	void AddLink(Pin *const RESTRICT start_pin, Pin *const RESTRICT end_pin) NOEXCEPT;

	/*
	*	Updates this node editor.
	*/
	void Update() NOEXCEPT;

	/*
	*	Returns the number of nodes.
	*/
	FORCE_INLINE NO_DISCARD uint64 NumberOfNodes() const NOEXCEPT
	{
		return _Nodes.Size();
	}

	/*
	*	Returns the node at the given index.
	*/
	FORCE_INLINE NO_DISCARD Node *const RESTRICT NodeAt(const uint64 index) NOEXCEPT
	{
		return _Nodes[index];
	}

	/*
	*	Returns the links for the given node.
	*/
	void LinksForNode(Node *const RESTRICT node, DynamicArray<Pair<Pin *RESTRICT, Pin *RESTRICT>> *const RESTRICT links) NOEXCEPT;

	/*
	*	Returns the position of the given node.
	*/
	NO_DISCARD Vector2<float32> NodePosition(Node *const RESTRICT node) NOEXCEPT;

private:

	//The name.
	DynamicString _Name;

	//The ID counter.
	uint64 _IDCounter{ 1 };

	//The nodes.
	DynamicArray<Node *RESTRICT> _Nodes;

	//The links.
	DynamicArray<Link> _Links;

	//The implementation.
	Any<IMPLEMENTATION_SIZE> _Implementation;

	/*
	*	Finds the pin with the given ID.
	*/
	NO_DISCARD Pin *const RESTRICT FindPin(const uint64 ID) NOEXCEPT;

	/*
	*	Returns whether or not a link between the given pins should be accepted.
	*/
	NO_DISCARD bool ShouldAcceptLink(const Pin *const RESTRICT start_pin, const Pin *const RESTRICT end_pin) NOEXCEPT;

};
#endif