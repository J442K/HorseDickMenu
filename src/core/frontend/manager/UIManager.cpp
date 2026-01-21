#include "UIManager.hpp"

#include <algorithm>

namespace YimMenu
{
	void UIManager::AddSubmenuImpl(const std::shared_ptr<Submenu>&& submenu)
	{
		if (!m_ActiveSubmenu)
			m_ActiveSubmenu = submenu;

		m_Submenus.push_back(std::move(submenu));
	}

	void UIManager::SetActiveSubmenuImpl(const std::shared_ptr<Submenu> Submenu)
	{
		m_ActiveSubmenu = Submenu;
	}

	void UIManager::DrawImpl()
	{
		const float header_height = 52.0f;
		const float footer_height = 42.0f;
		const float sidebar_width = 175.0f;
		const float content_spacing = 8.0f;

		if (ImGui::BeginChild("##menu_header", ImVec2(0, header_height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			const ImVec2 pos = ImGui::GetWindowPos();
			const ImVec2 size = ImGui::GetWindowSize();
			draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(20, 25, 36, 235), 8.0f);
			draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(70, 140, 220, 255), 8.0f, 0, 1.2f);
			draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(20, 60, 120, 160), 8.0f, 0, 3.0f);

			ImGui::SetCursorPos(ImVec2(16.0f, 14.0f));
			ImGui::Text("ChairX Menu | Red Dead Redemption 2");

			const char* brand_label = "ChairX";
			const ImVec2 brand_size = ImGui::CalcTextSize(brand_label);
			ImGui::SetCursorPos(ImVec2(size.x - brand_size.x - 18.0f, 14.0f));
			ImGui::Text("%s", brand_label);
		}
		ImGui::EndChild();

		ImGui::Spacing();

		ImVec2 content_avail = ImGui::GetContentRegionAvail();
		const float body_height = std::max(0.0f, content_avail.y - footer_height - content_spacing);

		if (ImGui::BeginChild("##menu_body", ImVec2(0, body_height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.12f, 0.28f, 0.48f, 0.85f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.16f, 0.35f, 0.62f, 0.9f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.18f, 0.4f, 0.7f, 0.95f));

			if (ImGui::BeginChild("##submenus", ImVec2(sidebar_width, 0), true))
			{
				for (auto& submenu : m_Submenus)
				{
					if (ImGui::Selectable(submenu->m_Name.data(), (submenu == m_ActiveSubmenu), 0, ImVec2(0, 28)))
					{
						SetActiveSubmenu(submenu);
					}
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();

			if (ImGui::BeginChild("##content", ImVec2(0, 0), true))
			{
				if (ImGui::BeginChild("##minisubmenus", ImVec2(0, 48), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					if (m_ActiveSubmenu)
						m_ActiveSubmenu->DrawCategorySelectors();
				}
				ImGui::EndChild();

				ImGui::Separator();

				if (ImGui::BeginChild("##options", ImVec2(0, 0), false))
				{
					if (m_OptionsFont)
						ImGui::PushFont(m_OptionsFont);

					if (m_ActiveSubmenu)
						m_ActiveSubmenu->Draw();

					if (m_OptionsFont)
						ImGui::PopFont();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		ImGui::Spacing();

		if (ImGui::BeginChild("##menu_footer", ImVec2(0, footer_height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			const ImVec2 pos = ImGui::GetWindowPos();
			const ImVec2 size = ImGui::GetWindowSize();
			draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(18, 22, 32, 235), 8.0f);
			draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(70, 140, 220, 255), 8.0f, 0, 1.2f);

			const ImVec2 button_size(90.0f, 0.0f);
			ImGui::SetCursorPos(ImVec2(size.x - button_size.x - 18.0f, 9.0f));
			if (ImGui::Button("Close", button_size))
			{
				if (m_CloseAction)
					m_CloseAction();
			}

			ImGui::SetCursorPos(ImVec2(16.0f, 11.0f));
			ImGui::Text("Spawn Horse   Infinite Ammo   $ MAX Cash");
		}
		ImGui::EndChild();
	}

	std::shared_ptr<Submenu> UIManager::GetActiveSubmenuImpl()
	{
		if (m_ActiveSubmenu)
		{
			return m_ActiveSubmenu;
		}

		return nullptr;
	}

	std::shared_ptr<Category> UIManager::GetActiveCategoryImpl()
	{
		if (m_ActiveSubmenu)
		{
			return m_ActiveSubmenu->GetActiveCategory();
		}

		return nullptr;
	}
}
