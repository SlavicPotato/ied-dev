#pragma once

#include "IED/ConfigData.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		class UIObjectSlotSelectorWidget
		{
		public:
			template <Data::concepts::is_valid_slot_id T>
			static bool DrawObjectSlotSelector(
				const char* a_label,
				T&          a_data,
				bool        a_allowNone = false);
		};

		template <Data::concepts::is_valid_slot_id T>
		bool UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
			const char* a_label,
			T&          a_data,
			bool        a_allowNone)
		{
			constexpr bool is_extra_type =
				stl::is_any_same_v<
					T,
					Data::ObjectSlotExtra,
					Data::ObjectTypeExtra>;

			bool result = false;

			ImGui::PushID("os_sel");

			const char* preview;

			if constexpr (is_extra_type)
			{
				preview = a_data != T::kNone ?
				              Data::GetObjectName(a_data) :
				              "None";
			}
			else
			{
				preview = Data::GetSlotName(a_data);
			}

			if (ImGui::BeginCombo(a_label, preview, ImGuiComboFlags_HeightLarge))
			{
				if constexpr (is_extra_type)
				{
					ImGui::PushID("header");

					if (a_allowNone)
					{
						if (ImGui::Selectable("None##1", a_data == T::kNone))
						{
							a_data = T::kNone;
							result = true;
						}

						ImGui::Separator();
					}

					ImGui::PopID();

					ImGui::PushID("list");
				}

				using enum_type = std::underlying_type_t<T>;

				for (enum_type i = 0; i < stl::underlying(T::kMax); i++)
				{
					const auto e = static_cast<T>(i);

					ImGui::PushID(i);

					bool selected = a_data == e;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					auto desc = Data::GetObjectName(e);

					if (ImGui::Selectable(desc, selected))
					{
						a_data = e;
						result = true;
					}

					ImGui::PopID();
				}

				if constexpr (is_extra_type)
				{
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}

	}
}