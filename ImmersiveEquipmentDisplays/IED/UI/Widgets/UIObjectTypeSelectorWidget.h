#pragma once

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
				T& a_out);
		};

		template <class T>
		bool UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
			const char* a_label,
			T& a_out)
		{
			bool result = false;

			ImGui::PushID("object_slot_selector");

			auto label = Data::GetSlotName(a_out);

			if (ImGui::BeginCombo(a_label, label))
			{
				using enum_type = std::underlying_type_t<T>;

				for (enum_type i = 0; i < stl::underlying(T::kMax); i++)
				{
					auto e = static_cast<T>(i);

					ImGui::PushID(i);

					bool selected = a_out == e;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					auto desc = Data::GetSlotName(e);

					if (ImGui::Selectable(desc, selected))
					{
						a_out = e;
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