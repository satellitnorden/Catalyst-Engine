#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/NodeEditor.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/imguinodeeditor/imgui_node_editor.h>

//STL.
#include <stdio.h>

//Macros.
#define IMPLEMENTATION() (*_Implementation.Get<NodeEditorImplementation>())

/*
*	Node editor implementation.
*/
class NodeEditorImplementation final
{

public:

	//The context.
	ax::NodeEditor::EditorContext *RESTRICT _Context;

};

static_assert(sizeof(NodeEditorImplementation) <= NodeEditor::IMPLEMENTATION_SIZE, "Blah!");

/*
*	Initializes this node editor.
*/
void NodeEditor::Initialize(const char *const RESTRICT name) NOEXCEPT
{
	//Set the name.
	_Name = name;

	//Create the context.
	ax::NodeEditor::Config config;

	char settings_file_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(settings_file_buffer, "%s.json", name);

	config.SettingsFile = settings_file_buffer;

	IMPLEMENTATION()._Context = ax::NodeEditor::CreateEditor(&config);
}

/*
*	Terminates this node editor.
*/
void NodeEditor::Terminate() NOEXCEPT
{
	//Destroy the context.
	ax::NodeEditor::DestroyEditor(IMPLEMENTATION()._Context);

	//Destroy all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		//Destroy all input pins.
		for (Pin *const RESTRICT pin : node->_InputPins)
		{
			delete pin;
		}

		//Destroy all output pins.
		for (Pin *const RESTRICT pin : node->_OutputPins)
		{
			delete pin;
		}

		//Destroy the node.
		delete node;
	}

	_Nodes.Clear();
}

/*
*	Adds a node.
*/
void NodeEditor::AddNode(Node *const RESTRICT node, const Vector2<float32> position) NOEXCEPT
{
	//Set the editor.
	node->_Editor = this;

	//Generate the unique ID.
	node->_UniqueID = _IDCounter++;

	//Initialize the node.
	node->Initialize();

	//Add the node.
	_Nodes.Emplace(node);

	//Set the position.
	ax::NodeEditor::SetCurrentEditor(IMPLEMENTATION()._Context);
	ax::NodeEditor::SetNodePosition(node->_UniqueID, ImVec2(position._X, position._Y));
}

/*
*	Adds a link between the given pins.
*/
void NodeEditor::AddLink(Pin *const RESTRICT start_pin, Pin *const RESTRICT end_pin) NOEXCEPT
{
	_Links.Emplace();

	_Links.Back()._LinkID = _IDCounter++;
	_Links.Back()._StartPinID = start_pin->_PinType == PinType::OUTPUT ? start_pin->_UniqueID : end_pin->_UniqueID;
	_Links.Back()._EndPinID = end_pin->_PinType == PinType::INPUT ? end_pin->_UniqueID : start_pin->_UniqueID;
}

/*
*	Updates this node editor.
*/
void NodeEditor::Update() NOEXCEPT
{
	//Pre update all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		node->PreUpdate();
	}

	//Set the context.
	ax::NodeEditor::SetCurrentEditor(IMPLEMENTATION()._Context);

	//Begin the editor.
	ax::NodeEditor::Begin(_Name.Data());

	//Add all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		ax::NodeEditor::BeginNode(node->_UniqueID);

		ImGui::Text(node->_Name.Data());

		for (Pin *const RESTRICT pin : node->_InputPins)
		{
			ax::NodeEditor::BeginPin(pin->_UniqueID, ax::NodeEditor::PinKind::Input);
			ImGui::Text(pin->_Name.Data());
			ax::NodeEditor::EndPin();
		}

		for (Pin *const RESTRICT pin : node->_OutputPins)
		{
			ax::NodeEditor::BeginPin(pin->_UniqueID, ax::NodeEditor::PinKind::Output);
			ImGui::Text(pin->_Name.Data());
			ax::NodeEditor::EndPin();
		}

		//Update whether or not this node is seleceted.
		node->_IsSelected = ax::NodeEditor::IsNodeSelected(node->_UniqueID);

		node->Update();

		ax::NodeEditor::EndNode();
	}

	//Create links.
	if (ax::NodeEditor::BeginCreate(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f))
	{
		ax::NodeEditor::PinId start_pin_id{ 0 };
		ax::NodeEditor::PinId end_pin_id{ 0 };

		if (ax::NodeEditor::QueryNewLink(&start_pin_id, &end_pin_id))
		{
			if (start_pin_id && end_pin_id)
			{
				if (ShouldAcceptLink(FindPin(start_pin_id.Get()), FindPin(end_pin_id.Get())))
				{
					if (ax::NodeEditor::AcceptNewItem(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 2.0f))
					{
						AddLink(FindPin(start_pin_id.Get()), FindPin(end_pin_id.Get()));
					}
				}
				
				else
				{
					ax::NodeEditor::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
				}
			}
		}
	}

	ax::NodeEditor::EndCreate();

	//Add the links.
	for (const Link &link : _Links)
	{
		ax::NodeEditor::Link(link._LinkID, link._StartPinID, link._EndPinID);
	}

	//End the editor.
	ax::NodeEditor::End();

	//Set the current editor.
	ax::NodeEditor::SetCurrentEditor(nullptr);
	
	//Post update all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		node->PostUpdate();
	}
}

/*
*	Returns the links for the given node.
*/
void NodeEditor::LinksForNode(Node *const RESTRICT node, DynamicArray<Pair<Pin *RESTRICT, Pin *RESTRICT>> *const RESTRICT links) NOEXCEPT
{
	links->Clear();

	for (Pin *const RESTRICT pin : node->_InputPins)
	{
		for (const Link &link : _Links)
		{
			if (pin->_UniqueID == link._StartPinID
				|| pin->_UniqueID == link._EndPinID)
			{
				links->Emplace(FindPin(link._StartPinID), FindPin(link._EndPinID));
			}
		}
	}

	for (Pin *const RESTRICT pin : node->_OutputPins)
	{
		for (const Link &link : _Links)
		{
			if (pin->_UniqueID == link._StartPinID
				|| pin->_UniqueID == link._EndPinID)
			{
				links->Emplace(FindPin(link._StartPinID), FindPin(link._EndPinID));
			}
		}
	}
}

/*
*	Returns the position of the given node.
*/
NO_DISCARD Vector2<float32> NodeEditor::NodePosition(Node *const RESTRICT node) NOEXCEPT
{
	ax::NodeEditor::SetCurrentEditor(IMPLEMENTATION()._Context);
	const ImVec2 position{ ax::NodeEditor::GetNodePosition(node->_UniqueID) };

	return Vector2<float32>(position.x, position.y);
}

/*
*	Finds the pin with the given ID.
*/
NO_DISCARD NodeEditor::Pin *const RESTRICT NodeEditor::FindPin(const uint64 ID) NOEXCEPT
{
	for (Node *const RESTRICT node : _Nodes)
	{
		for (Pin *const RESTRICT pin : node->_InputPins)
		{
			if (pin->_UniqueID == ID)
			{
				return pin;
			}
		}

		for (Pin *const RESTRICT pin : node->_OutputPins)
		{
			if (pin->_UniqueID == ID)
			{
				return pin;
			}
		}
	}

	ASSERT(false, "Could not find pin!");

	return nullptr;
}

/*
*	Returns whether or not a link between the given pins should be accepted.
*/
NO_DISCARD bool NodeEditor::ShouldAcceptLink(const Pin *const RESTRICT start_pin, const Pin *const RESTRICT end_pin) NOEXCEPT
{
	//Reject if the pins have the same node.
	if (start_pin->_Node == end_pin->_Node)
	{
		return false;
	}

	//Reject if they have the same pin type.
	if (start_pin->_PinType == end_pin->_PinType)
	{
		return false;
	}

	//All tests passed. (:
	return true;
}
#endif