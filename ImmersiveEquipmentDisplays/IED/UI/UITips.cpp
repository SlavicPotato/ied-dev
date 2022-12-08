#include "pch.h"

#include "UITips.h"

#include "UICommon.h"

namespace IED
{
	namespace UI
	{
		void UITipsInterface::DrawTip(
			UITip a_id,
			bool  a_sameLine)
		{
			DrawTipText(UITipsInterface::GetTipText(a_id), a_sameLine);
		}

		void UITipsInterface::DrawTipImportant(
			UITip a_id,
			bool  a_sameLine)
		{
			DrawTipTextImportant(UITipsInterface::GetTipText(a_id), a_sameLine);
		}

		void UITipsInterface::DrawTip(
			const char* a_text,
			bool        a_sameLine)
		{
			DrawTipText(a_text, a_sameLine);
		}

		void UITipsInterface::DrawTipText(
			const char* a_text,
			bool        a_sameLine)
		{
			ImGui::SameLine();
			UICommon::HelpMarker(a_text);
		}

		void UITipsInterface::DrawTipTextImportant(
			const char* a_text,
			bool        a_sameLine)
		{
			ImGui::SameLine();
			UICommon::HelpMarkerImportant(a_text);
		}

		const char* UITipsInterface::GetTipText(UITip a_id)
		{
			return UIL::LS(a_id);
		}

	}
}