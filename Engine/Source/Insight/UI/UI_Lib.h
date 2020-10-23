#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>
#include "ImGuizmo.h"

#include "Insight/Math/ie_Vectors.h"

namespace Insight {

	namespace UI {

		static void DrawVector3Control(const std::string& Label, ieVector3& Values, float ResetValue = 0.0f, float ColumnWidth = 100.0f)
		{
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
		}
	}
}
