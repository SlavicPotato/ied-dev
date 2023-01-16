#pragma once

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"

#include "IED/UI/UICommon.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		namespace detail
		{
			template <class T>
			concept valid_list_handle =
				std::is_reference_v<T> == false &&
				std::is_convertible_v<T, std::uint64_t> &&
				sizeof(T) <= sizeof(std::uint64_t);
		}

		template <
			class Td,
			detail::valid_list_handle Th>
		class UIListBase
		{
			using list_type = stl::map<Th, stl::fixed_string>;

		public:
			constexpr void QueueListUpdateCurrent()
			{
				m_listNextUpdateCurrent = true;
			}

			constexpr void QueueListUpdate()
			{
				m_listNextUpdate = true;
			}

			constexpr void QueueListUpdate(Th const a_desiredHandle)
			{
				m_listNextUpdate = true;
				m_desiredHandle  = a_desiredHandle;
			}

		protected:
			struct listValue_t
			{
				Th                handle;
				stl::fixed_string desc;
				mutable Td        data;
			};

			virtual void ListReset();
			virtual void ListTick();

			void ListUpdateCurrent();
			void ListDrawInfo(const listValue_t& a_entry);

			UIListBase(
				float a_itemWidthScalar = -12.0f) noexcept;

			virtual ~UIListBase() noexcept = default;

			virtual void ListDraw();

			virtual void ListDrawOptions();
			virtual void ListDrawExtraControls();

			virtual void ListDrawInfoText(const listValue_t& a_entry) = 0;

			constexpr const std::optional<listValue_t>& ListGetSelected() const noexcept;

			virtual bool ListSetCurrentItem(Th a_handle);

			virtual void ListSetCurrentItem(
				const typename list_type::value_type& a_value);

			virtual void ListClearCurrentItem();

			virtual void ListUpdate() = 0;

			virtual void ListResetAllValues(
				Th a_handle) = 0;

			[[nodiscard]] virtual Td GetData(Th a_handle) = 0;

			virtual void OnListChangeCurrentItem(
				const std::optional<listValue_t>& a_oldHandle,
				const std::optional<listValue_t>& a_newHandle);

			virtual const ImVec4* HighlightEntry(Th a_handle);

			bool ListSelectFirstAvailable();
			void ListFilterSelected();

			bool m_listFirstUpdate{ false };
			bool m_listNextUpdateCurrent{ false };
			bool m_listNextUpdate{ true };

			list_type                  m_listData;
			std::optional<listValue_t> m_listCurrent;
			std::optional<Th>          m_desiredHandle;

			char            m_listBuf1[256]{ 0 };
			UIGenericFilter m_listFilter;
			float           m_itemWidthScalar;

		private:
			virtual void OnListSetHandleInternal(Th a_handle) = 0;
		};

		template <class Td, detail::valid_list_handle Th>
		UIListBase<Td, Th>::UIListBase(
			float a_itemWidthScalar) noexcept :
			m_itemWidthScalar(a_itemWidthScalar)
		{
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListDraw()
		{
			ListTick();
			ListFilterSelected();

			ImGui::PushID("list_base");

			ImGui::PushItemWidth(ImGui::GetFontSize() * m_itemWidthScalar);

			m_listFilter.DrawButton();
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushID("extra_controls");
			ListDrawExtraControls();
			ImGui::PopID();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushID("combo");

			if (ImGui::BeginCombo(
					m_listBuf1,
					m_listCurrent ?
						m_listCurrent->desc.c_str() :
						nullptr,
					ImGuiComboFlags_HeightLarge))
			{
				std::optional<Th> newItem;

				for (const auto& [i, e] : m_listData)
				{
					if (!m_listFilter.Test(*e))
					{
						continue;
					}

					const auto* const highlight = HighlightEntry(i);

					ImGui::PushID(
						reinterpret_cast<const void*>(
							static_cast<std::uint64_t>(i)));

					const bool selected = m_listCurrent && i == m_listCurrent->handle;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (highlight)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, *highlight);
					}

					if (ImGui::Selectable(UIL::LMKID<3>(e.c_str(), "1"), selected))
					{
						newItem.emplace(i);
					}

					if (highlight)
					{
						ImGui::PopStyleColor();
					}

					ImGui::PopID();
				}

				if (newItem)
				{
					ListSetCurrentItem(*newItem);
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			if (m_listCurrent)
			{
				ListDrawInfo(*m_listCurrent);
			}

			m_listFilter.Draw();

			ImGui::PopItemWidth();

			ImGui::PushID("options");
			ListDrawOptions();
			ImGui::PopID();

			ImGui::PopID();
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListDrawOptions()
		{
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListDrawExtraControls()
		{
		}

		template <class Td, detail::valid_list_handle Th>
		bool UIListBase<Td, Th>::ListSelectFirstAvailable()
		{
			for (const auto& e : m_listData)
			{
				if (!m_listFilter.Test(*e.second))
				{
					continue;
				}

				ListSetCurrentItem(e);

				return true;
			}

			return false;
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListFilterSelected()
		{
			if (m_listCurrent)
			{
				if (!m_listFilter.Test(*m_listCurrent->desc))
				{
					if (!ListSelectFirstAvailable())
					{
						ListClearCurrentItem();
					}
				}
			}
			else
			{
				ListSelectFirstAvailable();
			}
		}

		template <class Td, detail::valid_list_handle Th>
		constexpr auto UIListBase<Td, Th>::ListGetSelected() const noexcept
			-> const std::optional<listValue_t>&
		{
			return m_listCurrent;
		}

		template <class Td, detail::valid_list_handle Th>
		bool UIListBase<Td, Th>::ListSetCurrentItem(Th a_handle)
		{
			auto it = m_listData.find(a_handle);
			if (it != m_listData.end())
			{
				ListSetCurrentItem(*it);
				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListSetCurrentItem(
			const typename list_type::value_type& a_value)
		{
			const auto old(std::move(m_listCurrent));

			m_listCurrent.emplace(
				a_value.first,
				a_value.second,
				GetData(a_value.first));

			OnListSetHandleInternal(m_listCurrent->handle);
			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListClearCurrentItem()
		{
			const auto old(std::move(m_listCurrent));

			m_listCurrent.reset();

			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::OnListChangeCurrentItem(
			const std::optional<listValue_t>& a_oldHandle,
			const std::optional<listValue_t>& a_newHandle)
		{
		}

		template <class Td, detail::valid_list_handle Th>
		const ImVec4* UIListBase<Td, Th>::HighlightEntry(Th a_handle)
		{
			return nullptr;
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListReset()
		{
			m_listNextUpdateCurrent = false;
			m_listFirstUpdate       = false;
			m_listNextUpdate        = true;
			m_listData.clear();
			m_listCurrent.reset();
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListTick()
		{
			if (m_listNextUpdateCurrent)
			{
				m_listNextUpdateCurrent = false;
				ListUpdateCurrent();
			}

			if (m_listNextUpdate)
			{
				m_listNextUpdate = false;
				ListUpdate();

				if (m_desiredHandle)
				{
					ListSetCurrentItem(*m_desiredHandle);
					m_desiredHandle.reset();
				}
			}
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListUpdateCurrent()
		{
			if (m_listCurrent)
			{
				m_listCurrent->data = GetData(m_listCurrent->handle);
			}
		}

		template <class Td, detail::valid_list_handle Th>
		void UIListBase<Td, Th>::ListDrawInfo(const listValue_t& a_entry)
		{
			ImGui::SameLine();

			ImGui::TextDisabled("[?]");
			if (ImGui::IsItemHovered())
			{
				ImGui::PushID("info_text");
				ImGui::SetNextWindowSizeConstraints({ 400, 0 }, { 800, -1 });

				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(-1);

				if (ImGui::BeginTable(
						"table",
						2,
						ImGuiTableFlags_NoSavedSettings |
							ImGuiTableFlags_SizingStretchProp,
						{ -1.0f, 0.f }))
				{
					ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.33f);
					ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.67f);

					ListDrawInfoText(a_entry);

					ImGui::EndTable();
				}

				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();

				ImGui::PopID();
			}
		}

	}
}