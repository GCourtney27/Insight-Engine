#pragma once

#if defined (IE_PLATFORM_BUILD_WIN32)
#include "imgui.h"
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>
//#include "ImGuizmo.h"
#endif


#include "Insight/Math/ie_Vectors.h"

namespace Insight {

	namespace UI {

		typedef int NodeFlags;
		typedef int ColorPickerFlags;
		typedef int InputTextFieldFlags;

		enum _NodeFlags
		{
			TreeNode_DefaultOpen = 32,
			TreeNode_Leaf = 256,
			TreeNode_OpenArrow = 128,
			TreeNode_OpenDoubleClick = 64,
			TreeNode_SpanAvailWidth = 1 << 11
		};

		enum _ColorPickerFlags
		{
			ColorPickerFlags_NoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
			ColorPickerFlags_Uint8 = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
			ColorPickerFlags_PickerHueWheel = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
		};

		enum _InputTextFieldFlags
		{
			InputTextFieldFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
		};

#if defined (IE_PLATFORM_BUILD_WIN32)

		#define EDITOR_TEXTBOX(Text) ImGui::Text(Text)
		#define EDITOR_SPACING() ImGui::Spacing()
		#define EDITOR_INPUT_TEXT_FIELD(Label, TargetBuffer, Flags) ImGui::InputText(Label, &TargetBuffer, Flags)
		#define EDITOR_CHECKBOX(Label, pValue) ImGui::Checkbox(Label, pValue)
		#define EDITOR_DRAG_FLOAT(label, v, v_speed, v_min, v_max, format, power) ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, power)
		#define EDITOR_DRAG_FLOAT_2(label, v, v_speed, v_min, v_max, format, power) ImGui::DragFloat2(label, v, v_speed, v_min, v_max, format, power);
		#define EDITOR_DRAG_FLOAT_3(label, v, v_speed, v_min, v_max, format, power) ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, power);
		#define EDITOR_SLIDER_FLOAT(label, v, v_min, v_max, format, power) ImGui::SliderFloat(label, v, v_min, v_max, format, power)
		#define EDITOR_SLIDER_FLOAT_2(label, v, v_min, v_max, format, power) ImGui::SliderFloat2(label, v, v_min, v_max, format, power)
		#define EDITOR_COLOR_PICKER_3(Label, Color, Flags) ImGui::ColorEdit3(Label, Color, Flags)
		#define EDITOR_BEGIN_WINDOW(Title) ImGui::Begin(Title)
		#define EDITOR_END_WINDOW() ImGui::End()
		#define EDITOR_COMBO_BOX(Label, CurrentItem, Items, NumItems) ImGui::Combo(Label, &CurrentItem, Items, NumItems);
		#define EDITOR_NEW_LINE() ImGui::NewLine()
		#define EDITOR_COLLAPSING_HEADER(Label, Flags) ImGui::CollapsingHeader(Label, Flags)
		#define EDITOR_TREE_NODE_EX(Label, Flags) ImGui::TreeNodeEx(Label, Flags)
		#define EDITOR_POP_TREE_NODE() ImGui::TreePop()
		#define EDITOR_IS_ITEM_CLICKED() ImGui::IsItemClicked()
		#define EDITOR_SAME_LINE() ImGui::SameLine()
		#define EDITOR_PUSH_ID(ID) ImGui::PushID(ID)
		#define EDITOR_POP_ID() ImGui::PopID()
		#define EDITOR_TREE_NODE(Label) ImGui::TreeNode(Label)
		#define EDITOR_TREE_POP() ImGui::TreePop()



#elif defined (IE_PLATFORM_BUILD_UWP)
		#define EDITOR_TEXTBOX(Text)
		#define EDITOR_SPACING()
		#define EDITOR_INPUT_TEXT_FIELD(Label, TargetBuffer, Flags) false
		#define EDITOR_CHECKBOX(Label, pValue) false
		#define EDITOR_DRAG_FLOAT(label, v, v_speed, v_min, v_max, format, power) false
		#define EDITOR_DRAG_FLOAT_2(label, v, v_speed, v_min, v_max, format, power) false
		#define EDITOR_DRAG_FLOAT_3(label, v, v_speed, v_min, v_max, format, power) false
		#define EDITOR_SLIDER_FLOAT(label, v, v_min, v_max, format, power)
		#define EDITOR_SLIDER_FLOAT_2(label, v, v_min, v_max, format, power)
		#define EDITOR_COLOR_PICKER_3(Label, Color, Flags) false
		#define EDITOR_BEGIN_WINDOW(Title) 
		#define EDITOR_END_WINDOW()
		#define EDITOR_COMBO_BOX(Label, CurrentItem, Items, NumItems) false
		#define EDITOR_NEW_LINE()
		#define EDITOR_COLLAPSING_HEADER(Label, Flags) false
		#define EDITOR_TREE_NODE_EX(Label, Flags) false
		#define EDITOR_POP_TREE_NODE()
		#define EDITOR_IS_ITEM_CLICKED() false
		#define EDITOR_SAME_LINE()
		#define EDITOR_PUSH_ID(ID)
		#define EDITOR_POP_ID()
		#define EDITOR_TREE_NODE(Label) false
		#define EDITOR_TREE_POP()



#endif

		static inline void Text(const char* Text)
		{
			EDITOR_TEXTBOX(Text);
		}

		static inline bool TreeNode(const char* Label)
		{
			return EDITOR_TREE_NODE(Label);
		}

		static inline void TreePop()
		{
			EDITOR_TREE_POP();
		}

		static inline void PushID(const char* ID)
		{
			EDITOR_PUSH_ID(ID);
		}

		static inline void PopID()
		{
			EDITOR_POP_ID();
		}

		static inline void SameLine()
		{
			EDITOR_SAME_LINE();
		}

		static inline void Spacing()
		{
			EDITOR_SPACING();
		}

		static inline bool InputTextField(const char* Label, std::string& TargetBuffer, InputTextFieldFlags Flags)
		{
			return EDITOR_INPUT_TEXT_FIELD(Label, TargetBuffer, Flags);
		}

		static inline bool Checkbox(const char* Label, bool* pValue)
		{
			return EDITOR_CHECKBOX(Label, pValue);
		}

		static inline bool DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
		{
			return EDITOR_DRAG_FLOAT(label, v, v_speed, v_min, v_max, format, power);
		}

		static inline bool DragFloat2(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
		{
			return EDITOR_DRAG_FLOAT_2(label, v, v_speed, v_min, v_max, format, power);
		}

		static inline bool DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
		{
			return EDITOR_DRAG_FLOAT_3(label, v, v_speed, v_min, v_max, format, power);
		}

		static inline void SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
		{
			EDITOR_SLIDER_FLOAT(label, v, v_min, v_max, format, power);
		}

		static inline void SliderFloat2(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
		{
			EDITOR_SLIDER_FLOAT_2(label, v, v_min, v_max, format, power);
		}

		static inline bool ColorPicker3(const char* Label, float Color[3], ColorPickerFlags Flags)
		{
			return EDITOR_COLOR_PICKER_3(Label, Color, Flags);
		}

		static inline void BeginWindow(const char* Title)
		{
			EDITOR_BEGIN_WINDOW(Title);
		}

		static inline void EndWindow()
		{
			EDITOR_END_WINDOW();
		}

		static inline bool ComboBox(const char* Label, int& CurrentItem, const char*const* Items, int NumItems)
		{
			return EDITOR_COMBO_BOX(Label, CurrentItem, Items, NumItems);
		}

		static inline void NewLine()
		{
			EDITOR_NEW_LINE();
		}

		static inline bool CollapsingHeader(const char* Label, NodeFlags Flags)
		{
			return EDITOR_COLLAPSING_HEADER(Label, Flags);
		}

		static inline bool TreeNodeEx(const char* Label, NodeFlags Flags)
		{
			return EDITOR_TREE_NODE_EX(Label, Flags);
		}

		static inline void TreePopNode()
		{
			EDITOR_POP_TREE_NODE();
		}

		static inline bool IsItemClicked()
		{
			return EDITOR_IS_ITEM_CLICKED();
		}

		static void DrawVector3Control(const std::string& Label, Math::ieVector3& Values, float ResetValue = 0.0f, float ColumnWidth = 100.0f)
		{
#if defined (IE_PLATFORM_BUILD_WIN32)

			ImGui::PushID(Label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, ColumnWidth);
			ImGui::Text(Label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float LineHeight = io.Fonts->Fonts[0]->FontSize + style.FramePadding.y + 2.0f;
			ImVec2 ButtonSize = { LineHeight + 3.0f, LineHeight };


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			if (ImGui::Button("X", ButtonSize))
				Values.x = ResetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##X", &Values.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PopStyleColor(3);
			
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			if (ImGui::Button("Y", ButtonSize))
				Values.y = ResetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Y", &Values.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PopStyleColor(3);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			if (ImGui::Button("Z", ButtonSize))
				Values.z = ResetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Z", &Values.z, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PopStyleColor(3);


			ImGui::PopStyleVar();
			ImGui::Columns(1);

			ImGui::PopID();
#endif
		}
	}
}
