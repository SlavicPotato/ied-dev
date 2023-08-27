#pragma once

#include "Widgets/Filters/UIGenericFilter.h"
#include "Widgets/Form/UIFormBrowserOwner.h"
#include "Widgets/UIWidgetsCommon.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "UILocalizationInterface.h"

#include "IED/Controller/IFormDatabase.h"

#include "UIFormBrowserStrings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormBrowser :
			public UIContext,
			public UIWindow
		{
			struct TabItem
			{
				UIFormBrowserStrings label;
				std::uint32_t        type{ 0 };
				bool                 enabled{ true };
				bool                 state{ false };
			};

			static constexpr auto POPUP_ID = "form_browser";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUIFormBrowser;

			using selected_form_list = stl::vectormap<Game::FormID, IFormDatabase::entry_t>;

			struct FormBrowserDrawResult
			{
				bool result{ false };
				bool closed{ false };

				[[nodiscard]] inline constexpr explicit operator bool() const noexcept
				{
					return result;
				}
			};

			using tab_filter_type   = stl::flat_set<std::uint32_t>;
			using select_callback_t = std::function<void(const IFormDatabase::entry_t&)>;

			UIFormBrowser(Controller& a_controller);

			FormBrowserDrawResult DrawImpl();
			bool                  Open(bool a_multisel);
			bool                  IsBrowserOpen() const;
			void                  SetTabFilter(const tab_filter_type& a_filter);
			void                  SetTabFilter(std::initializer_list<tab_filter_type::value_type> a_init);
			void                  ClearTabFilter();

			void OnMainClose() override;
			void Reset() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

			constexpr auto& GetSelectedEntry() const noexcept
			{
				return m_selectedEntry;
			}

			constexpr auto& GetSelectedEntries() const noexcept
			{
				return m_selectedEntries;
			}

			inline void ClearSelectedEntries() noexcept
			{
				m_selectedEntries.clear();
			}

			inline void SetHighlightForm(Game::FormID a_form)
			{
				m_hlForm = a_form;
			}

			bool HasType(std::uint32_t a_type) const;

		private:
			bool DrawTabBar();

			bool DrawTabPanel(std::uint32_t a_type);

			template <class T>
			bool DrawTable(const T& a_data);

			void DrawContextMenu();

			void QueueGetDatabase();

			//bool m_openPopup{ false };
			bool m_nextDoFilterUpdate{ false };

			struct db_container :
				stl::intrusive_ref_counted
			{
				SKMP_REDEFINE_NEW_PREF();

				IFormDatabase::result_type data;
				bool                       queryInProgress{ false };
				stl::mutex                 lock;
			};

			const stl::smart_ptr<db_container> m_db;

			UIGenericFilter m_formIDFilter;
			UIGenericFilter m_formNameFilter;

			std::optional<stl::vector<IFormDatabase::entry_t>> m_filteredData;

			std::uint32_t m_currentType{ 0 };
			Game::FormID  m_hlForm;

			std::array<TabItem, 47> m_tabItems;

			//select_callback_t m_current;

			std::optional<IFormDatabase::entry_t> m_selectedEntry;
			selected_form_list                    m_selectedEntries;

			bool m_multiSelectMode{ false };

			Controller& m_controller;
		};

	}

}