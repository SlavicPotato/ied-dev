#pragma once

#include "IED/ConfigStore.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "IED/UI/Widgets/UIProfileSelectorWidget.h"
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
			void UpdateConfigSingle(
				T         a_handle,
				const Td& a_params,
				bool      a_syncSex);

			void UpdateConfig(
				T                               a_handle,
				const NodeOverrideUpdateParams& a_params);

			template <class Td>
			bool EraseConfig(
				T                               a_handle,
				Data::configMapNodeOverrides_t& a_map,
				const stl::fixed_string&        a_name);

			template <class Td>
			void PostClear(
				const Td&                a_data,
				Td&                      a_workingData,
				const stl::fixed_string& a_name);

			void MergeProfileData(
				const profileSelectorParamsNodeOverride_t<T>& a_data,
				const NodeOverrideProfile&                    a_profile);

			void DrawPlayerDisabledWarning();

		private:
			virtual void DrawMainHeaderControlsExtra(
				T                        a_handle,
				entryNodeOverrideData_t& a_data) override;

			virtual Data::configNodeOverrideHolder_t&
				GetOrCreateConfigHolder(T a_handle) const = 0;

			virtual NodeOverrideProfile::base_type GetData(
				const profileSelectorParamsNodeOverride_t<T>& a_params) override;
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
		void UINodeOverrideEditorCommon<T>::UpdateConfigSingle(
			T         a_handle,
			const Td& a_params,
			bool      a_syncSex)
		{
			auto& confEntry = GetOrCreateConfigHolder(a_handle)
			                      .get_data<decltype(a_params.entry.second)>()
			                      .try_emplace(a_params.name)
			                      .first->second;

			if (a_syncSex)
			{
				auto og = Data::GetOppositeSex(a_params.sex);

				a_params.entry.second(og) = a_params.entry.second(a_params.sex);
				confEntry                 = a_params.entry.second;
			}
			else
			{
				confEntry(a_params.sex) = a_params.entry.second(a_params.sex);
			}

			a_params.entry.first = GetConfigClass();
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::UpdateConfig(
			T                               a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			a_params.data.copy_cc(
				GetConfigClass(),
				GetOrCreateConfigHolder(a_handle));
		}

		template <class T>
		template <class Td>
		bool UINodeOverrideEditorCommon<T>::EraseConfig(
			T                               a_handle,
			Data::configMapNodeOverrides_t& a_map,
			const stl::fixed_string&        a_name)
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
			const Td&                a_data,
			Td&                      a_workingData,
			const stl::fixed_string& a_name)
		{
			if (auto it = a_data.find(a_name); it != a_data.end())
			{
				a_workingData.emplace(a_name, it->second);
			}
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::MergeProfileData(
			const profileSelectorParamsNodeOverride_t<T>& a_data,
			const NodeOverrideProfile&                    a_profile)
		{
			auto& conf  = GetOrCreateConfigHolder(a_data.handle);
			auto& pdata = a_profile.Data();

			for (auto& e : pdata.data)
			{
				conf.data.insert_or_assign(e.first, e.second);
			}

			for (auto& e : pdata.placementData)
			{
				conf.placementData.insert_or_assign(e.first, e.second);
			}

			conf.flags = pdata.flags;
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::DrawPlayerDisabledWarning()
		{
			ImGui::TextColored(
				UICommon::g_colorWarning,
				"%s",
				LS(UINodeOverrideEditorWidgetStrings::PlayerDisabledWarning));

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		template <class T>
		void UINodeOverrideEditorCommon<T>::DrawMainHeaderControlsExtra(
			T                        a_handle,
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

		template <class T>
		NodeOverrideProfile::base_type UINodeOverrideEditorCommon<T>::GetData(
			const profileSelectorParamsNodeOverride_t<T>& a_params)
		{
			return a_params.data.copy_cc(GetConfigClass());
		}
	}
}