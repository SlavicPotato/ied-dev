#include "pch.h"

#include "UIPopupAction.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		void UIPopupAction::sanitize_input()
		{
			m_input = UICommon::SanitizeInputText(m_input);
		}

		void UIPopupAction::make_key()
		{
			if (auto it = std::find(
					m_key.begin(),
					m_key.end(),
					'#');
			    it != m_key.end())
			{
				m_key.erase(it, m_key.end());
			}

			m_key += "###pa_key";
		}
	}
}