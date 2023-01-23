#include "pch.h"

#include "UIBipedObjectSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIBipedObjectSelectorWidget::data_type UIBipedObjectSelectorWidget::m_desc{ {

			"30 - Head",
			"31 - Hair",
			"32 - Body",
			"33 - Hands",
			"34 - Forearms",
			"35 - Amulet",
			"36 - Ring",
			"37 - Feet",
			"38 - Calves",
			"39 - Shield",
			"40 - Tail",
			"41 - Long hair",
			"42 - Circlet",
			"43 - Ears",
			"44 - Unnamed",
			"45 - Unnamed",
			"46 - Unnamed",
			"47 - Unnamed",
			"48 - Unnamed",
			"49 - Unnamed",
			"50 - DecapitateHead",
			"51 - Decapitate",
			"52 - Unnamed",
			"53 - Unnamed",
			"54 - Unnamed",
			"55 - Unnamed",
			"56 - Unnamed",
			"57 - Unnamed",
			"58 - Unnamed",
			"59 - Unnamed",
			"60 - Unnamed",
			"61 - FX01",
			"Hand to hand melee",
			"One hand sword",
			"One hand dagger",
			"One hand axe",
			"One hand mace",
			"Two hand melee",
			"Bow",
			"Staff",
			"Crossbow",
			"Quiver",

			"Race - Head",
			"Race - Hair",
			"Race - Shield",
			"Race - Body"

		} };

		bool UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
			const char*   a_label,
			BIPED_OBJECT& a_data,
			bool          a_allowNone)
		{
			bool result = false;

			ImGui::PushID("bo_sel");

			auto preview = GetBipedSlotDesc(a_data);

			if (!preview)
			{
				preview = "None";
			}

			if (ImGui::BeginCombo(
					a_label,
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("header");

				if (a_allowNone)
				{
					if (ImGui::Selectable("None##1", a_data == BIPED_OBJECT::kNone))
					{
						a_data = BIPED_OBJECT::kNone;
						result = true;
					}

					ImGui::Separator();
				}

				ImGui::PopID();

				ImGui::PushID("list");

				using enum_type = std::underlying_type_t<BIPED_OBJECT>;

				for (enum_type i = 0; i < stl::underlying(BIPED_OBJECT::kSpecTotal); i++)
				{
					const auto e = static_cast<BIPED_OBJECT>(i);

					ImGui::PushID(i);

					bool selected = a_data == e;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					if (ImGui::Selectable(m_desc[i], selected))
					{
						a_data = e;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::PopID();

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}
	}
}