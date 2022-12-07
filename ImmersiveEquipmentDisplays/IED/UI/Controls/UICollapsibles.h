#pragma once

#include "IED/UI/UIData.h"

namespace IED
{
	namespace UI
	{
		class UICollapsibles
		{
		protected:
			bool CollapsingHeader(
				const char* a_label,
				bool        a_default = true);

			bool Tree(
				const char* a_label,
				bool        a_default = true,
				bool        a_framed  = false);

			template <class Ti, class... Args>
			bool TreeEx(
				Ti          a_id,
				bool        a_default,
				const char* a_fmt,
				Args... a_args);

			template <class Ti, class... Args>
			bool CollapsingHeader(
				Ti          a_id,
				bool        a_default,
				const char* a_fmt,
				Args... a_args);

			bool EraseCollapsibleEntry(
				ImGuiID a_key);

		private:
			template <class Ti, class... Args>
			bool TreeNodeBehaviorExImpl(
				Ti                 a_id,
				bool               a_default,
				ImGuiTreeNodeFlags a_flags,
				const char*        a_fmt,
				Args... a_args);

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData()  = 0;
			virtual void                         OnCollapsibleStatesUpdate() = 0;
		};

		template <class Ti, class... Args>
		bool UICollapsibles::TreeEx(
			Ti          a_id,
			bool        a_default,
			const char* a_fmt,
			Args... a_args)
		{
			return TreeNodeBehaviorExImpl(
				a_id,
				a_default,
				ImGuiTreeNodeFlags_SpanAvailWidth,
				a_fmt,
				a_args...);
		}

		template <class Ti, class... Args>
		bool UICollapsibles::CollapsingHeader(
			Ti          a_id,
			bool        a_default,
			const char* a_fmt,
			Args... a_args)
		{
			return TreeNodeBehaviorExImpl(
				a_id,
				a_default,
				ImGuiTreeNodeFlags_CollapsingHeader,
				a_fmt,
				a_args...);
		}

		template <class Ti, class... Args>
		bool UICollapsibles::TreeNodeBehaviorExImpl(
			Ti                 a_id,
			bool               a_default,
			ImGuiTreeNodeFlags a_flags,
			const char*        a_fmt,
			Args... a_args)
		{
			auto window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return false;
			}

			auto& data = GetCollapsibleStatesData();

			auto id = window->GetID(a_id);

			auto& state = data.get(id, a_default);

			if (state)
			{
				a_flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			auto context = ImGui::GetCurrentContext();

			_snprintf_s(
				context->TempBuffer.Data,
				_TRUNCATE,
				context->TempBuffer.Size,
				a_fmt,
				a_args...);

			bool newState = ImGui::TreeNodeBehavior(
				id,
				a_flags,
				context->TempBuffer.Data);

			if (state != newState)
			{
				state = newState;
				OnCollapsibleStatesUpdate();
			}

			return newState;
		}

	}
}