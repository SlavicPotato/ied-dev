#pragma once

#include "UIEditorInterface.h"

#include "IED/ConfigCommon.h"
#include "IED/SettingHolder.h"

#include "UILocalizationInterface.h"

#include "UIMainCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace detail
		{
			template <class T>
			concept accept_editor_label =                                              //
				(std::is_same_v<std::underlying_type_t<T>, Localization::StringID> ||  //
			     std::is_same_v<T, Localization::StringID>);
		}

		class UIEditorTabPanel :
			public UILocalizationInterface
		{
		protected:
			struct Interface
			{
				Interface() = default;

				template <detail::accept_editor_label Ts>
				inline constexpr Interface(
					std::unique_ptr<UIEditorInterface> a_interface,
					Ts                                 a_label) :
					ptr(std::move(a_interface)),
					label(static_cast<Localization::StringID>(a_label))
				{
				}

				[[nodiscard]] inline constexpr explicit operator bool() const noexcept
				{
					return static_cast<bool>(ptr);
				}

				[[nodiscard]] inline constexpr auto operator->() const noexcept
				{
					return ptr.get();
				}

				std::unique_ptr<UIEditorInterface> ptr;
				Localization::StringID             label{ 0 };
				ImGuiTabBarFlags                   flags{ ImGuiTabItemFlags_None };
			};

		public:
			UIEditorTabPanel(
				Controller&            a_controller,
				Localization::StringID a_menuName);

			template <detail::accept_editor_label Ts, class... Args>
			constexpr UIEditorTabPanel(
				Controller& a_controller,
				Ts          a_menuName,
				Args&&... a_args) :
				UILocalizationInterface(a_controller),
				m_controller(a_controller),
				m_menuName(static_cast<Localization::StringID>(a_menuName)),
				m_interfaces{ stl::make_array(std::forward<Args>(a_args)...) }
			{
			}

			virtual ~UIEditorTabPanel() noexcept = default;

			void Initialize();
			void Reset();
			void QueueUpdateCurrent();
			void Draw();
			void DrawMenuBarItems();
			void OnOpen();
			void OnClose();

			template <detail::accept_editor_label Ts>
			constexpr void SetEditor(
				Data::ConfigClass                  a_class,
				std::unique_ptr<UIEditorInterface> a_interface,
				Ts                                 a_label)
			{
				m_interfaces[stl::underlying(a_class)] = {
					std::move(a_interface),
					a_label
				};
			}

			inline constexpr auto& GetInterface(Data::ConfigClass a_class) noexcept
			{
				return m_interfaces[stl::underlying(a_class)];
			}

			inline constexpr auto& GetInterface(Data::ConfigClass a_class) const noexcept
			{
				return m_interfaces[stl::underlying(a_class)];
			}

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() = 0;

			void SetTabSelected(
				Data::ConfigClass a_class);

			void EvaluateTabSwitch(
				Data::ConfigClass a_class);

			std::array<Interface, Data::CONFIG_CLASS_MAX> m_interfaces;
			Localization::StringID                        m_menuName;
			Data::ConfigClass                             m_currentClass{ Data::ConfigClass::Global };

		protected:
			Controller& m_controller;
		};

	}
}