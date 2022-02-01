#pragma once

#include "IED/UI/UICommon.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "IED/ConfigOverride.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "UISlotEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UISlotEditorCommon :
			public UISlotEditorBase<T>,
			public UIProfileSelectorWidget<
				profileSelectorParamsSlot_t<T>,
				SlotProfile>
		{
		public:
			UISlotEditorCommon(
				Controller& a_controller,
				bool a_disableApply = false);

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
				const entrySlotData_t& a_data);

		private:
			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				entrySlotData_t& a_data) override;

			virtual Data::configSlotHolder_t&
				GetOrCreateConfigSlotHolder(T a_handle) const = 0;

			virtual SlotProfile::base_type
				GetData(const profileSelectorParamsSlot_t<T>& a_data) override;

			virtual constexpr bool BaseConfigStoreCC() const override;
		};

		template <class T>
		UISlotEditorCommon<T>::UISlotEditorCommon(
			Controller& a_controller,
			bool a_disableApply) :
			UISlotEditorBase<T>(a_controller),
			UIProfileSelectorWidget<
				profileSelectorParamsSlot_t<T>,
				SlotProfile>(
				a_controller,
				UIProfileSelectorFlags::kEnableMerge |
					(!a_disableApply ?
                         UIProfileSelectorFlags::kEnableApply :
                         UIProfileSelectorFlags::kNone))
		{
		}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfigSingleSlot(
			T a_handle,
			const SingleSlotConfigUpdateParams* a_params,
			bool a_syncSex)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);
			auto& slot = holder.get(a_params->slot);

			auto& src = a_params->entry.second.get(a_params->sex);

			if (a_syncSex)
			{
				a_params->entry.second.get(Data::GetOppositeSex(a_params->sex)) = src;
			}

			a_params->entry.first = GetConfigClass();

			if (!slot)
			{
				slot = std::make_unique<Data::configSlotHolder_t::data_type>(
					a_params->entry.second);
			}
			else
			{
				if (a_syncSex)
				{
					*slot = a_params->entry.second;
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

				auto& srcSlot = a_data.get(slotId);
				auto& dstSlot = holder.get(slotId);

				if (dstSlot)
				{
					if (srcSlot)
					{
						*dstSlot = *srcSlot;
					}
					else
					{
						if (a_resetEmpty)
						{
							dstSlot.reset();

							/*if (a_defaultData)
							{
								if (auto& srcSlotDef = a_defaultData->get(slotId))
								{
									if (dstSlot)
									{
										*dstSlot = *srcSlotDef;
									}
									else
									{
										dstSlot =
											std::make_unique<Data::configSlotHolder_t::data_type>(*srcSlotDef);
									}
								}
								else
								{
									dstSlot = CreateDefaultSlotConfig(slotId);
								}
							}
							else
							{
								dstSlot.reset();
							}*/
						}
					}
				}
				else
				{
					if (srcSlot)
					{
						dstSlot =
							std::make_unique<Data::configSlotHolder_t::data_type>(*srcSlot);
					}
				}
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfig(
			T a_handle,
			const entrySlotData_t& a_data)
		{
			a_data.copy_cc(
				GetConfigClass(),
				GetOrCreateConfigSlotHolder(a_handle));
		}

		template <class T>
		void UISlotEditorCommon<T>::DrawMainHeaderControlsExtra(
			T a_handle,
			entrySlotData_t& a_data)
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
		SlotProfile::base_type UISlotEditorCommon<T>::GetData(
			const profileSelectorParamsSlot_t<T>& a_data)
		{
			return a_data.data.copy_cc(GetConfigClass());
		}

		template <class T>
		inline constexpr bool UISlotEditorCommon<T>::BaseConfigStoreCC() const
		{
			return true;
		}

	}  // namespace UI
}  // namespace IED