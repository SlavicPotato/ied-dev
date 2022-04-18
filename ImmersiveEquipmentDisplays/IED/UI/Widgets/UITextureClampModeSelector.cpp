#include "pch.h"

#include "UITextureClampModeSelector.h"

namespace IED
{
	namespace UI
	{
		UITextureClampModeSelector::data_type UITextureClampModeSelector::m_data{ {

			{ TextureAddressMode::kClampSClampT, "ClampSClampT" },
			{ TextureAddressMode::kClampSWrapT, "ClampSWrapT" },
			{ TextureAddressMode::kWrapSClampT, "WrapSClampT" },
			{ TextureAddressMode::kWrapSWrapT, "WrapSWrapT" },

		} };

		UITextureClampModeSelector::UITextureClampModeSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UITextureClampModeSelector::DrawTextureClampModeSelector(
			Localization::StringID a_label,
			TextureAddressMode&    a_data)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(a_label, "tcm_sel"),
					texture_clamp_mode_to_desc(a_data),
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

		const char* UITextureClampModeSelector::texture_clamp_mode_to_desc(
			TextureAddressMode a_data) const
		{
			switch (a_data)
			{
			case TextureAddressMode::kClampSClampT:
				return "ClampSClampT";
			case TextureAddressMode::kClampSWrapT:
				return "ClampSWrapT";
			case TextureAddressMode::kWrapSClampT:
				return "WrapSClampT";
			case TextureAddressMode::kWrapSWrapT:
				return "WrapSWrapT";
			default:
				return nullptr;
			}
		}
	}
}