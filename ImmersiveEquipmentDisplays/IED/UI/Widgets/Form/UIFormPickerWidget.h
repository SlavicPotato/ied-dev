#pragma once

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"

#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UIPopupInterface.h"
#include "IED/UI/UITips.h"
#include "UIFormSelectorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormPickerWidget :
			public UIFormSelectorWidget,
			UIPopupInterface,
			public virtual UIFormLookupInterface
		{
		public:
			UIFormPickerWidget(
				Controller&   a_controller,
				FormInfoFlags a_requiredFlags,
				bool          a_restrictTypes = false,
				bool          a_forceBase     = false);

			bool DrawFormPicker(
				const char*               a_strid,
				Localization::StringID    a_label,
				Data::configCachedForm_t& a_form,
				const char*               a_tipText = nullptr);

			bool DrawFormPicker(
				const char*            a_strid,
				Localization::StringID a_label,
				Game::FormID&          a_form,
				const char*            a_tipText = nullptr);

			constexpr const auto GetCurrentInfo() const noexcept
			{
				return m_currentInfo;
			}

			constexpr void SetAllowClear(bool a_allowClear) noexcept
			{
				m_allowClear = a_allowClear;
			}

		private:
			bool                                 DrawContextMenu(Game::FormID& a_form);
			UIFormBrowser::FormBrowserDrawResult DrawFormBrowserPopup(Game::FormID& a_form);

			const FormInfoResult* m_currentInfo{ nullptr };

			bool m_allowClear{ true };

			Controller& m_controller;
		};
	}
}