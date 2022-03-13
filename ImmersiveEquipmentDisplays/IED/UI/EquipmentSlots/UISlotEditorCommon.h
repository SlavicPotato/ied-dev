#pragma once

#include "IED/UI/UICommon.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "IED/ConfigStore.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "UISlotEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class UISlotImportFlags : std::uint32_t
		{
			kNone       = 0,
			kEverything = 1u << 0,
			kOverrides  = 1u << 1,

			kAll = kEverything | kOverrides
		};

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
				bool        a_disableApply = false);

		protected:
			void UpdateConfigSingleSlot(
				T                                   a_handle,
				const SingleSlotConfigUpdateParams* a_params,
				bool                                a_syncSex);

			bool ResetConfigSlot(
				T                      a_handle,
				Data::ObjectSlot       a_slot,
				Data::configMapSlot_t& a_storage);

			bool ResetConfig(
				T                      a_handle,
				Data::configMapSlot_t& a_storage);

			void UpdateConfigFromProfile(
				T                               a_handle,
				const Data::configSlotHolder_t& a_data,
				bool                            a_resetEmpty);

			void DoMerge(
				T                               a_handle,
				const Data::configSlotHolder_t& a_data);

			void DoApply(
				T                               a_handle,
				const Data::configSlotHolder_t& a_data);

			void UpdateConfig(
				T                      a_handle,
				const entrySlotData_t& a_data);

		private:
			virtual void DrawMainHeaderControlsExtra(
				T                a_handle,
				entrySlotData_t& a_data) override;

			virtual Data::configSlotHolder_t&
				GetOrCreateConfigSlotHolder(T a_handle) const = 0;

			virtual entrySlotData_t GetCurrentData(
				T a_handle) = 0;

			virtual SlotProfile::base_type
				GetData(const profileSelectorParamsSlot_t<T>& a_data) override;

			virtual constexpr bool BaseConfigStoreCC() const override;

			virtual bool DrawProfileImportOptions(
				const profileSelectorParamsSlot_t<T>& a_data,
				const SlotProfile&                    a_profile) override;

			virtual void ResetProfileImportOptions() override;

			template <class Tf>
			void PartialImport(
				T                               a_handle,
				const Data::configSlotHolder_t& a_data,
				Tf                              a_func);

			stl::flag<UISlotImportFlags> m_importFlags{ UISlotImportFlags::kEverything };
		};

		DEFINE_ENUM_CLASS_BITWISE(UISlotImportFlags);

		template <class T>
		UISlotEditorCommon<T>::UISlotEditorCommon(
			Controller& a_controller,
			bool        a_disableApply) :
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
			T                                   a_handle,
			const SingleSlotConfigUpdateParams* a_params,
			bool                                a_syncSex)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);
			auto& slot   = holder.get(a_params->slot);

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
			T                      a_handle,
			Data::ObjectSlot       a_slot,
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
			T                      a_handle,
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
			T                               a_handle,
			const Data::configSlotHolder_t& a_data,
			bool                            a_resetEmpty)
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
		void UISlotEditorCommon<T>::DoMerge(
			T                               a_handle,
			const Data::configSlotHolder_t& a_data)
		{
			if (m_importFlags.test(UISlotImportFlags::kEverything))
			{
				UpdateConfigFromProfile(a_handle, a_data, false);
			}
			else
			{
				if (m_importFlags.test(UISlotImportFlags::kOverrides))
				{
					PartialImport(
						a_handle,
						a_data,
						[](
							auto& a_currentData,
							auto  a_slotId,
							auto& a_dstSlot,
							auto  a_sex,
							auto& a_data) {
							for (auto& e : a_data.equipmentOverrides)
							{
								if (!a_dstSlot)
								{
									if (auto& cSlot = a_currentData.get(a_slotId))
									{
										a_dstSlot =
											std::make_unique<Data::configSlotHolder_t::data_type>(cSlot->second);
									}
									else
									{
										continue;
									}
								}

								auto& dst = a_dstSlot->get(a_sex).equipmentOverrides;

								auto it = std::find_if(
									dst.begin(),
									dst.end(),
									[&](auto& a_e) { return a_e.description == e.description; });

								if (it != dst.end())
								{
									continue;
								}

								dst.emplace_back(e);
							}
						});
				}
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::DoApply(
			T                               a_handle,
			const Data::configSlotHolder_t& a_data)
		{
			if (m_importFlags.test(UISlotImportFlags::kEverything))
			{
				UpdateConfigFromProfile(a_handle, a_data, true);
			}
			else
			{
				if (m_importFlags.test(UISlotImportFlags::kOverrides))
				{
					PartialImport(
						a_handle,
						a_data,
						[](
							auto& a_currentData,
							auto  a_slotId,
							auto& a_dstSlot,
							auto  a_sex,
							auto& a_data) {
							if (!a_dstSlot)
							{
								if (auto& cSlot = a_currentData.get(a_slotId))
								{
									a_dstSlot =
										std::make_unique<Data::configSlotHolder_t::data_type>(cSlot->second);
								}
								else
								{
									return;
								}
							}

							a_dstSlot->get(a_sex).equipmentOverrides = a_data.equipmentOverrides;
						});
				}
			}
		}

		template <class T>
		void UISlotEditorCommon<T>::UpdateConfig(
			T                      a_handle,
			const entrySlotData_t& a_data)
		{
			a_data.copy_cc(
				GetConfigClass(),
				GetOrCreateConfigSlotHolder(a_handle));
		}

		template <class T>
		void UISlotEditorCommon<T>::DrawMainHeaderControlsExtra(
			T                a_handle,
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

		template <class T>
		bool UISlotEditorCommon<T>::DrawProfileImportOptions(
			const profileSelectorParamsSlot_t<T>& a_data,
			const SlotProfile&                    a_profile)
		{
			ImGui::PushID("import_opts");

			ImGui::CheckboxFlagsT(
				LS(CommonStrings::All, "1"),
				stl::underlying(std::addressof(m_importFlags.value)),
				stl::underlying(UISlotImportFlags::kEverything));

			bool disabled = m_importFlags.test(UISlotImportFlags::kEverything);

			UICommon::PushDisabled(disabled);

			ImGui::CheckboxFlagsT(
				LS(CommonStrings::Overrides, "2"),
				stl::underlying(std::addressof(m_importFlags.value)),
				stl::underlying(UISlotImportFlags::kOverrides));

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			return m_importFlags.test_any(UISlotImportFlags::kAll);
		}

		template <class T>
		void UISlotEditorCommon<T>::ResetProfileImportOptions()
		{
			m_importFlags = UISlotImportFlags::kEverything;
		}

		template <class T>
		template <class Tf>
		void UISlotEditorCommon<T>::PartialImport(
			T                               a_handle,
			const Data::configSlotHolder_t& a_data,
			Tf                              a_func)
		{
			auto& holder = GetOrCreateConfigSlotHolder(a_handle);

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			auto cd = GetCurrentData(a_handle);

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				const auto slotId = static_cast<Data::ObjectSlot>(i);

				auto& srcSlot = a_data.get(slotId);
				auto& dstSlot = holder.get(slotId);

				if (srcSlot)
				{
					srcSlot->visit2(
						[&](Data::ConfigSex a_sex,
					        auto&           a_data) {
							a_func(cd, slotId, dstSlot, a_sex, a_data);
						});
				}
			}
		}

	}
}