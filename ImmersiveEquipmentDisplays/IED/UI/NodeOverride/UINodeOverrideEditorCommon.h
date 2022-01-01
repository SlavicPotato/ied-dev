#pragma once

#include "IED/ConfigOverride.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "../Widgets/UIProfileSelectorWidget.h"
#include "Widgets/UINodeOverrideEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UINodeOverrideEditorCommon :
			public UINodeOverrideEditorWidget<T>,
			public UIProfileSelectorWidget<
				profileSelectorParamsNodeOverride_t<T>,
				NodeOverrideProfile>
		{
		public:
			UINodeOverrideEditorCommon(
				Controller& a_controller);

		protected:
			template <class Td>
			void UpdateConfig(
				T a_handle,
				const Td& a_params,
				bool a_syncSex);

			template <class Td>
			bool EraseConfig(
				T a_handle,
				Data::configMapNodeOverrides_t& a_map,
				const stl::fixed_string& a_name);

			template <class Td>
			void PostClear(
				const Td& a_data,
				Td& a_workingData,
				const stl::fixed_string& a_name);

			void DrawPlayerDisabledWarning();

		private:
			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				entryNodeOverrideData_t& a_data) override;

			virtual Data::configNodeOverrideHolder_t&
				GetOrCreateConfigHolder(T a_handle) const = 0;
		};

		template <class T>
		UINodeOverrideEditorCommon<T>::UINodeOverrideEditorCommon(
			Controller& a_controller) :
			UINodeOverrideEditorWidget<T>(a_controller),
			UIProfileSelectorWidget<
				profileSelectorParamsNodeOverride_t<T>,
				NodeOverrideProfile>(
				a_controller,
				UIProfileSelectorFlags::kEnableApply |
					UIProfileSelectorFlags::kEnableMerge)
		{
		}

		template <class T>
		template <class Td>
		void UINodeOverrideEditorCommon<T>::UpdateConfig(
			T a_handle,
			const Td& a_params,
			bool a_syncSex)
		{
			auto& conf = GetOrCreateConfigHolder(a_handle);
			auto& confEntry = conf.get_data<decltype(a_params.entry)>().try_emplace(a_params.name).first->second;

			if (a_syncSex)
			{
				auto og = Data::GetOppositeSex(a_params.sex);

				a_params.entry(og) = a_params.entry(a_params.sex);
				confEntry = a_params.entry;
			}
			else
			{
				confEntry(a_params.sex) = a_params.entry(a_params.sex);
			}
		}

		template <class T>
		template <class Td>
		bool UINodeOverrideEditorCommon<T>::EraseConfig(
			T a_handle,
			Data::configMapNodeOverrides_t& a_map,
			const stl::fixed_string& a_name)
		{
			auto it = a_map.find(a_handle);
			if (it != a_map.end())
			{
				bool r = it->second.get_data<Td>().erase(a_name) > 0;

				if (it->second.empty())
				{
					a_map.erase(it);
				}

				return r;
			}
			else
			{
				return false;
			}
		}

		template <class T>
		template <class Td>
		void UINodeOverrideEditorCommon<T>::PostClear(
			const Td& a_data,
			Td& a_workingData,
			const stl::fixed_string& a_name)
		{
			if (auto it = a_data.find(a_name); it != a_data.end())
			{
				a_workingData.insert_or_assign(a_name, it->second);
			}
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::DrawPlayerDisabledWarning()
		{
			ImGui::TextColored(
				UICommon::g_colorWarning,
				"%s",
				LS(UINodeOverrideEditorStrings::PlayerDisabledWarning));

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::DrawMainHeaderControlsExtra(
			T a_handle,
			entryNodeOverrideData_t& a_data)
		{
			ImGui::Separator();

			if (TreeEx(
					"tree_prof",
					false,
					"%s",
					LS(CommonStrings::Profile)))
			{
				ImGui::Spacing();

				profileSelectorParamsNodeOverride_t params{ a_handle, a_data };

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				DrawProfileSelector(params);

				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}
	}
}