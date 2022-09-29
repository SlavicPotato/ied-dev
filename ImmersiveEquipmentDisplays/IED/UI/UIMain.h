#pragma once

#include "UIAboutModal.h"
#include "UIContext.h"
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
			public UIContext,
			public UIWindow,
			UIExportFilterWidget,
			UIAboutModal,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_main";

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
				return GetChild<UIFormBrowser>();
			}

			inline constexpr auto& GetFormLookupCache() noexcept
			{
				return m_formLookupCache;
			}

			const Data::SettingHolder::UserInterface& GetUISettings() noexcept;

		private:
			void DrawChildWindows();

			void DrawMenuBarMain();
			void DrawMenuBarContents();
			void DrawFileMenu();
			void DrawViewMenu();
			void DrawToolsMenu();
			void DrawActionsMenu();
			void DrawHelpMenu();

			void DrawDefaultConfigSubmenu();

			template <class T>
			[[nodiscard]] inline constexpr auto& GetChildContext() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				return *m_childWindows[stl::underlying(T::CHILD_ID)];
			}

			template <class T>
			[[nodiscard]] inline constexpr T& GetChild() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				auto ptr = m_childWindows[stl::underlying(T::CHILD_ID)].get();
				assert(ptr);

				auto result = dynamic_cast<T*>(ptr);
				assert(result);

				return *result;
			}

			template <class T, class... Args>
			void CreateChild(Args&&... a_args)  //
				requires(std::is_base_of_v<UIContext, T>)
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				assert(m_childWindows[stl::underlying(T::CHILD_ID)] == nullptr);

				m_childWindows[stl::underlying(T::CHILD_ID)] = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

			std::array<
				std::unique_ptr<UIContext>,
				stl::underlying(ChildWindowID::kMax)>
				m_childWindows;

#if defined(IED_ENABLE_I3DI)
			I3DIMain m_i3di;
#endif

			UIFormInfoCache m_formLookupCache;

			UIPopupQueue m_popupQueue;

			Controller& m_controller;
		};

	}
}