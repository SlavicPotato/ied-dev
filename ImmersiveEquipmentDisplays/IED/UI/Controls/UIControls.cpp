#include "pch.h"

#include "UIControls.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		bool UIControls::ColorEdit4(
			const char* a_label,
			float (&a_col)[4],
			ImGuiColorEditFlags a_flags)
		{
			bool res = ImGui::ColorEdit4(a_label, a_col, a_flags);
			if (res)
			{
				OnControlValueChange();
			}
			return res;
		}

		void UIControls::OnControlValueChange() const
		{
		}

	}
}