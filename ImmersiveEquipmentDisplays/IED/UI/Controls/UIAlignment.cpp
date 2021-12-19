#include "pch.h"

#include "UIAlignment.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		float UIAlignment::GetNextTextOffset(
			const stl::fixed_string& a_text,
			bool a_clear)
		{
			if (a_clear)
			{
				ClearTextOffset();
			}

			auto it = m_ctlPositions.find(a_text);
			if (it != m_ctlPositions.end())
			{
				return (m_posOffset += it->second + ImGui::GetStyle().FramePadding.x);
			}
			else
			{
				return (m_posOffset += ImGui::CalcTextSize(a_text.c_str()).x + ImGui::GetStyle().FramePadding.x);
			}
		}

		void UIAlignment::ClearTextOffset() { m_posOffset = 0.0f; }

		bool UIAlignment::ButtonRight(
			const stl::fixed_string& a_text,
			bool a_disabled)
		{
			UICommon::PushDisabled(a_disabled);

			bool res = ImGui::Button(a_text.c_str());

			UICommon::PopDisabled(a_disabled);

			m_ctlPositions.insert_or_assign(a_text, ImGui::GetItemRectSize().x);

			return res;
		}

	}
}