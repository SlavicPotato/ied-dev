#pragma once

#include "../UICommon.h"

#include "IED/ConfigOverride.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "../Widgets/UIProfileSelectorWidget.h"
#include "Widgets/UICustomEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UICustomEditorCommon :
			public UICustomEditorWidget<T>,
			public UIProfileSelectorWidget<profileSelectorParamsCustom_t<T>, CustomProfile>
		{
		public:
			UICustomEditorCommon(
				Controller& a_controller);

		protected:
			void UpdateConfig(
				T a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				bool a_syncSex);

			bool EraseConfig(
				Data::configCustomPluginMap_t& a_pluginMap,
				const stl::fixed_string& a_name);

			bool EraseConfig(
				T a_handle,
				Data::configMapCustom_t& a_map,
				const stl::fixed_string& a_name);

			bool DoConfigRename(
				T a_handle,
				const CustomConfigRenameParams& a_params);

		private:
			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				entryCustomData_t& a_data) override;

			virtual const CustomProfile::base_type&
				GetData(const profileSelectorParamsCustom_t<T>& a_handle) override;

			virtual Data::configCustomHolder_t&
				GetOrCreateConfigSlotHolder(T a_handle) const = 0;

			virtual constexpr bool BaseConfigStoreCC() const override;
		};

		template <class T>
		UICustomEditorCommon<T>::UICustomEditorCommon(
			Controller& a_controller) :
			UICustomEditorWidget<T>(a_controller),
			UIProfileSelectorWidget<profileSelectorParamsCustom_t<T>, CustomProfile>(
				a_controller,
				UIProfileSelectorFlags::kEnableApply |
					UIProfileSelectorFlags::kEnableMerge)
		{
		}

		template <class T>
		void UICustomEditorCommon<T>::UpdateConfig(
			T a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			bool a_syncSex)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);
			auto& confEntry = conf.data.try_emplace(a_params.name).first->second;

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
		bool UICustomEditorCommon<T>::EraseConfig(
			Data::configCustomPluginMap_t& a_pluginMap,
			const stl::fixed_string& a_name)
		{
			auto& sh = StringHolder::GetSingleton();

			auto it = a_pluginMap.find(sh.IED);
			if (it != a_pluginMap.end())
			{
				bool r = it->second.data.erase(a_name) > 0;

				if (it->second.data.empty())
				{
					a_pluginMap.erase(it);
				}

				return r;
			}
			else
			{
				return false;
			}
		}

		template <class T>
		bool UICustomEditorCommon<T>::EraseConfig(
			T a_handle,
			Data::configMapCustom_t& a_map,
			const stl::fixed_string& a_name)
		{
			auto it = a_map.find(a_handle.get());
			if (it != a_map.end())
			{
				bool r = EraseConfig(it->second, a_name);

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
		bool UICustomEditorCommon<T>::DoConfigRename(
			T a_handle,
			const CustomConfigRenameParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto ito = conf.data.find(a_params.oldName);
			if (ito == conf.data.end())
			{
				return false;
			}

			auto itn = conf.data.find(a_params.newName);
			if (itn != conf.data.end())
			{
				return false;
			}

			auto tmp = ito->second;

			conf.data.erase(ito);
			conf.data.insert_or_assign(a_params.newName, tmp);

			return true;
		}

		template <class T>
		void UICustomEditorCommon<T>::DrawMainHeaderControlsExtra(
			T a_handle,
			entryCustomData_t& a_data)
		{
			ImGui::Separator();

			if (TreeEx(
					"tree_prof",
					false,
					"%s",
					LS(CommonStrings::Profile)))
			{
				ImGui::Spacing();

				profileSelectorParamsCustom_t params{ a_handle, a_data };

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				DrawProfileSelector(params);

				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		const CustomProfile::base_type& UICustomEditorCommon<T>::GetData(
			const profileSelectorParamsCustom_t<T>& a_data)
		{
			return a_data.data;
		}

		template <class T>
		inline constexpr bool UICustomEditorCommon<T>::BaseConfigStoreCC() const
		{
			return false;
		}

	}

}