#pragma once

namespace IED
{
	namespace UI
	{
		template <class T>
		class UIFilterBase
		{
		public:
			static constexpr std::size_t FILTER_BUF_SIZE = 256;

			bool Draw(const char* a_label = "##fil_input");
			void DrawButton();
			void clear();
			void Toggle();

			[[nodiscard]] constexpr bool IsOpen() const noexcept
			{
				return m_searchOpen;
			}

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return m_filter.has_value();
			}

			/*[[nodiscard]] constexpr const auto& operator*() const noexcept
			{
				return m_filter;
			}

			[[nodiscard]] constexpr const auto* operator->() const noexcept
			{
				return std::addressof(m_filter);
			}*/

			constexpr void NextSetFocus() noexcept
			{
				m_nextSetFocus = true;
			}

			[[nodiscard]] constexpr bool Has() const noexcept
			{
				return m_filter.has_value();
			}

			[[nodiscard]] constexpr const char* GetBuffer() const noexcept
			{
				return m_filterBuf.get();
			}

			constexpr void SetFlags(ImGuiInputTextFlags a_flags) noexcept
			{
				m_inputTextFlags = a_flags;
			}

		protected:
			UIFilterBase();
			UIFilterBase(bool a_isOpen);

			virtual bool        ProcessInput(const char* a_label) = 0;
			virtual const char* GetHelpText() { return nullptr; };

			std::unique_ptr<char[]> m_filterBuf;

			std::optional<T> m_filter;
			bool             m_searchOpen{ false };
			bool             m_nextSetFocus{ false };

			ImGuiInputTextFlags m_inputTextFlags{ ImGuiInputTextFlags_None };
		};

		template <class T>
		UIFilterBase<T>::UIFilterBase() :
			m_filterBuf(std::make_unique<char[]>(FILTER_BUF_SIZE))
		{
		}

		template <class T>
		UIFilterBase<T>::UIFilterBase(bool a_isOpen) :
			m_filterBuf(std::make_unique<char[]>(FILTER_BUF_SIZE)),
			m_searchOpen(a_isOpen)
		{
		}

		template <class T>
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

		template <class T>
		bool UIFilterBase<T>::Draw(const char* a_label)
		{
			if (!m_searchOpen)
			{
				return false;
			}

			ImGui::PushID(a_label);

			const auto r = ProcessInput(a_label);

			ImGui::PopID();

			return r;
		}

		template <class T>
		void UIFilterBase<T>::Toggle()
		{
			m_searchOpen = !m_searchOpen;
		}

		template <class T>
		void UIFilterBase<T>::clear()
		{
			m_filter.clear();
			m_filterBuf[0] = 0x0;
		}

	}
}