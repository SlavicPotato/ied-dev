#include "pch.h"

#include "UICommon.h"
#include "UITips.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UITipsInterface::UITipsInterface(
			Controller& a_controller) :
			UILocalizationInterface(a_controller)
		{
		}

		void UITipsInterface::DrawTip(
			UITip a_id,
			bool  a_sameLine) const
		{
			DrawTipText(GetTipText(a_id), a_sameLine);
		}

		void UITipsInterface::DrawTipImportant(
			UITip a_id,
			bool  a_sameLine) const
		{
			DrawTipTextImportant(GetTipText(a_id), a_sameLine);
		}

		void UITipsInterface::DrawTip(
			const char* a_text,
			bool        a_sameLine) const
		{
			DrawTipText(a_text, a_sameLine);
		}

		void UITipsInterface::DrawTipText(
			const char* a_text,
			bool        a_sameLine) const
		{
			ImGui::SameLine();
			UICommon::HelpMarker(a_text);
		}

		void UITipsInterface::DrawTipTextImportant(
			const char* a_text,
			bool        a_sameLine) const
		{
			ImGui::SameLine();
			UICommon::HelpMarkerImportant(a_text);
		}

		const char* UITipsInterface::GetTipText(UITip a_id) const
		{
			return LS(a_id);
		}

	}
}