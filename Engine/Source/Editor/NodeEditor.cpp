#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/NodeEditor.h>

//Systems.
#include <Systems/InputSystem.h>

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
	//Set the context.
	ax::NodeEditor::SetCurrentEditor(IMPLEMENTATION()._Context);

	//Check deletion of nodes/links.
	if (InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE)->GetButtonState(KeyboardButton::Delete) == ButtonState::PRESSED)
	{
		{
			StaticArray<ax::NodeEditor::NodeId, 64> selected_nodes;

			const int32 number_of_selected_nodes{ ax::NodeEditor::GetSelectedNodes(selected_nodes.Data(), 64) };

			for (int32 selected_node_index{ 0 }; selected_node_index < number_of_selected_nodes; ++selected_node_index)
			{
				const uint64 selected_node_id{ selected_nodes[selected_node_index].Get() };

				for (uint64 node_index{ 0 }; node_index < _Nodes.Size(); ++node_index)
				{
					if (_Nodes[node_index]->_UniqueID == selected_node_id)
					{
						ax::NodeEditor::DeleteNode(selected_node_id);
					
						_Nodes.EraseAt<false>(node_index);

						break;
					}
				}
			}
		}

		{
			StaticArray<ax::NodeEditor::LinkId, 64> selected_links;

			const int32 number_of_selected_links{ ax::NodeEditor::GetSelectedLinks(selected_links.Data(), 64) };

			for (int32 selected_link_index{ 0 }; selected_link_index < number_of_selected_links; ++selected_link_index)
			{
				const uint64 selected_link_id{ selected_links[selected_link_index].Get() };

				for (uint64 link_index{ 0 }; link_index < _Links.Size(); ++link_index)
				{
					if (_Links[link_index]._LinkID == selected_link_id)
					{
						ax::NodeEditor::DeleteLink(selected_link_id);

						_Links.EraseAt<false>(link_index);

						break;
					}
				}
			}
		}
	}

	//Pre update all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		node->PreUpdate();
	}

	//Begin the editor.
	ax::NodeEditor::Begin(_Name.Data());

	//Add all nodes.
	for (Node *const RESTRICT node : _Nodes)
	{
		//Begin the node.
		ax::NodeEditor::BeginNode(node->_UniqueID);

		//Draw then name.
		ImGui::Text(node->_Name.Data());

		//Draw input pins.
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotAlignment, ImVec2(0.0f, 0.5f));
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotSize, ImVec2(0.0f, 0.0f));

		const ImVec2 input_cursor{ ImGui::GetCursorPos() };
		float32 furthest_input{ -FLOAT32_MAXIMUM };

		for (Pin *const RESTRICT pin : node->_InputPins)
		{
			ax::NodeEditor::BeginPin(pin->_UniqueID, ax::NodeEditor::PinKind::Input);
			ImGui::Indent(16.0f);
			ImGui::Text(pin->_Name.Data());
			
			const ImVec2 item_min{ ImGui::GetItemRectMin() };
			const ImVec2 item_max{ ImGui::GetItemRectMax() };

			const ImVec2 circle_center
			{
				item_min.x - (item_max.y - item_min.y) * 0.5f,
				item_min.y + (item_max.y - item_min.y) * 0.5f
			};

			ImGui::GetWindowDrawList()->AddCircleFilled(circle_center, 4.0f, IM_COL32(255, 255, 255, 127), 0);
			ImGui::GetWindowDrawList()->AddCircle(circle_center, 4.0f, IM_COL32(255, 255, 255, 255), 0, 2.0f);

			ax::NodeEditor::PinRect(ImVec2(circle_center.x - 4.0f, item_min.y), ImVec2(item_max.x, item_max.y));

			ax::NodeEditor::EndPin();

			furthest_input = BaseMath::Maximum<float32>(furthest_input, item_max.x);
		}

		ax::NodeEditor::PopStyleVar(2);

		//Draw output pins.
		if (!node->_OutputPins.Empty())
		{
			//Set the cursor position.
			ImGui::SetCursorPos(ImVec2(furthest_input > -FLOAT32_MAXIMUM ? furthest_input + 16.0f : ImGui::GetCursorPosX(), input_cursor.y));

			//Push style vars.
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotSize, ImVec2(0.0f, 0.0f));

			for (Pin *const RESTRICT pin : node->_OutputPins)
			{
				ImGui::SetCursorPos(ImVec2(furthest_input > -FLOAT32_MAXIMUM ? furthest_input + 16.0f : ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));

				ax::NodeEditor::BeginPin(pin->_UniqueID, ax::NodeEditor::PinKind::Output);
				ImGui::Text(pin->_Name.Data());

				ImVec2 item_min{ ImGui::GetItemRectMin() };
				ImVec2 item_max{ ImGui::GetItemRectMax() };

				ImGui::SameLine();
				ImGui::Text(" ");

				const ImVec2 circle_center
				{
					item_max.x + 8.0f,
					item_min.y + (item_max.y - item_min.y) * 0.5f
				};

				ImGui::GetWindowDrawList()->AddCircleFilled(circle_center, 4.0f, IM_COL32(255, 255, 255, 127), 0);
				ImGui::GetWindowDrawList()->AddCircle(circle_center, 4.0f, IM_COL32(255, 255, 255, 255), 0, 2.0f);

				ax::NodeEditor::PinRect(ImVec2(item_min.x, item_min.y), ImVec2(circle_center.x + 4.0f, item_max.y));

				ax::NodeEditor::EndPin();
			}

			ax::NodeEditor::PopStyleVar(2);
		}

		//Update whether or not this node is seleceted.
		node->_IsSelected = ax::NodeEditor::IsNodeSelected(node->_UniqueID);

		//Update the node.
		node->Update();

		//End the node.
		ax::NodeEditor::EndNode();

		//Draw header.
		{
			ImDrawList *const RESTRICT draw_list{ ax::NodeEditor::GetNodeBackgroundDrawList(node->_UniqueID) };

			const ImVec2 node_position{ ax::NodeEditor::GetNodePosition(node->_UniqueID) };
			const ImVec2 node_size{ ax::NodeEditor::GetNodeSize(node->_UniqueID) };
			const ImVec2 node_half_size{ node_size.x * 0.5f, node_size.y * 0.5f };

			draw_list->AddRectFilled
			(
				ImVec2(node_position.x + 1.0f, node_position.y + 1.0f),
				ImVec2(node_position.x + node_size.x - 1.0f, node_position.y + ImGui::GetStyle().ItemSpacing.y * 8.0f - 1.0f),
				*node->_Color.Data(),
				ax::NodeEditor::GetStyle().NodeRounding
			);

			draw_list->AddRectFilled
			(
				ImVec2(node_position.x + 1.0f, node_position.y + ImGui::GetStyle().ItemSpacing.y * 4.0f),
				ImVec2(node_position.x + node_size.x - 1.0f, node_position.y + ImGui::GetStyle().ItemSpacing.y * 8.0f - 1.0f),
				*node->_Color.Data(),
				0.0f
			);
		}
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