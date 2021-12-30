#pragma once

#include "../../UICommon.h"
#include "../Filters/UIGenericFilter.h"

namespace IED
{
	namespace UI
	{
		template <class T, class P>
		class UIListBase
		{
			using list_type = std::map<P, stl::fixed_string>;

		public:
			inline void QueueListUpdateCurrent()
			{
				m_listNextUpdateCurrent = true;
			}

			inline void QueueListUpdate() { m_listNextUpdate = true; }
			inline void QueueListUpdate(typename P a_desiredHandle)
			{
				m_listNextUpdate = true;
				m_desiredHandle = a_desiredHandle;
			}

		protected:
			struct listValue_t
			{
				P handle;
				std::string desc;
				T data;
			};

			virtual void ListTick();
			virtual void ListReset();

			void ListUpdateCurrent();
			void ListDrawInfo(const listValue_t& a_entry);

			UIListBase(float a_itemWidthScalar = -12.0f) noexcept;
			virtual ~UIListBase() noexcept = default;

			virtual void ListDraw();

			virtual void ListDrawOptions();
			virtual void ListDrawExtraControls();

			virtual void ListDrawInfoText(const listValue_t& a_entry) = 0;

			virtual listValue_t* ListGetSelected();

			virtual bool ListSetCurrentItem(
				P a_handle);

			virtual void ListSetCurrentItem(
				const typename list_type::value_type& a_value);

			virtual void ListClearCurrentItem();

			virtual void ListUpdate() = 0;

			virtual void ListResetAllValues(
				P a_handle) = 0;

			[[nodiscard]] virtual T GetData(
				P a_formid) = 0;

			[[nodiscard]] virtual const SetObjectWrapper<Game::FormID>&
				GetCrosshairRef() = 0;

			virtual void OnListChangeCurrentItem(
				const SetObjectWrapper<listValue_t>& a_oldHandle,
				const SetObjectWrapper<listValue_t>& a_newHandle);

			bool ListSelectFirstAvailable();
			void ListFilterSelected();

			bool m_listFirstUpdate{ false };
			bool m_listNextUpdateCurrent{ false };
			bool m_listNextUpdate{ true };

			list_type m_listData;
			SetObjectWrapper<listValue_t> m_listCurrent;
			SetObjectWrapper<P> m_desiredHandle;

			char m_listBuf1[256]{ 0 };
			UIGenericFilter m_listFilter;
			float m_itemWidthScalar;
		};

		template <class T, class P>
		UIListBase<T, P>::UIListBase(float a_itemWidthScalar) noexcept :
			m_itemWidthScalar(a_itemWidthScalar)
		{
		}

		template <class T, class P>
		void UIListBase<T, P>::ListDraw()
		{
			ListFilterSelected();

			ImGui::PushID("__base_list");

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

					ImGui::PushID(e.first);

					bool selected = m_listCurrent && e.first == m_listCurrent->handle;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(e.second.c_str(), selected))
					{
						newItem = std::addressof(e);
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

		template <class T, class P>
		void UIListBase<T, P>::ListDrawOptions()
		{}

		template <class T, class P>
		void UIListBase<T, P>::ListDrawExtraControls()
		{
		}

		template <class T, class P>
		bool UIListBase<T, P>::ListSelectFirstAvailable()
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

		template <class T, class P>
		void UIListBase<T, P>::ListFilterSelected()
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

		template <class T, class P>
		auto UIListBase<T, P>::ListGetSelected() -> listValue_t*
		{
			if (!m_listCurrent)
			{
				return nullptr;
			}

			/*if (!m_listCurrent->data) {
          m_listCurrent->data = GetData(m_listCurrent->handle);
      }*/

			return std::addressof(*m_listCurrent);
		}

		template <class T, class P>
		bool UIListBase<T, P>::ListSetCurrentItem(P a_handle)
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

		template <class T, class P>
		void UIListBase<T, P>::ListSetCurrentItem(
			const typename list_type::value_type& a_value)
		{
			auto old(std::move(m_listCurrent));

			m_listCurrent.emplace(
				a_value.first,
				a_value.second,
				GetData(a_value.first));

			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class T, class P>
		void UIListBase<T, P>::ListClearCurrentItem()
		{
			auto old(std::move(m_listCurrent));

			m_listCurrent.reset();

			OnListChangeCurrentItem(old, m_listCurrent);
		}

		template <class T, class P>
		void UIListBase<T, P>::OnListChangeCurrentItem(
			const SetObjectWrapper<listValue_t>& a_oldHandle,
			const SetObjectWrapper<listValue_t>& a_newHandle)
		{}

		template <class T, class P>
		void UIListBase<T, P>::ListReset()
		{
			m_listNextUpdateCurrent = false;
			m_listFirstUpdate = false;
			m_listNextUpdate = true;
			m_listData.clear();
		}

		template <class T, class P>
		void UIListBase<T, P>::ListTick()
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

		template <class T, class P>
		void UIListBase<T, P>::ListUpdateCurrent()
		{
			if (!m_listCurrent)
			{
				return;
			}

			m_listCurrent->data = GetData(m_listCurrent->handle);
		}

		template <class T, class P>
		void UIListBase<T, P>::ListDrawInfo(const listValue_t& a_entry)
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