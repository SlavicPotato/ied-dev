#include "pch.h"

#include "UIEffectShaderDepthModeSelector.h"

namespace IED
{
	namespace UI
	{
		UIEffectShaderDepthModeSelector::data_type UIEffectShaderDepthModeSelector::m_data{ {

			{ DepthStencilDepthMode::kDisabled, "Disabled" },
			{ DepthStencilDepthMode::kTest, "Test" },
			{ DepthStencilDepthMode::kWrite, "Write" },
			{ DepthStencilDepthMode::kTestWrite, "TestWrite" },
			{ DepthStencilDepthMode::kTestEqual, "TestEqual" },
			{ DepthStencilDepthMode::kTestGreaterEqual, "TestGreaterEqual" },
			{ DepthStencilDepthMode::kTestGreater, "TestGreater" },

		} };

		UIEffectShaderDepthModeSelector::UIEffectShaderDepthModeSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIEffectShaderDepthModeSelector::DrawDepthModeSelector(
			Localization::StringID a_label,
			DepthStencilDepthMode& a_data)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(a_label, "dsm_sel"),
					depth_mode_to_desc(a_data),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : m_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_data);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							e,
							selected))
					{
						a_data = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIEffectShaderDepthModeSelector::depth_mode_to_desc(
			DepthStencilDepthMode a_data) const
		{
			switch (a_data)
			{
			case DepthStencilDepthMode::kDisabled:
				return "Disabled";
			case DepthStencilDepthMode::kTest:
				return "Test";
			case DepthStencilDepthMode::kWrite:
				return "Write";
			case DepthStencilDepthMode::kTestWrite:
				return "TestWrite";
			case DepthStencilDepthMode::kTestEqual:
				return "TestEqual";
			case DepthStencilDepthMode::kTestGreaterEqual:
				return "TestGreaterEqual";
			case DepthStencilDepthMode::kTestGreater:
				return "TestGreater";
			default:
				return nullptr;
			}
		}
	}
}