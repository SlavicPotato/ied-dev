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

		void UITipsInterface::DrawTip(UITip a_id) const
		{
			DrawTipText(GetTipText(a_id));
		}

		void UITipsInterface::DrawTip(const char* a_text) const
		{
			DrawTipText(a_text);
		}

		void UITipsInterface::DrawTipText(const char* a_text) const
		{
			ImGui::SameLine();
			UICommon::HelpMarker(a_text);
		}

		const char* UITipsInterface::GetTipText(UITip a_id) const
		{
			return LS(a_id);
		}

	}
}