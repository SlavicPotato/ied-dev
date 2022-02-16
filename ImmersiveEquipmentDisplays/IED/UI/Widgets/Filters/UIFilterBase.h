#pragma once

namespace IED
{
	namespace UI
	{
		template <class T>
		class UIFilterBase
		{
		public:
			bool Draw(const char* a_label = "##fil_input");
			void DrawButton();
			void clear();
			void Toggle();

			[[nodiscard]] virtual bool Test(const std::string& a_haystack) const = 0;

			[[nodiscard]] inline constexpr bool IsOpen() const noexcept
			{
				return m_searchOpen;
			}

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return m_filter.has();
			}

			/*[[nodiscard]] inline constexpr const auto& operator*() const noexcept
			{
				return m_filter;
			}

			[[nodiscard]] inline constexpr const auto* operator->() const noexcept
			{
				return std::addressof(m_filter);
			}*/

			inline constexpr void NextSetFocus() noexcept
			{
				m_nextSetFocus = true;
			}

			[[nodiscard]] inline constexpr bool Has() const noexcept
			{
				return m_filter.has();
			}

			[[nodiscard]] inline constexpr const char* GetBuffer() const noexcept
			{
				return m_filterBuf;
			}

			inline constexpr void SetFlags(ImGuiInputTextFlags a_flags) noexcept
			{
				m_inputTextFlags = a_flags;
			}

		protected:
			UIFilterBase();
			UIFilterBase(bool a_isOpen);

			virtual bool        ProcessInput(const char* a_label) = 0;
			virtual const char* GetHelpText() { return nullptr; };

			char m_filterBuf[128]{ 0 };

			stl::optional<T> m_filter;
			bool             m_searchOpen{ false };
			bool             m_nextSetFocus{ false };

			ImGuiInputTextFlags m_inputTextFlags{ ImGuiInputTextFlags_None };
		};

		template <typename T>
		UIFilterBase<T>::UIFilterBase()
		{
		}

		template <typename T>
		UIFilterBase<T>::UIFilterBase(bool a_isOpen) :
			m_searchOpen(a_isOpen)
		{
		}

		template <typename T>
		void UIFilterBase<T>::DrawButton()
		{
			if (ImGui::ArrowButton(
					"filter_base_button",
					m_searchOpen ? ImGuiDir_Down : ImGuiDir_Right))
			{
				m_searchOpen = !m_searchOpen;
				if (m_searchOpen)
					m_nextSetFocus = true;
			}
		}

		template <typename T>
		bool UIFilterBase<T>::Draw(const char* a_label)
		{
			if (!m_searchOpen)
			{
				return false;
			}

			ImGui::PushID("filter_base_main");

			auto r = ProcessInput(a_label);

			ImGui::PopID();

			return r;
		}

		template <typename T>
		void UIFilterBase<T>::Toggle()
		{
			m_searchOpen = !m_searchOpen;
		}

		template <typename T>
		void UIFilterBase<T>::clear()
		{
			m_filter.clear();
			m_filterBuf[0] = 0x0;
		}

	}
}