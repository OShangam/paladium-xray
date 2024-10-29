#pragma once

#include "imgui/imgui.h"

namespace imtheme
{
	void init_theme()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 1.0f;
		style.WindowPadding = ImVec2(12.0f, 12.0f);
		style.WindowRounding = 0.1f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(20.0f, 20.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.ChildRounding = 0.1f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 3.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(10.0f, 10.0f);
		style.FrameRounding = 10.f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(5, 10);
		style.ItemInnerSpacing = ImVec2(7.099999904632568f, 1.799999952316284f);
		style.CellPadding = ImVec2(12.10000038146973f, 9.199999809265137f);
		style.IndentSpacing = 0.0f;
		style.ColumnsMinSpacing = 4.900000095367432f;
		style.ScrollbarSize = 11.60000038146973f;
		style.ScrollbarRounding = 15.89999961853027f;
		style.GrabMinSize = 3.700000047683716f;
		style.GrabRounding = 0.f;
		style.TabRounding = 5.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.25f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.3605149984359741f);
		style.Colors[ImGuiCol_WindowBg] = ImColor(28, 28, 28);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);

        style.Colors[ImGuiCol_Border] = ImColor(51, 51, 51);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.10f, 0.10f, 0.15f, 0.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(ImColor(0.9f, 0.24f, 0.24f));

        style.Colors[ImGuiCol_SliderGrab] = ImColor(48, 122, 229);
        style.Colors[ImGuiCol_SliderGrabActive] = ImColor(48, 122, 229);

		style.Colors[ImGuiCol_Button] = ImColor(48, 122, 229);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(48, 122, 229);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(48, 122, 229);

		style.Colors[ImGuiCol_Header] = ImColor(0.33f, 0.33f, 0.33f);             // Couleur de base
		style.Colors[ImGuiCol_HeaderHovered] = ImColor(0.33f, 0.33f, 0.33f);     // Survol
		style.Colors[ImGuiCol_HeaderActive] = ImColor(0.33f, 0.33f, 0.33f);      // Actif

        style.Colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);

        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.05f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);

        style.Colors[ImGuiCol_WindowBg] = ImColor(28, 28, 28);
        style.Colors[ImGuiCol_TitleBgActive] = ImColor(20, 20, 20);
        style.Colors[ImGuiCol_FrameBg] = ImColor(30, 30, 30);
        style.Colors[ImGuiCol_FrameBgHovered] = ImColor(45, 45, 45);
        style.Colors[ImGuiCol_FrameBgActive] = ImColor(23, 23, 23);
        style.Colors[ImGuiCol_ChildBg] = ImColor(23, 23, 23);
	}
}