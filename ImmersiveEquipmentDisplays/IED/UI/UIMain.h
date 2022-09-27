#pragma once

#include "UIAboutModal.h"
#include "UIContextBase.h"
#include "UIFormInfoCache.h"
#include "UILocalizationInterface.h"
#include "UIMainCommon.h"
#include "UIMainStrings.h"

#include "Custom/UICustomTabPanel.h"
#include "EquipmentSlots/UISlotTabPanel.h"

#include "Window/UIWindow.h"

#include "Widgets/UIExportFilterWidget.h"

#include "IED/ConfigSerializationFlags.h"
#include "IED/SettingHolder.h"

#if defined(IED_ENABLE_I3DI)
#	include "I3DI/I3DIMain.h"
#endif

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIMain :
			public UIContextBase,
			public UIWindow,
			UIExportFilterWidget,
			UIAboutModal,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto TITLE_NAME = PLUGIN_NAME_FULL;
			inline static constexpr auto WINDOW_ID  = "ied_main";

		public:
			UIMain(Controller& a_controller);

			virtual ~UIMain() noexcept override = default;

			virtual void Initialize() override;
			virtual void Reset() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;

			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;

			inline constexpr auto& GetPopupQueue() noexcept
			{
				return m_popupQueue;
			}

			inline constexpr auto& GetFormBrowser() noexcept
			{
				return GetChildWindow<UIFormBrowser>();
			}

			inline constexpr auto& GetFormLookupCache() noexcept
			{
				return m_formLookupCache;
			}

			const Data::SettingHolder::UserInterface& GetUISettings() noexcept;

		private:
			void DrawMenuBar();
			void DrawFileMenu();
			void DrawViewMenu();
			void DrawToolsMenu();
			void DrawActionsMenu();
			void DrawHelpMenu();

			void DrawDefaultConfigSubmenu();

			void OpenEditorPanel(UIEditorPanel a_panel);

			void SetTitle(Localization::StringID a_strid);

			template <class T>
			[[nodiscard]] inline constexpr auto& GetChildWindowBase() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				return *m_childWindows[stl::underlying(T::CHILD_ID)];
			}

			template <class T>
			[[nodiscard]] inline constexpr T& GetChildWindow() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				auto ptr = m_childWindows[stl::underlying(T::CHILD_ID)].get();
				assert(ptr);

				auto result = dynamic_cast<T*>(ptr);
				assert(result);

				return *result;
			}

			template <class T, class... Args>
			void CreateChildWindow(Args&&... a_args)  //
				requires(std::is_base_of_v<UIChildWindowBase, T>)
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				assert(m_childWindows[stl::underlying(T::CHILD_ID)] == nullptr);

				m_childWindows[stl::underlying(T::CHILD_ID)] = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

			
			template <class T>
			[[nodiscard]] inline constexpr auto& GetEditorPanelBase() const noexcept
			{
				static_assert(stl::underlying(T::PANEL_ID) < 2);

				return *m_editorPanels[stl::underlying(T::PANEL_ID)];
			}
			
			[[nodiscard]] inline auto& GetEditorPanelBase(UIEditorPanel a_id) const noexcept
			{
				assert(a_id < 2);

				return *m_editorPanels[stl::underlying(a_id)];
			}

			
			template <class T, class... Args>
			void CreateEditorPanel(Args&&... a_args)  //
				requires(std::is_base_of_v<UIEditorTabPanel, T>)
			{
				static_assert(stl::underlying(T::PANEL_ID) < 2);

				assert(m_editorPanels[stl::underlying(T::PANEL_ID)] == nullptr);

				m_editorPanels[stl::underlying(T::PANEL_ID)] = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

			std::array<
				std::unique_ptr<UIChildWindowBase>,
				stl::underlying(ChildWindowID::kMax)>
				m_childWindows;

#if defined(IED_ENABLE_I3DI)
			I3DIMain m_i3di;
#endif

			std::array<
				std::unique_ptr<UIEditorTabPanel>,
				2>
				m_editorPanels;

			UIEditorPanel m_currentEditorPanel{ UIEditorPanel::Slot };

			UIFormInfoCache m_formLookupCache;

			UIPopupQueue m_popupQueue;

			char m_currentTitle[128]{ 0 };

			Controller& m_controller;
		};

	}
}