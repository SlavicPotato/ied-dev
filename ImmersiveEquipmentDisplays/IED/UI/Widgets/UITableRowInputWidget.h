#pragma once

namespace IED
{
	namespace UI
	{
		template <class T>
		class UITableRowInputWidget
		{
			struct state_t
			{
				int index{ -1 };
				T key;
				bool grabFocus{ true };
				bool focusObtained{ false };

				char buffer[512]{ 0 };

				void Reset();
			};

		public:
			UITableRowInputWidget(
				ImGuiInputTextFlags a_inputFlags = ImGuiInputTextFlags_None);

			bool DrawColumn(
				int a_index,
				const T& a_key,
				const char* a_text);

			const char* GetBuffer() const;

		private:
			ImGuiInputTextFlags m_inputFlags;

			stl::optional<state_t> m_current;
		};

		template <class T>
		inline UITableRowInputWidget<T>::UITableRowInputWidget(
			ImGuiInputTextFlags a_inputFlags) :
			m_inputFlags(a_inputFlags)
		{
		}

		template <class T>
		bool UITableRowInputWidget<T>::DrawColumn(
			int a_index,
			const T& a_key,
			const char* a_text)
		{
			bool result = false;

			ImGui::PushID("table_row_input");
			ImGui::PushID(a_index);

			if (m_current && m_current->key == a_key && m_current->index == a_index)
			{
				ImGui::PushItemWidth(-1.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

				if (m_current->grabFocus)
				{
					ImGui::SetKeyboardFocusHere();
					m_current->grabFocus = false;
				}

				if (ImGui::InputText(
						"##input",
						m_current->buffer,
						sizeof(m_current->buffer),
						ImGuiInputTextFlags_EnterReturnsTrue |
							ImGuiInputTextFlags_AutoSelectAll |
							m_inputFlags))
				{
					result = true;
				}
				else
				{
					auto hasFocus = ImGui::IsItemActive();

					if (!m_current->focusObtained)
					{
						m_current->focusObtained = hasFocus;
					}
					else
					{
						if (!hasFocus)
						{
							m_current.reset();
						}
					}
				}

				ImGui::PopStyleVar();
				ImGui::PopItemWidth();
			}
			else
			{
				if (ImGui::Selectable(
						a_text,
						false,
						ImGuiSelectableFlags_DontClosePopups))
				{
					m_current = {
						a_index,
						a_key
					};

					stl::snprintf(m_current->buffer, "%s", a_text);
				}
			}

			ImGui::PopID();
			ImGui::PopID();

			return result;
		}

		template <class T>
		inline const char* UITableRowInputWidget<T>::GetBuffer() const
		{
			return m_current->buffer;
		}

		template <class T>
		inline void UITableRowInputWidget<T>::state_t::Reset()
		{
			buffer[0] = 0;
		}

	}
}