#pragma once

#include "../UICommon.h"

#include "IED/ConfigOverride.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "../Widgets/UIProfileSelectorWidget.h"
#include "UISlotEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UISlotEditorCommon :
			public UISlotEditorBase<T>,
			public UIProfileSelectorWidget<profileSelectorParamsSlot_t<T>, SlotProfile>
		{
		public:
			UISlotEditorCommon(
				Controller& a_controller);

		protected:
			void UpdateConfigSingleSlot(
				T a_handle,
				const SingleSlotConfigUpdateParams* a_params,
				bool a_syncSex);

			bool ResetConfigSlot(
				T a_handle,
				Data::ObjectSlot a_slot,
				Data::configMapSlot_t& a_storage);

			bool ResetConfig(
				T a_handle,
				Data::configMapSlot_t& a_storage);

			void UpdateConfigFromProfile(
				T a_handle,
				const Data::configSlotHolder_t& a_data,
				bool a_resetEmpty);

			void UpdateConfig(
				T a_handle,
				const Data::configStoreSlot_t::result_copy& a_data);

		private:
			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				Data::configStoreSlot_t::result_copy& a_data) override;

			virtual Data::configSlotHolder_t&
				GetOrCreateConfigSlotHolder(T a_handle) const = 0;

			virtual const SlotProfile::base_type&
				GetData(const profileSelectorParamsSlot_t<T>& a_data) override;

			virtual constexpr bool BaseConfigStoreCC() const override;

			SlotProfile::base_type m_tempData;
		};

		template <class T>
		UISlotEditorCommon<T>::UISlotEditorCommon(
			Controller& a_controller) :
			UISlotEditorBase<T>(a_controller),
			UIProfileSelectorWidget<profileSelectorParamsSlot_t<T>, SlotProfile>(
				a_controller,
				UIProfileSelectorFlags::kEnableMerge)
		{}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfigSingleSlot(
			T a_handle,
			const SingleSlotConfigUpdateParams* a_params,
			bool a_syncSex)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);
			auto& slot = holder.get(a_params->slot);

			auto& src = a_params->entry.data->get(a_params->sex);

			if (a_syncSex)
			{
				a_params->entry.data->get(Data::GetOppositeSex(a_params->sex)) = src;
			}

			if (!slot)
			{
				slot = std::make_unique<Data::configSlotHolder_t::data_type>(
					*a_params->entry.data);

				a_params->entry.conf_class = GetConfigClass();
			}
			else
			{
				if (a_syncSex)
				{
					*slot = *a_params->entry.data;
				}
				else
				{
					slot->get(a_params->sex) = src;
				}
			}
		}

		template <class T>
		bool UISlotEditorCommon<T>::ResetConfigSlot(
			T a_handle,
			Data::ObjectSlot a_slot,
			Data::configMapSlot_t& a_storage)
		{
			auto it = a_storage.find(a_handle);
			if (it == a_storage.end())
			{
				return false;
			}

			it->second.get(a_slot).reset();

			if (it->second.empty())
			{
				a_storage.erase(it);
			}

			return true;
		}

		template <class T>
		bool UISlotEditorCommon<T>::ResetConfig(
			T a_handle,
			Data::configMapSlot_t& a_storage)
		{
			auto it = a_storage.find(a_handle);
			if (it != a_storage.end())
			{
				a_storage.erase(it);
				return true;
			}
			else
			{
				return false;
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfigFromProfile(
			T a_handle,
			const Data::configSlotHolder_t& a_data,
			bool a_resetEmpty)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				const auto slotId = static_cast<Data::ObjectSlot>(i);

				auto& sourceSlot = a_data.get(slotId);
				auto& holderSlot = holder.get(slotId);

				if (holderSlot)
				{
					if (sourceSlot)
					{
						*holderSlot = *sourceSlot;
					}
					else
					{
						if (a_resetEmpty)
						{
							holderSlot.reset();
						}
					}
				}
				else
				{
					if (sourceSlot)
					{
						holderSlot =
							std::make_unique<Data::configSlotHolder_t::data_type>(*sourceSlot);
					}
				}
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfig(
			T a_handle,
			const Data::configStoreSlot_t::result_copy& a_data)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);

			auto configClass = GetConfigClass();

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				auto& sourceEntry = a_data.entries[i];

				if (!sourceEntry.data)
				{
					continue;
				}

				if (sourceEntry.conf_class != configClass)
				{
					continue;
				}

				auto& holderSlot = holder.get(static_cast<Data::ObjectSlot>(i));

				if (holderSlot)
				{
					*holderSlot = *sourceEntry.data;
				}
				else
				{
					holderSlot = std::make_unique<Data::configSlotHolder_t::data_type>(
						*sourceEntry.data);
				}
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::DrawMainHeaderControlsExtra(
			T a_handle,
			Data::configStoreSlot_t::result_copy& a_data)
		{
			ImGui::Separator();

			if (TreeEx(
					"tree_prof",
					false,
					"%s",
					LS(CommonStrings::Profile)))
			{
				ImGui::Spacing();

				profileSelectorParamsSlot_t psParams{ a_handle, a_data };

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				DrawProfileSelector(psParams);

				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		const SlotProfile::base_type& UISlotEditorCommon<T>::GetData(
			const profileSelectorParamsSlot_t<T>& a_data)
		{
			m_tempData = {};

			auto configClass = GetConfigClass();

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				auto& v = a_data.data.entries[i];

				if (v.data && v.conf_class == configClass)
				{
					m_tempData.get(static_cast<Data::ObjectSlot>(i)) =
						std::make_unique<SlotProfile::base_type::data_type>(*v.data);
				}
			}

			return m_tempData;
		}

		template <class T>
		inline constexpr bool UISlotEditorCommon<T>::BaseConfigStoreCC() const
		{
			return true;
		}

	}  // namespace UI
}  // namespace IED