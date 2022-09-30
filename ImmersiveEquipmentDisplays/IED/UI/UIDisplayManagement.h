#pragma once

#include "UIMainCommon.h"

#include "Window/UIWindow.h"
#include "UIContext.h"

#include "Custom/UICustomTabPanel.h"
#include "EquipmentSlots/UISlotTabPanel.h"

#include "UILocalizationInterface.h"

#include "Widgets/UIExportFilterWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIDisplayManagement :
			public UIWindow,
			public UIContext,
			UIExportFilterWidget,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID  = "ied_displmgmt";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIDisplayManagement;

			UIDisplayManagement(Controller& a_controller);

			virtual ~UIDisplayManagement() noexcept override = default;

			void Initialize() override;
			void Reset() override;
			void Draw() override;
			void OnOpen() override;
			void OnClose() override;
			void Notify(std::uint32_t a_code, void* a_params) override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawMenuBar();
			void DrawViewMenu();
			void OpenEditorPanel(UIDisplayManagementEditorPanel a_panel);
			void SetTitle(Localization::StringID a_strid);

			inline static constexpr std::size_t NUM_PANELS = 2;

			template <class T>
			[[nodiscard]] inline constexpr auto& GetEditorPanelBase() const noexcept
			{
				static_assert(stl::underlying(T::PANEL_ID) < NUM_PANELS);

				return *m_editorPanels[stl::underlying(T::PANEL_ID)];
			}

			[[nodiscard]] inline auto& GetEditorPanelBase(UIDisplayManagementEditorPanel a_id) const noexcept
			{
				assert(stl::underlying(a_id) < NUM_PANELS);

				return *m_editorPanels[stl::underlying(a_id)];
			}

			template <class T, class... Args>
			void CreateEditorPanel(Args&&... a_args)  //
				requires(std::is_base_of_v<UIEditorTabPanel, T>)
			{
				static_assert(stl::underlying(T::PANEL_ID) < NUM_PANELS);

				assert(m_editorPanels[stl::underlying(T::PANEL_ID)] == nullptr);

				m_editorPanels[stl::underlying(T::PANEL_ID)] = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

#if defined(IED_ENABLE_I3DI)
			I3DIMain m_i3di;
#endif

			std::array<
				std::unique_ptr<UIEditorTabPanel>,
				NUM_PANELS>
				m_editorPanels;

			UIDisplayManagementEditorPanel m_currentEditorPanel{
				UIDisplayManagementEditorPanel::Slot
			};

			char m_currentTitle[128]{ 0 };

			Controller& m_controller;
		};

	}
}