#include "pch.h"

#include "UICollapsibles.h"

#include "IED/UI/UICommon.h"

#include "IED/UI/UIData.h"

namespace IED
{
	namespace UI
	{
		bool UICollapsibles::CollapsingHeader(
			const char* a_label,
			bool a_default)
		{
			auto window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return false;
			}

			auto& data = GetCollapsibleStatesData();

			auto id = window->GetID(a_label);

			auto& state = data.get(id, a_default);

			ImGuiTreeNodeFlags flags(ImGuiTreeNodeFlags_CollapsingHeader);

			if (state)
			{
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			bool newState = ImGui::TreeNodeBehavior(
				id,
				flags,
				a_label);

			if (state != newState)
			{
				state = newState;
				OnCollapsibleStatesUpdate();
			}

			return newState;
		}

		bool UICollapsibles::Tree(
			const char* a_label,
			bool a_default,
			bool a_framed)
		{
			auto window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return false;
			}

			auto& data = GetCollapsibleStatesData();

			auto id = window->GetID(a_label);

			auto& state = data.get(id, a_default);

			ImGuiTreeNodeFlags flags(ImGuiTreeNodeFlags_SpanAvailWidth);

			if (state)
			{
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}

			if (a_framed)
			{
				flags |= ImGuiTreeNodeFlags_Framed;
			}

			bool newState = ImGui::TreeNodeBehavior(id, flags, a_label);

			if (state != newState)
			{
				state = newState;
				OnCollapsibleStatesUpdate();
			}

			return newState;
		}

		bool UICollapsibles::EraseCollapsibleEntry(
			ImGuiID a_key)
		{
			auto& data = GetCollapsibleStatesData();

			bool r = data.erase(a_key);

			if (r)
			{
				OnCollapsibleStatesUpdate();
			}

			return r;
		}

	}
}