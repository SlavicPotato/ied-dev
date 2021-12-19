#include "pch.h"

#include "UIBipedObjectSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		const char* UIBipedObjectSelectorWidget::m_desc[]{
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
			"Quiver"
		};

		bool UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
			const char* a_label,
			Biped::BIPED_OBJECT& a_data)
		{
			bool result = false;

			ImGui::PushID("biped_object_selector");

			auto preview = GetBipedSlotDesc(a_data);

			if (ImGui::BeginCombo(a_label, preview))
			{
				using enum_type = std::underlying_type_t<Biped::BIPED_OBJECT>;

				for (enum_type i = 0; i < stl::underlying(Biped::BIPED_OBJECT::kTotal); i++)
				{
					auto e = static_cast<Biped::BIPED_OBJECT>(i);

					ImGui::PushID(i);

					bool selected = a_data == e;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(m_desc[i], selected))
					{
						a_data = e;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}
	}
}