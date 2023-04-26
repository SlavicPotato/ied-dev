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
		namespace concepts
		{
			template <class T>
			concept accept_editor_label =                                              //
				(std::is_same_v<std::underlying_type_t<T>, Localization::StringID> ||  //
			     std::is_same_v<T, Localization::StringID>);
		}

		class UIEditorTabPanel
		{
		protected:
			struct Interface
			{
				Interface() = default;

				template <concepts::accept_editor_label Ts>
				explicit Interface(
					std::unique_ptr<UIEditorInterface> a_interface,
					Ts                                 a_label) :
					ptr(std::move(a_interface)),
					label(static_cast<Localization::StringID>(a_label))
				{
				}

				[[nodiscard]] SKMP_143_CONSTEXPR explicit operator bool() const noexcept
				{
					return static_cast<bool>(ptr);
				}

				[[nodiscard]] SKMP_143_CONSTEXPR auto operator->() const noexcept
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

			template <concepts::accept_editor_label Ts, class... Args>
			constexpr UIEditorTabPanel(
				Controller& a_controller,
				Ts          a_menuName,
				Args&&... a_args) :
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

			template <concepts::accept_editor_label Ts>
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

			constexpr auto& GetInterface(Data::ConfigClass a_class) noexcept
			{
				return m_interfaces[stl::underlying(a_class)];
			}

			constexpr auto& GetInterface(Data::ConfigClass a_class) const noexcept
			{
				return m_interfaces[stl::underlying(a_class)];
			}

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() = 0;

			void SetTabSelected(
				Data::ConfigClass a_class);

			void EvaluateTabSwitch(
				Data::ConfigClass a_class);

			void StoreCurrentTab();

			std::array<Interface, Data::CONFIG_CLASS_MAX> m_interfaces;
			Localization::StringID                        m_menuName;
			Data::ConfigClass                             m_currentClass{ Data::ConfigClass::Global };

		protected:
			Controller& m_controller;
		};

	}
}