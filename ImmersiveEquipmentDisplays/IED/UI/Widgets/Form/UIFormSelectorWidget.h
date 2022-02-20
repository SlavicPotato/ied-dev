#pragma once

#include "IED/Controller/IForm.h"
#include "IED/UI/UIFormBrowser.h"
#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UITips.h"

#include "../UIPopupToggleButtonWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormSelectorWidget :
			public virtual UITipsInterface,
			public virtual UIPopupToggleButtonWidget,
			public virtual UILocalizationInterface
		{
			using on_open_func_t = std::function<void(UIFormSelectorWidget&, UIFormBrowser&)>;

		public:
			UIFormSelectorWidget(
				Controller&   a_controller,
				FormInfoFlags a_requiredFlags,
				bool          a_restrictTypes     = false,
				bool          a_enableFormBrowser = true,
				bool          a_forceBase         = false);

			bool DrawFormSelector(
				const char*   a_label,
				Game::FormID& a_form,
				const char*   a_tipText = nullptr);

			bool DrawFormSelectorMulti();

			void Reset();

			void SetOnFormBrowserOpenFunc(on_open_func_t a_func);
			void SetAllowedTypes(std::initializer_list<UIFormBrowser::tab_filter_type::value_type> a_types);
			void SetAllowedTypes(const std::shared_ptr<const UIFormBrowser::tab_filter_type>& a_types);

			inline constexpr void SetFormBrowserEnabled(bool a_switch) noexcept
			{
				m_enableFormBrowser = a_switch;
			}

			[[nodiscard]] inline constexpr const auto& GetAllowedTypes() const noexcept
			{
				return m_types;
			}

			[[nodiscard]] const UIFormBrowser::selected_form_list& GetSelectedEntries() const noexcept;

			void ClearSelectedEntries() noexcept;

			[[nodiscard]] bool HasType(const formInfo_t& a_info) const;

			[[nodiscard]] inline const auto& GetInfo() const noexcept
			{
				return m_state->m_currentInfo;
			}

			[[nodiscard]] bool IsEntryValid(const IFormDatabase::entry_t& a_entry) const;

			[[nodiscard]] bool IsCurrentValid() const noexcept;

			[[nodiscard]] inline constexpr bool IsFormBrowserEnabled() const noexcept
			{
				return m_enableFormBrowser;
			}

		private:
			void         DrawInfo(Game::FormID a_form);
			void         QueueLookup(Game::FormID);
			void         QueueGetCrosshairRef();
			Game::FormID GetFormIDFromInputBuffer();
			bool         GetInputBufferChanged();
			void         ErrorMessage(const char* a_text);

			inline constexpr auto& GetCurrentFormInfo() const noexcept
			{
				return !m_forceBase ?
				           m_state->m_currentInfo->form :
                           m_state->m_currentInfo->get_base();
			}

			struct state_t
			{
				IForm::info_result m_currentInfo;
				Game::FormID       m_bufferedFormID;
				Game::FormID       m_lastInputFormID;
				char               m_inputBuffer[9]{ 0 };
				char               m_lastInputBuffer[9]{ 0 };
			};

			static void SetInputFormID(
				const std::shared_ptr<state_t>& a_state,
				Game::FormID                    a_form);

			bool m_nextGrabKeyboardFocus{ false };
			bool m_restrictTypes{ false };
			bool m_enableFormBrowser{ true };
			bool m_forceBase{ false };

			FormInfoFlags                                         m_requiredFlags{ FormInfoFlags::kNone };
			std::shared_ptr<const UIFormBrowser::tab_filter_type> m_types;
			std::shared_ptr<state_t>                              m_state;

			Controller& m_controller;

		protected:
			on_open_func_t m_onOpenFunc;
		};

	}  // namespace UI
}  // namespace IED