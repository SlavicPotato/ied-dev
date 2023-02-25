#pragma once

#include "PopupQueue/UIPopupQueue.h"
#include "UIAboutModal.h"
#include "UIContext.h"
#include "UIFormBrowser.h"
#include "UIFormInfoCache.h"
#include "UIKeyedInputLockReleaseHandler.h"
#include "UILocalizationInterface.h"
#include "UIMainCommon.h"
#include "UIMainStrings.h"

#include "Window/UIWindow.h"

#include "IED/SettingHolder.h"

namespace IED
{
	class Controller;

	namespace Tasks
	{
		class UIRenderTaskBase;
	};

	namespace UI
	{
		class UIMain :
			public UIContext,
			public UIWindow,
			public UIKeyedInputLockReleaseHandler,
			UIAboutModal,
			::Events::EventSink<UIContextStateChangeEvent>
		{
			static constexpr auto WINDOW_ID = "ied_main";

		public:
			UIMain(
				Tasks::UIRenderTaskBase& a_owner,
				Controller&              a_controller);

			virtual ~UIMain() noexcept override = default;

			virtual void Initialize() override;
			virtual void Reset() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;

			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;
			virtual void OnKeyEvent(const Handlers::KeyEvent& a_evn) override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(-1);
			}

			[[nodiscard]] constexpr auto& GetPopupQueue() noexcept
			{
				return m_popupQueue;
			}

			[[nodiscard]] constexpr auto& GetFormLookupCache() noexcept
			{
				return m_formLookupCache;
			}

			Data::SettingHolder::UserInterface& GetUISettings() const noexcept;

			[[nodiscard]] constexpr auto& GetOwnerTask() const noexcept
			{
				return m_owner;
			}

			template <class T>
			[[nodiscard]] constexpr auto GetChildContext() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				return m_childWindows[stl::underlying(T::CHILD_ID)].get();
			}

			[[nodiscard]] inline auto* GetChildContext(ChildWindowID a_id) const noexcept
			{
				assert(a_id < ChildWindowID::kMax);

				return m_childWindows[stl::underlying(a_id)].get();
			}

			template <class T>
			[[nodiscard]] constexpr T* GetChild() const noexcept
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				auto ptr = m_childWindows[stl::underlying(T::CHILD_ID)].get();

				return static_cast<T*>(ptr);
			}

			[[nodiscard]] constexpr auto& GetFormBrowser() noexcept
			{
				auto result = GetChild<UIFormBrowser>();
				assert(result);
				return *result;
			}

		private:
			virtual void Receive(const UIContextStateChangeEvent& a_evn) override;

			void DrawChildWindows();

			void DrawMenuBarMain();
			void DrawMenuBarContents();
			void DrawFileMenu();
			void DrawViewMenu();
			void DrawProfileEditorsSubmenu();
			void DrawDiagnosticsSubmenu();
			void DrawToolsMenu();
			void DrawActionsMenu();
			void DrawHelpMenu();

			void DrawDefaultConfigSubmenu();

			bool HasOpenChild() const;

			template <class T, class Te>
			void DrawContextMenuItem(Te a_title, const char* a_id);

			virtual bool ILRHGetCurrentControlLockSetting() const override;
			virtual bool ILRHGetCurrentFreezeTimeSetting() const override;

			template <class T, class... Args>
			constexpr void CreateChild(Args&&... a_args)  //
				requires(std::is_base_of_v<UIContext, T>)
			{
				static_assert(T::CHILD_ID < ChildWindowID::kMax);

				assert(!m_childWindows[stl::underlying(T::CHILD_ID)]);

				m_childWindows[stl::underlying(T::CHILD_ID)] = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

			static_assert(
				std::is_same_v<
					std::underlying_type_t<ChildWindowID>,
					std::uint32_t>);

			using child_window_container_type = std::array<
				std::unique_ptr<UIContext>,
				stl::underlying(ChildWindowID::kMax)>;

			child_window_container_type m_childWindows;

			UIFormInfoCache              m_formLookupCache;
			UIPopupQueue                 m_popupQueue;
			std::optional<ChildWindowID> m_lastClosedChild;
			bool                         m_seenOpenChildThisSession{ false };

			Tasks::UIRenderTaskBase& m_owner;
			Controller&              m_controller;
		};

		template <class T, class Te>
		void UIMain::DrawContextMenuItem(Te a_title, const char* a_id)
		{
			auto context = GetChildContext<T>();

			if (ImGui::MenuItem(
					UIL::LS(a_title, a_id),
					nullptr,
					context && context->IsContextOpen(),
					static_cast<bool>(context)))
			{
				if (context)
				{
					context->ToggleOpenState();
				}
			}
		}

	}
}