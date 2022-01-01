#pragma once

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		template <
			class Td,
			class Th>
		class UIListBase
		{
			using list_type = std::map<Th, stl::fixed_string>;

		public:
			inline void QueueListUpdateCurrent()
			{
				m_listNextUpdateCurrent = true;
			}

			inline void QueueListUpdate() { m_listNextUpdate = true; }
			inline void QueueListUpdate(typename Th a_desiredHandle)
			{
				m_listNextUpdate = true;
				m_desiredHandle = a_desiredHandle;
			}

		protected:
			struct listValue_t
			{
				Th handle;
				stl::fixed_string desc;
				mutable Td data{};
			};

			virtual void ListReset();
			virtual void ListTick();

			void ListUpdateCurrent();
			void ListDrawInfo(const listValue_t& a_entry);

			UIListBase(float a_itemWidthScalar = -12.0f) noexcept;
			virtual ~UIListBase() noexcept = default;

			virtual void ListDraw();

			virtual void ListDrawOptions();
			virtual void ListDrawExtraControls();

			virtual void ListDrawInfoText(const listValue_t& a_entry) = 0;

			inline constexpr const SetObjectWrapper<listValue_t>& ListGetSelected() const noexcept;

			virtual bool ListSetCurrentItem(
				Th a_handle);

			virtual void ListSetCurrentItem(
				const typename list_type::value_type& a_value);

			virtual void ListClearCurrentItem();

			virtual void ListUpdate() = 0;

			virtual void ListResetAllValues(
				Th a_handle) = 0;

			[[nodiscard]] virtual Td GetData(Th a_handle) = 0;

			virtual void OnListChangeCurrentItem(
				const SetObjectWrapper<listValue_t>& a_oldHandle,
				const SetObjectWrapper<listValue_t>& a_newHandle);

			virtual const ImVec4* HighlightEntry(Th a_handle);

			bool ListSelectFirstAvailable();
			void ListFilterSelected();

			bool m_listFirstUpdate{ false };
			bool m_listNextUpdateCurrent{ false };
			bool m_listNextUpdate{ true };

			list_type m_listData;
			SetObjectWrapper<listValue_t> m_listCurrent;
			SetObjectWrapper<Th> m_desiredHandle;

			char m_listBuf1[256]{ 0 };
			UIGenericFilter m_listFilter;
			float m_itemWidthScalar;

			static_assert(std::is_convertible_v<Th, std::uint64_t>);
		};

		template <class Td, class Th>
		UIListBase<Td, Th>::UIListBase(float a_itemWidthScalar) noexcept :
			m_itemWidthScalar(a_itemWidthScalar)
		{
		}

		template <class Td, class Th>
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
				const typename list_type::value_type* newItem = nullptr;

				for (const auto& e : m_listData)
				{
					if (!m_listFilter.Test(e.second))
					{
						continue;
					}

					auto highlight = HighlightEntry(e.first);

					ImGui::PushID(
						reinterpret_cast<const void*>(
							static_cast<std::uint64_t>(e.first)));

					bool selected = m_listCurrent && e.first == m_listCurrent->handle;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (highlight)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, *highlight);
					}

					if (ImGui::Selectable(e.second.c_str(), selected))
					{
						newItem = std::addressof(e);
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

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListDrawOptions()
		{
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListDrawExtraControls()
		{
		}

		template <class Td, class Th>
		bool UIListBase<Td, Th>::ListSelectFirstAvailable()
		{
			for (const auto& e : m_listData)
			{
				if (!m_listFilter.Test(e.second))
				{
					continue;
				}

				ListSetCurrentItem(e);

				return true;
			}

			return false;
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListFilterSelected()
		{
			if (m_listCurrent)
			{
				if (!m_listFilter.Test(m_listCurrent->desc))
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

		template <class Td, class Th>
		inline constexpr auto UIListBase<Td, Th>::ListGetSelected() const noexcept
			-> const SetObjectWrapper<listValue_t>&
		{
			return m_listCurrent;
		}

		template <class Td, class Th>
		bool UIListBase<Td, Th>::ListSetCurrentItem(Th a_handle)
		{
			auto it = m_listData.find(a_handle);
			if (it == m_listData.end())
			{
				return false;
			}

			auto old(std::move(m_listCurrent));

			m_listCurrent.emplace(a_handle, it->second, GetData(a_handle));

			OnListChangeCurrentItem(old, m_listCurrent);

			return true;
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListSetCurrentItem(
			const typename list_type::value_type& a_value)
		{
			auto old(std::move(m_listCurrent));

			m_listCurrent.emplace(
				a_value.first,
				a_value.second,
				GetData(a_value.first));

			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListClearCurrentItem()
		{
			auto old(std::move(m_listCurrent));

			m_listCurrent.reset();

			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::OnListChangeCurrentItem(
			const SetObjectWrapper<listValue_t>& a_oldHandle,
			const SetObjectWrapper<listValue_t>& a_newHandle)
		{
		}

		template <class Td, class Th>
		const ImVec4* UIListBase<Td, Th>::HighlightEntry(Th a_handle)
		{
			return nullptr;
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListReset()
		{
			m_listNextUpdateCurrent = false;
			m_listFirstUpdate = false;
			m_listNextUpdate = true;
			m_listData.clear();
		}

		template <class Td, class Th>
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
					m_desiredHandle.clear();
				}
			}
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListUpdateCurrent()
		{
			if (!m_listCurrent)
			{
				return;
			}

			m_listCurrent->data = GetData(m_listCurrent->handle);
		}

		template <class Td, class Th>
		void UIListBase<Td, Th>::ListDrawInfo(const listValue_t& a_entry)
		{
			ImGui::SameLine();

			ImGui::TextDisabled("[?]");
			if (ImGui::IsItemHovered())
			{
				ImGui::PushID("info_text");
				ImGui::SetNextWindowSizeConstraints({ 500, 0 }, { 800, -1 });

				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

				ListDrawInfoText(a_entry);

				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();

				ImGui::PopID();
			}
		}

	}  // namespace UI
}  // namespace IED