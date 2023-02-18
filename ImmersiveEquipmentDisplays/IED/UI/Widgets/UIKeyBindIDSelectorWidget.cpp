#include "pch.h"

#include "UIKeyBindIDSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIKeyBindIDSelectorWidget::DrawKeyBindIDSelector(
			stl::fixed_string& a_out)
		{
			std::string tmp = a_out;

			const auto result = DrawKeyBindIDSelector(tmp);

			if (result)
			{
				a_out = tmp;
			}

			return result;
		}

		bool UIKeyBindIDSelectorWidget::DrawKeyBindIDSelector(
			std::string& a_id)
		{
			return ImGui::InputText(
				UIL::LS(CommonStrings::ID, "ex_kbid"),
				std::addressof(a_id),
				ImGuiInputTextFlags_EnterReturnsTrue);
		}
	}
}