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
			template <class T>
			static bool DrawObjectSlotSelector(
				const char* a_label,
				T&          a_data,
				bool        a_allowNone = false) requires(  //
				stl::is_any_same_v<T, Data::ObjectSlot, Data::ObjectSlotExtra>);
		};

		template <class T>
		bool UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
			const char* a_label,
			T&          a_data,
			bool        a_allowNone) requires(  //
			stl::is_any_same_v<T, Data::ObjectSlot, Data::ObjectSlotExtra>)
		{
			bool result = false;

			ImGui::PushID("os_sel");

			const char* preview;

			if constexpr (std::is_same_v<T, Data::ObjectSlotExtra>)
			{
				preview = a_data != T::kNone ?
				              Data::GetSlotName(a_data) :
                              "None";
			}
			else
			{
				preview = Data::GetSlotName(a_data);
			}

			if (ImGui::BeginCombo(a_label, preview, ImGuiComboFlags_HeightLarge))
			{
				if constexpr (std::is_same_v<T, Data::ObjectSlotExtra>)
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
					auto e = static_cast<T>(i);

					ImGui::PushID(i);

					bool selected = a_data == e;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					auto desc = Data::GetSlotName(e);

					if (ImGui::Selectable(desc, selected))
					{
						a_data = e;
						result = true;
					}

					ImGui::PopID();
				}

				if constexpr (std::is_same_v<T, Data::ObjectSlotExtra>)
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