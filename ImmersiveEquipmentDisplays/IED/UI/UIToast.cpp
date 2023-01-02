#include "pch.h"

#include "UIToast.h"

#include "UICommon.h"
#include "UIData.h"

#include "Drivers/UI.h"
#include "Drivers/UI/Tasks.h"

namespace IED
{
	namespace UI
	{
		UIToast::UIToast(
			Tasks::UIRenderTaskBase& a_owner) :
			m_owner(a_owner)
		{
		}

		void UIToast::Draw()
		{
			const stl::lock_guard lock(m_lock);

			const auto& io = ImGui::GetIO();

			if (!m_queue.empty())
			{
				auto& front = m_queue.front();

				const auto m = static_cast<float>(
					std::min(
						m_queue.size(),
						decltype(m_queue)::size_type(4)));

				front.lifetime -= io.DeltaTime * m;

				if (front.lifetime <= 0.0f)
				{
					m_queue.pop();
				}
			}

			if (m_queue.empty())
			{
				m_currentTag.reset();
				SetOpenState(false);
				return;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.75f });
			ImGui::PushStyleColor(ImGuiCol_Border, { 0.5f, 0.5f, 0.5f, 0.5f });
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, { 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);

			const auto& front = m_queue.front();

			if (front.tag != m_currentTag)
			{
				m_currentTag.emplace(front.tag);
				m_animbg.reset();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_animbg.lerp());

			constexpr auto VERTICAL_OFFSET = 40.f;

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f,
			      io.DisplaySize.y - std::clamp(VERTICAL_OFFSET, 0.0f, io.DisplaySize.y * 0.5f) },
				ImGuiCond_Always,
				{ 0.5f, 1.0f });

			constexpr auto WINDOW_FLAGS =
				ImGuiWindowFlags_Tooltip |  // internal, could cause undefined behavior
				ImGuiWindowFlags_NoInputs |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoSavedSettings;

			if (ImGui::Begin(
					"###ied_toast",
					nullptr,
					WINDOW_FLAGS))
			{
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

				if (front.color)
				{
					ImGui::TextColored(*front.color, "%s", front.text.c_str());
				}
				else
				{
					ImGui::TextUnformatted(front.text.c_str());
				}

				ImGui::PopTextWrapPos();
			}

			ImGui::End();

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
	}
}