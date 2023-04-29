#pragma once

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/UIAllowedModelTypes.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UIPopupInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/UITips.h"

#include "Form/UIFormFilterWidget.h"
#include "Form/UIFormSelectorWidget.h"
#include "UIBaseConfigWidgetStrings.h"
#include "UIConditionParamEditorWidget.h"
#include "UIDescriptionPopup.h"
#include "UIEffectShaderEditorWidget.h"
#include "UIEquipmentOverrideConditionsWidget.h"
#include "UIEquipmentOverrideResult.h"
#include "UIEquipmentOverrideWidget.h"
#include "UIExtraLightEditorWidget.h"
#include "UINodeSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPhysicsValueEditorWidget.h"
#include "UIPopupToggleButtonWidget.h"
#include "UISimpleStringList.h"
#include "UISimpleStringSet.h"
#include "UITransformSliderWidget.h"
#include "UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/StringHolder.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"

#include "BaseConfigEditorAction.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		struct bcFormFilterParams_t
		{
			T           handle;
			const void* params;
		};

		template <class T>
		struct baseEffectShaderEditorParams_t
		{
			T                                 handle;
			const void*                       params;
			Data::configEffectShaderHolder_t& data;
		};

		template <class T>
		struct baseConfigEquipmentOverrideParams_t
		{
			T                       handle;
			const void*             params;
			const stl::fixed_string name;
		};

		template <class T>
		class UIBaseConfigWidget :
			UIEffectShaderEditorWidget<baseEffectShaderEditorParams_t<T>>,
			UIPhysicsValueEditorWidget,
			public UIEquipmentOverrideWidget<
				Data::equipmentOverrideList_t,
				baseConfigEquipmentOverrideParams_t<T>>,
			public UINodeSelectorWidget,
			public UIFormLookupInterface,
			public virtual UIEquipmentOverrideConditionsWidget,
			public virtual UIDescriptionPopupWidget,
			public virtual UITransformSliderWidget,
			public virtual UIPopupInterface,
			public virtual UISettingsInterface,
			public virtual UISimpleStringSetWidget,
			public virtual UISimpleStringListWidget
		{
		public:
			UIBaseConfigWidget(
				Controller& a_controller);

			void DrawBaseConfig(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

			void DrawBaseConfigValues(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				Data::configBase_t*       a_baseConfig);

		protected:
			void DrawBaseConfigValuesFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				Data::configBase_t*       a_baseConfig,
				const bool                a_disabled);

			void DrawBaseConfigValuesNode(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				Data::configBase_t*       a_baseConfig,
				const bool                a_disabled);

			void DrawBaseConfigValuesOther(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				Data::configBase_t*       a_baseConfig,
				const bool                a_disabled);

			BaseConfigEditorAction DrawPhysicsValuesContextMenu(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params);

			void DrawBaseConfigValuesPhysics(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				Data::configBase_t*       a_baseConfig,
				const bool                a_disabled);

			virtual void OnBaseConfigChange(
				T                a_handle,
				const void*      a_params,
				PostChangeAction a_action) = 0;

			template <class Tpv, class Tf>
			void PropagateMemberToEquipmentOverrides(
				Data::configBase_t* a_data,
				std::ptrdiff_t      a_offset,
				const Tpv&          a_value,
				Tf                  a_postWrite);

			template <class Tpv, class Tf>
			static void RecursivePropagateMemberToEquipmentOverrides(
				Data::equipmentOverrideList_t& a_list,
				std::ptrdiff_t                 a_offset,
				const Tpv&                     a_srcVal,
				Tf                             a_postWrite);

			template <class Tpv>
			void PropagateToEquipmentOverrides(
				Data::configBase_t* a_data);

			template <class Tpv>
			void RecursivePropagateToEquipmentOverrides(
				Data::equipmentOverrideList_t& a_list,
				const Tpv&                     a_src);

			void PropagateFlagToEquipmentOverrides(
				Data::configBase_t* a_data,
				Data::BaseFlags     a_mask);

			void RecursivePropagateFlagToEquipmentOverrides(
				Data::equipmentOverrideList_t& a_list,
				Data::BaseFlags                a_mask,
				Data::BaseFlags                a_flags);

		private:
			void DrawBaseConfigGeneralFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				bool                      a_disabled,
				Data::configBase_t*       a_baseConfig);

			void DrawBaseConfig3DFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				bool                      a_disabled,
				Data::configBase_t*       a_baseConfig);

			void DrawBaseConfigAnimationFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				bool                      a_disabled,
				Data::configBase_t*       a_baseConfig);

			void DrawBaseConfigLightFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				const void*               a_params,
				const stl::fixed_string&  a_slotName,
				bool                      a_storecc,
				bool                      a_disabled,
				Data::configBase_t*       a_baseConfig);

			virtual void DrawExtraFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				Data::configBase_t*       a_baseConfig,
				const void*               a_params);

			BaseConfigEditorAction DrawFiltersTreeContextMenu(
				T                   a_handle,
				Data::configBase_t& a_data,
				const void*         a_params);

			void DrawFiltersTree(
				T                   a_handle,
				Data::configBase_t& a_data,
				const void*         a_params);

			virtual constexpr bool BaseConfigStoreCC() const = 0;

			virtual void DrawEquipmentOverrideValues(
				const baseConfigEquipmentOverrideParams_t<T>& a_params,
				Data::configBaseValues_t&                     a_data) override;

			virtual constexpr bool EOStoreCC() const override;

			virtual void OnEquipmentOverrideChange(
				const baseConfigEquipmentOverrideParams_t<T>& a_params) override;

		protected:
		private:
			virtual bool GetEnableEquipmentOverridePropagation() = 0;

			BaseConfigEditorAction DrawEffectShaderTreeContextMenu(
				T                   a_handle,
				Data::configBase_t& a_data,
				const void*         a_params);

			void DrawEffectShaders(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

			UIEquipmentOverrideResult DrawEffectShaderHolderContextMenu(
				T                                 a_handle,
				const void*                       a_params,
				Data::effectShaderList_t&         a_list,
				Data::configEffectShaderHolder_t& a_data);

			void DrawEffectShaderHolderList(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

			void TriggerEffectShaderUpdate(
				T                                 a_handle,
				Data::configEffectShaderHolder_t& a_data,
				const void*                       a_params);

			virtual void OnEffectShaderUpdate(
				const baseEffectShaderEditorParams_t<T>& a_params,
				bool                                     a_updateTag) override;

		protected:
			virtual constexpr bool IsProfileEditor() const;

		private:
			UIFormSelectorWidget                        m_ffFormSelector;
			UIFormFilterWidget<bcFormFilterParams_t<T>> m_formFilter;
			UIFormPickerWidget                          m_formPicker;

			Controller& m_controller;
		};

		template <class T>
		UIBaseConfigWidget<T>::UIBaseConfigWidget(
			Controller& a_controller) :
			UINodeSelectorWidget(a_controller),
			UIFormLookupInterface(a_controller),
			m_controller(a_controller),
			m_ffFormSelector(a_controller, FormInfoFlags::kNone, true),
			m_formPicker(a_controller, FormInfoFlags::kValidCustom, true, true),
			m_formFilter(a_controller, m_ffFormSelector)
		{
			m_formFilter.SetOnChangeFunc([this](auto& a_params) {
				OnBaseConfigChange(
					a_params.handle,
					a_params.params,
					PostChangeAction::Evaluate);
			});

			m_formPicker.SetAllowedTypes(g_allowedModelTypes);
		}

		template <class T>
		inline void UIBaseConfigWidget<T>::DrawBaseConfig(
			T                        a_handle,
			Data::configBase_t&      a_data,
			const void*              a_params,
			const stl::fixed_string& a_slotName)
		{
			ImGui::PushID(a_slotName.c_str());

			ImGui::PushID("base_config_values");

			DrawBaseConfigValues(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				true,
				std::addressof(a_data));

			ImGui::PopID();

			const baseConfigEquipmentOverrideParams_t<T> eoparams{
				a_handle,
				a_params,
				a_slotName
			};

			this->DrawEquipmentOverrides(
				a_data,
				a_data.equipmentOverrides,
				eoparams);

			DrawEffectShaders(
				a_handle,
				a_data,
				a_params,
				a_slotName);

			const bool disabled = a_data.flags.test(Data::BaseFlags::kDisabled) &&
			                      a_data.equipmentOverrides.empty();

			UICommon::PushDisabled(disabled);

			DrawFiltersTree(a_handle, a_data, a_params);

			if (DrawStringSetTree(
					"bc_hkx_flt",
					static_cast<Localization::StringID>(UIWidgetCommonStrings::BehaviorGraphFilter),
					a_data.hkxFilter))
			{
				OnBaseConfigChange(
					a_handle,
					a_params,
					PostChangeAction::Reset);
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawFiltersTreeContextMenu(
			T                   a_handle,
			Data::configBase_t& a_data,
			const void*         a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("context");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (a_data.filters)
				{
					if (UIL::LCG_MI(UIWidgetCommonStrings::ClearAll, "1"))
					{
						a_data.filters.reset();

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result = BaseConfigEditorAction::Delete;
					}
				}
				else
				{
					if (UIL::LCG_MI(CommonStrings::Create, "2"))
					{
						a_data.filters = std::make_unique<Data::configBaseFilters_t>();

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result = BaseConfigEditorAction::Insert;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawFiltersTree(
			T                   a_handle,
			Data::configBase_t& a_data,
			const void*         a_params)
		{
			ImGui::PushID("cnf_filters");

			const auto result = DrawFiltersTreeContextMenu(a_handle, a_data, a_params);

			if (result == BaseConfigEditorAction::Insert)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			}
			else if (result == BaseConfigEditorAction::Delete)
			{
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);
			}

			bool disabled = !a_data.filters;

			UICommon::PushDisabled(disabled);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Filters)))
			{
				if (a_data.filters)
				{
					bcFormFilterParams_t<T> ffparams{
						a_handle,
						a_params
					};

					ImGui::Spacing();

					ImGui::PushID("1");

					m_formFilter.DrawFormFiltersTree(
						UIL::LS(UIWidgetCommonStrings::ActorFilters),
						ffparams,
						a_data.filters->actorFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
							m_ffFormSelector.SetFormBrowserEnabled(false);
						});

					ImGui::PopID();

					ImGui::PushID("2");

					m_formFilter.DrawFormFiltersTree(
						UIL::LS(UIWidgetCommonStrings::NPCFilters),
						ffparams,
						a_data.filters->npcFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
							m_ffFormSelector.SetFormBrowserEnabled(true);
						});

					ImGui::PopID();

					ImGui::PushID("3");

					m_formFilter.DrawFormFiltersTree(
						UIL::LS(UIWidgetCommonStrings::RaceFilters),
						ffparams,
						a_data.filters->raceFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
							m_ffFormSelector.SetFormBrowserEnabled(true);
						});

					ImGui::PopID();

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrideValues(
			const baseConfigEquipmentOverrideParams_t<T>& a_params,
			Data::configBaseValues_t&                     a_data)
		{
			DrawBaseConfigValues(
				a_params.handle,
				a_data,
				a_params.params,
				a_params.name,
				false,
				nullptr);
		}

		template <class T>
		constexpr bool UIBaseConfigWidget<T>::EOStoreCC() const
		{
			return BaseConfigStoreCC();
		}

		template <class T>
		void UIBaseConfigWidget<T>::OnEquipmentOverrideChange(
			const baseConfigEquipmentOverrideParams_t<T>& a_params)
		{
			OnBaseConfigChange(a_params.handle, a_params.params, PostChangeAction::Evaluate);
		}

		template <class T>
		template <class Tpv, class Tf>
		void UIBaseConfigWidget<T>::PropagateMemberToEquipmentOverrides(
			Data::configBase_t* a_data,
			std::ptrdiff_t      a_offset,
			const Tpv&          a_value,
			Tf                  a_postWrite)
		{
			if (!a_data)
			{
				return;
			}

			if (!GetEnableEquipmentOverridePropagation())
			{
				return;
			}

			auto src = static_cast<Data::configBaseValues_t*>(a_data);

			auto srcVal = reinterpret_cast<Tpv*>(
				reinterpret_cast<std::uintptr_t>(src) + a_offset);

			RecursivePropagateMemberToEquipmentOverrides(
				a_data->equipmentOverrides,
				a_offset,
				*srcVal,
				a_postWrite);
		}

		template <class T>
		template <class Tpv, class Tf>
		void UIBaseConfigWidget<T>::RecursivePropagateMemberToEquipmentOverrides(
			Data::equipmentOverrideList_t& a_list,
			std::ptrdiff_t                 a_offset,
			const Tpv&                     a_srcVal,
			Tf                             a_postWrite)
		{
			for (auto& e : a_list)
			{
				if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
				{
					RecursivePropagateMemberToEquipmentOverrides(e.group, a_offset, a_srcVal, a_postWrite);
				}
				else
				{
					auto& v = static_cast<Data::configBaseValues_t&>(e);

					auto dstVal = reinterpret_cast<Tpv*>(
						reinterpret_cast<std::uintptr_t>(
							std::addressof(v)) +
						a_offset);

					*dstVal = a_srcVal;

					a_postWrite(v);
				}
			}
		}

		template <class T>
		template <class Tpv>
		void UIBaseConfigWidget<T>::PropagateToEquipmentOverrides(
			Data::configBase_t* a_data)
		{
			if (!a_data)
			{
				return;
			}

			if (!GetEnableEquipmentOverridePropagation())
			{
				return;
			}

			auto src = static_cast<const Tpv*>(a_data);

			RecursivePropagateToEquipmentOverrides(a_data->equipmentOverrides, *src);
		}

		template <class T>
		template <class Tpv>
		void UIBaseConfigWidget<T>::RecursivePropagateToEquipmentOverrides(
			Data::equipmentOverrideList_t& a_list,
			const Tpv&                     a_src)
		{
			for (auto& e : a_list)
			{
				if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
				{
					RecursivePropagateToEquipmentOverrides(e.group, a_src);
				}
				else
				{
					auto& dst = static_cast<Tpv&>(e);

					dst = a_src;
				}
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::PropagateFlagToEquipmentOverrides(
			Data::configBase_t* a_data,
			Data::BaseFlags     a_mask)
		{
			if (!a_data)
			{
				return;
			}

			if (!GetEnableEquipmentOverridePropagation())
			{
				return;
			}

			RecursivePropagateFlagToEquipmentOverrides(
				a_data->equipmentOverrides,
				a_mask,
				a_data->flags.value & a_mask);
		}

		template <class T>
		void UIBaseConfigWidget<T>::RecursivePropagateFlagToEquipmentOverrides(
			Data::equipmentOverrideList_t& a_list,
			Data::BaseFlags                a_mask,
			Data::BaseFlags                a_flags)
		{
			for (auto& e : a_list)
			{
				if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
				{
					RecursivePropagateFlagToEquipmentOverrides(e.group, a_mask, a_flags);
				}
				else
				{
					e.flags.value = (e.flags.value & ~a_mask) | a_flags;
				}
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigValues(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			Data::configBase_t*       a_baseConfig)
		{
			const bool disabled = a_data.flags.test(Data::BaseFlags::kDisabled);

			DrawBaseConfigValuesFlags(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				a_storecc,
				a_baseConfig,
				disabled);

			DrawBaseConfigValuesNode(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				a_storecc,
				a_baseConfig,
				disabled);

			DrawBaseConfigValuesOther(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				a_storecc,
				a_baseConfig,
				disabled);

			DrawBaseConfigValuesPhysics(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				a_storecc,
				a_baseConfig,
				disabled);
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigValuesFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			Data::configBase_t*       a_baseConfig,
			const bool                a_disabled)
		{
			auto storecc = BaseConfigStoreCC() && a_storecc;

			ImGui::PushID("bc_flags");

			DrawBaseConfigGeneralFlags(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				storecc,
				a_disabled,
				a_baseConfig);

			DrawBaseConfig3DFlags(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				storecc,
				a_disabled,
				a_baseConfig);

			DrawBaseConfigAnimationFlags(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				storecc,
				a_disabled,
				a_baseConfig);

			DrawBaseConfigLightFlags(
				a_handle,
				a_data,
				a_params,
				a_slotName,
				storecc,
				a_disabled,
				a_baseConfig);

			if (a_baseConfig)
			{
				bool result;

				if (storecc)
				{
					result = this->TreeEx(
						"f_ex",
						true,
						"%s",
						UIL::LS(CommonStrings::Extra));
				}
				else
				{
					result = ImGui::TreeNodeEx(
						"f_ex",
						ImGuiTreeNodeFlags_DefaultOpen |
							ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						UIL::LS(CommonStrings::Extra));
				}

				if (result)
				{
					ImGui::Spacing();

					const bool extraDisabled = a_disabled &&
					                           a_baseConfig->equipmentOverrides.empty();

					UICommon::PushDisabled(extraDisabled);

					DrawExtraFlags(a_handle, a_data, a_baseConfig, a_params);

					UICommon::PopDisabled(extraDisabled);

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}

			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigValuesNode(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			Data::configBase_t*       a_baseConfig,
			const bool                a_disabled)
		{
			auto storecc = BaseConfigStoreCC() && a_storecc;

			ImGui::PushID("bc_node");

			bool r;

			if (storecc)
			{
				r = this->TreeEx(
					"tree",
					true,
					"%s",
					UIL::LS(CommonStrings::Node));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Node));
			}

			if (r)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(UIL::LS(UIBaseConfigString::AttachmentModeColon));
				ImGui::SameLine();

				bool tmpb;

				if (ImGui::RadioButton(
						UIL::LS(CommonStrings::Reference, "1"),
						a_data.flags.test(Data::BaseFlags::kReferenceMode)))
				{
					a_data.flags.set(Data::BaseFlags::kReferenceMode);

					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kReferenceMode);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

				tmpb = a_data.targetNode.managed();

				UICommon::PushDisabled(tmpb);

				if (ImGui::RadioButton(
						UIL::LS(CommonStrings::Parent, "2"),
						!a_data.flags.test(Data::BaseFlags::kReferenceMode)))
				{
					a_data.flags.clear(Data::BaseFlags::kReferenceMode);

					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kReferenceMode);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(tmpb);

				UITipsInterface::DrawTip(UITip::AttachmentMode);

				ImGui::Spacing();

				if (DrawNodeSelector(
						UIL::LS(UIWidgetCommonStrings::TargetNode, "ns"),
						!a_data.flags.test(Data::BaseFlags::kReferenceMode),
						a_data.targetNode))
				{
					PropagateMemberToEquipmentOverrides(
						a_baseConfig,
						offsetof(Data::configBaseValues_t, targetNode),
						a_data.targetNode,
						[](auto&) {});

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::TargetNode);

				if (!a_data.targetNode)
				{
					ImGui::Spacing();

					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						UIL::LS(UIBaseConfigString::EmptyNodeWarning));
				}

				ImGui::Spacing();

				DrawTransformTree(
					static_cast<Data::configTransform_t&>(a_data),
					false,
					[&](TransformUpdateValue a_v) {
						switch (a_v)
						{
						case TransformUpdateValue::Position:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, position),
								a_data.position,
								[](auto& a_v) {
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::Rotation:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, rotation),
								a_data.rotation,
								[](auto& a_v) {
									a_v.update_rotation_matrix();
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::Scale:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, scale),
								a_data.scale,
								[](auto& a_v) {
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::All:
							PropagateToEquipmentOverrides<
								Data::configTransform_t>(
								a_baseConfig);
							break;
						}

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::UpdateTransform);
					},
					[] {});

				UICommon::PopDisabled(a_disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigValuesOther(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			Data::configBase_t*       a_baseConfig,
			const bool                a_disabled)
		{
			auto storecc = BaseConfigStoreCC() && a_storecc;

			ImGui::PushID("bc_other");

			bool r;

			if (storecc)
			{
				r = this->TreeEx(
					"tree",
					true,
					"%s",
					UIL::LS(CommonStrings::Other));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Other));
			}

			if (r)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				if (m_formPicker.DrawFormPicker(
						"fp_m",
						static_cast<Localization::StringID>(UIWidgetCommonStrings::OverrideModel),
						a_data.forceModel,
						UITipsInterface::GetTipText(UITip::OverrideModel)))
				{
					PropagateMemberToEquipmentOverrides(
						a_baseConfig,
						offsetof(Data::configBaseValues_t, forceModel),
						a_data.forceModel,
						[](auto&) {});

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(a_disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawPhysicsValuesContextMenu(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				bool hasValue = static_cast<bool>(a_data.physicsValues);

				if (hasValue)
				{
					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Clear, "0")))
					{
						a_data.physicsValues.data.reset();

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result = BaseConfigEditorAction::Reset;
					}
				}
				else
				{
					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Create, "1")))
					{
						a_data.physicsValues.data =
							std::make_unique_for_overwrite<
								Data::configNodePhysicsValues_t>();

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result = BaseConfigEditorAction::Create;
					}
				}

				hasValue = static_cast<bool>(a_data.physicsValues);

				ImGui::Separator();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Copy, "2"),
						nullptr,
						false,
						hasValue))
				{
					if (hasValue)
					{
						UIClipboard::Set<Data::configNodePhysicsValues_t>(
							*a_data.physicsValues.data);
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodePhysicsValues_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data.physicsValues.data =
							std::make_unique<Data::configNodePhysicsValues_t>(*clipData);

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result = BaseConfigEditorAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigValuesPhysics(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			Data::configBase_t*       a_baseConfig,
			const bool                a_disabled)
		{
			const auto storecc = BaseConfigStoreCC() && a_storecc;

			ImGui::PushID("bc_phys");

			UICommon::PushDisabled(a_disabled);

			const auto cmresult = DrawPhysicsValuesContextMenu(
				a_handle,
				a_data,
				a_params);

			UICommon::PopDisabled(a_disabled);

			switch (cmresult)
			{
			case BaseConfigEditorAction::Create:
			case BaseConfigEditorAction::Paste:
				ImGui::SetNextItemOpen(true);
				break;
			}

			bool r;

			if (storecc)
			{
				r = this->TreeEx(
					"tree",
					false,
					"%s",
					UIL::LS(CommonStrings::Physics));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Physics));
			}

			if (r)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				if (a_data.physicsValues)
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * -14.5f);

					if (DrawPhysicsValues(
							*a_data.physicsValues.data))
					{
						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					}

					ImGui::PopItemWidth();
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
					ImGui::TextWrapped(
						"%s",
						UIL::LS(UIBaseConfigString::CreatePhysicsDataInfo));
					ImGui::PopStyleColor();
				}

				UICommon::PopDisabled(a_disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigGeneralFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			bool                      a_disabled,
			Data::configBase_t*       a_baseConfig)
		{
			bool tresult;

			if (a_storecc)
			{
				tresult = this->TreeEx(
					"f_gn",
					true,
					"%s",
					UIL::LS(CommonStrings::General));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_gn",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::General));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Disabled, "0"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDisabled)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisabled);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				UICommon::PushDisabled(a_disabled);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Invisible, "1"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kInvisible)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kInvisible);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::Invisible);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::HideInFurniture, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kHideIfUsingFurniture)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kHideIfUsingFurniture);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::HideInFurniture);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::HideLayingDown, "3"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kHideLayingDown)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kHideLayingDown);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::HideLayingDown);

				ImGui::NextColumn();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::PlayEquipSound, "4"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kPlayEquipSound)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kPlayEquipSound);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::PlayLoopSound, "4a"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kPlayLoopSound)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kPlayLoopSound);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::IgnoreRaceEquipTypes, "5"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kIgnoreRaceEquipTypes)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kIgnoreRaceEquipTypes);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::IgnoreRaceEquipTypesSlot);

				const bool atmReference = a_data.flags.test(Data::BaseFlags::kReferenceMode);

				UICommon::PushDisabled(!atmReference);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::SyncReference, "6"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kSyncReferenceTransform)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kSyncReferenceTransform);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(!atmReference);

				UITipsInterface::DrawTip(UITip::SyncReferenceNode);

				UICommon::PopDisabled(a_disabled);

				ImGui::Columns();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfig3DFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			bool                      a_disabled,
			Data::configBase_t*       a_baseConfig)
		{
			bool tresult;

			if (a_storecc)
			{
				tresult = this->TreeEx(
					"f_3d",
					true,
					"%s",
					UIL::LS(CommonStrings::Model));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_3d",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Model));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				UICommon::PushDisabled(a_disabled);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::RemoveScabbard, "1"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kRemoveScabbard)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kRemoveScabbard);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::RemoveScabbard);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::RemoveProjectileTracers, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kRemoveProjectileTracers)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kRemoveProjectileTracers);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::RemoveProjectileTracers);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::RemoveEditorMarker, "_2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kRemoveEditorMarker)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kRemoveEditorMarker);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::DynamicArrows, "3"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDynamicArrows)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDynamicArrows);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::DynamicArrows);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::Use1pWeaponModels, "4"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kLoad1pWeaponModel)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kLoad1pWeaponModel);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::Load1pWeaponModel);

				ImGui::NextColumn();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::UseWorldModel, "5"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kUseWorldModel)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kUseWorldModel);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::UseWorldModel);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::KeepTorchFlame, "6"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kKeepTorchFlame)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kKeepTorchFlame);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::KeepTorchFlame);

				const bool disable = !GetLightsEnabled() && !IsProfileEditor();

				UICommon::PushDisabled(disable);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::AttachLight, "_1"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kAttachLight)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kAttachLight);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::AttachLight);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::HideLight, "_2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kHideLight)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kHideLight);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::HideLight);

				UICommon::PopDisabled(disable);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::DisableHavok, "7"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDisableHavok)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisableHavok);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::DisableHavok);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIBaseConfigString::DropOnDeath, "8"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDropOnDeath)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDropOnDeath);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::DropOnDeath);

				UICommon::PopDisabled(a_disabled);

				ImGui::Columns();

				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				DrawTransformTree(
					a_data.geometryTransform,
					false,
					[&](TransformUpdateValue a_v) {
						switch (a_v)
						{
						case TransformUpdateValue::Position:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, geometryTransform.position),
								a_data.geometryTransform.position,
								[](auto& a_v) {
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::Rotation:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, geometryTransform.rotation),
								a_data.geometryTransform.rotation,
								[](auto& a_v) {
									a_v.update_rotation_matrix();
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::Scale:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, geometryTransform.scale),
								a_data.geometryTransform.scale,
								[](auto& a_v) {
									a_v.update_tag();
								});
							break;
						case TransformUpdateValue::All:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, geometryTransform),
								a_data.geometryTransform,
								[](auto& a_v) {});
							break;
						}

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					},
					[&] {
						ImGui::Spacing();

						if (ImGui::CheckboxFlagsT(
								UIL::LS(CommonStrings::Invisible, "1"),
								stl::underlying(std::addressof(a_data.flags.value)),
								stl::underlying(Data::BaseFlags::kHideGeometry)))
						{
							PropagateFlagToEquipmentOverrides(
								a_baseConfig,
								Data::BaseFlags::kHideGeometry);

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}
						UITipsInterface::DrawTip(UITip::HideGeometry);

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UIBaseConfigString::ScaleCollider, "2"),
								stl::underlying(std::addressof(a_data.flags.value)),
								stl::underlying(Data::BaseFlags::kGeometryScaleCollider)))
						{
							PropagateFlagToEquipmentOverrides(
								a_baseConfig,
								Data::BaseFlags::kGeometryScaleCollider);

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
						}
					},
					static_cast<Localization::StringID>(UIBaseConfigString::Geometry),
					ImGuiTreeNodeFlags_None);

				UICommon::PopDisabled(a_disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigAnimationFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			bool                      a_disabled,
			Data::configBase_t*       a_baseConfig)
		{
			bool tresult;

			if (a_storecc)
			{
				tresult = this->TreeEx(
					"f_an",
					true,
					"%s",
					UIL::LS(CommonStrings::Animation));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_an",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Animation));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				UICommon::PushDisabled(a_disabled);

				bool paChanged = ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::PlayAnimation, "1"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::BaseFlags::kPlaySequence));

				if (paChanged)
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kPlaySequence);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::PlayAnimation);

				if (a_data.flags.test(Data::BaseFlags::kPlaySequence))
				{
					if (paChanged &&
					    a_data.niControllerSequence.empty())
					{
						ImGui::OpenPopup("7ctx");
						ClearDescriptionPopupBuffer();
					}

					ImGui::SameLine();
					if (UIPopupToggleButtonWidget::DrawPopupToggleButton("7b", "7ctx"))
					{
						SetDescriptionPopupBuffer(a_data.niControllerSequence);
					}

					if (ImGui::BeginPopup("7ctx"))
					{
						if (DrawDescriptionPopup(UIL::LS(CommonStrings::Sequence, "1")))
						{
							a_data.niControllerSequence = GetDescriptionPopupBuffer();

							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, niControllerSequence),
								a_data.niControllerSequence,
								[](auto&) {});

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}

						ImGui::EndPopup();
					}
				}

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::DisableWeaponAnims, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDisableBehaviorGraphAnims)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisableBehaviorGraphAnims);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				UITipsInterface::DrawTip(UITip::DisableWeaponAnims);

				ImGui::NextColumn();

				paChanged = ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::AnimationEvent, "4"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::BaseFlags::kAnimationEvent));

				if (paChanged)
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kAnimationEvent);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				UITipsInterface::DrawTip(UITip::AnimationEvent);

				if (a_data.flags.test(Data::BaseFlags::kAnimationEvent))
				{
					if (paChanged &&
					    a_data.animationEvent.empty())
					{
						ImGui::OpenPopup("Hctx");
						ClearDescriptionPopupBuffer();
					}

					ImGui::SameLine();
					if (UIPopupToggleButtonWidget::DrawPopupToggleButton("Hb", "Hctx"))
					{
						SetDescriptionPopupBuffer(a_data.animationEvent);
					}

					if (ImGui::BeginPopup("Hctx"))
					{
						if (DrawDescriptionPopup(UIL::LS(CommonStrings::Event, "1")))
						{
							a_data.animationEvent = GetDescriptionPopupBuffer();

							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, animationEvent),
								a_data.animationEvent,
								[](auto&) {});

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}

						ImGui::EndPopup();
					}
				}

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::ReceiveActorEvents, "5"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kAttachSubGraphs)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kAttachSubGraphs);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				UITipsInterface::DrawTip(UITip::AnimEventForwarding);

				UICommon::PopDisabled(a_disabled);

				ImGui::Columns();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawBaseConfigLightFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			const void*               a_params,
			const stl::fixed_string&  a_slotName,
			bool                      a_storecc,
			bool                      a_disabled,
			Data::configBase_t*       a_baseConfig)
		{
			bool tresult;

			if (a_storecc)
			{
				tresult = this->TreeEx(
					"f_li",
					false,
					"%s",
					UIL::LS(CommonStrings::Light));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_li",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Light));
			}

			if (tresult)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				if (UIExtraLightEditorWidget::DrawExtraLightEditor(a_data.extraLightConfig))
				{
					PropagateMemberToEquipmentOverrides(
						a_baseConfig,
						offsetof(Data::configBaseValues_t, extraLightConfig),
						a_data.extraLightConfig,
						[](auto&) {});

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(a_disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawExtraFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			Data::configBase_t*       a_baseConfig,
			const void*               a_params)
		{
		}

		template <class T>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawEffectShaderTreeContextMenu(
			T                   a_handle,
			Data::configBase_t& a_data,
			const void*         a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("es_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::New, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_data.effectShaders.data.emplace_back(
							GetDescriptionPopupBuffer());

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						ClearDescriptionPopupBuffer();

						result = BaseConfigEditorAction::Insert;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set<Data::effectShaderList_t>(a_data.effectShaders);
				}

				{
					auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_data.effectShaders.data.emplace_back(*clipData);

							OnBaseConfigChange(
								a_handle,
								a_params,
								PostChangeAction::Evaluate);

							result = BaseConfigEditorAction::Paste;
						}
					}
				}

				{
					auto clipData = UIClipboard::Get<Data::effectShaderList_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_data.effectShaders = *clipData;

							OnBaseConfigChange(
								a_handle,
								a_params,
								PostChangeAction::Evaluate);

							result = BaseConfigEditorAction::Paste;
						}
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEffectShaders(
			T                        a_handle,
			Data::configBase_t&      a_data,
			const void*              a_params,
			const stl::fixed_string& a_slotName)
		{
			const bool disabled = !GetShaderProcessingEnabled();

			UICommon::PushDisabled(disabled);

			auto storecc = BaseConfigStoreCC();

			ImGui::PushID("config_effect_shaders");

			const auto result = DrawEffectShaderTreeContextMenu(a_handle, a_data, a_params);

			const bool empty = a_data.effectShaders.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result == BaseConfigEditorAction::Insert)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			if (disabled)
			{
				ImGui::SetNextItemOpen(false);
			}

			bool r;

			if (storecc)
			{
				r = this->TreeEx(
					"tree",
					true,
					"%s",
					UIL::LS(UIBaseConfigString::EffectShaders));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UIBaseConfigString::EffectShaders));
			}

			if (r)
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawEffectShaderHolderList(
						a_handle,
						a_data,
						a_params,
						a_slotName);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();

			UICommon::PopDisabled(disabled);
		}

		template <class T>
		UIEquipmentOverrideResult UIBaseConfigWidget<T>::DrawEffectShaderHolderContextMenu(
			T                                 a_handle,
			const void*                       a_params,
			Data::effectShaderList_t&         a_list,
			Data::configEffectShaderHolder_t& a_data)
		{
			UIEquipmentOverrideResult result;

			ImGui::PushID("es_holder_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = UIEquipmentOverrideAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = UIEquipmentOverrideAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::New, "2"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = UIEquipmentOverrideAction::Insert;
							result.desc   = GetDescriptionPopupBuffer();

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = UIEquipmentOverrideAction::Paste;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(UIBaseConfigString::AddShaderEntry, "2"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = UIEquipmentOverrideAction::Create;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "4")))
				{
					result.action = UIEquipmentOverrideAction::Delete;
				}

				if (UIL::LCG_BM(CommonStrings::Rename, "5"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = UIEquipmentOverrideAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "8"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						TriggerEffectShaderUpdate(
							a_handle,
							a_data,
							a_params);

						result.action = UIEquipmentOverrideAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEffectShaderHolderList(
			T                        a_handle,
			Data::configBase_t&      a_data,
			const void*              a_params,
			const stl::fixed_string& a_slotName)
		{
			auto& list = a_data.effectShaders.data;

			if (list.empty())
			{
				return;
			}

			ImGui::PushID("es_list");

			int i = 0;

			auto it = list.begin();

			while (it != list.end())
			{
				ImGui::PushID(i);

				const auto result = DrawEffectShaderHolderContextMenu(
					a_handle,
					a_params,
					a_data.effectShaders,
					*it);

				switch (result.action)
				{
				case UIEquipmentOverrideAction::Delete:
					it = list.erase(it);

					OnBaseConfigChange(
						a_handle,
						a_params,
						PostChangeAction::Evaluate);

					break;
				case UIEquipmentOverrideAction::Insert:
					it = list.emplace(it, result.desc);

					OnBaseConfigChange(
						a_handle,
						a_params,
						PostChangeAction::Evaluate);

					ImGui::SetNextItemOpen(true);
					break;
				case UIEquipmentOverrideAction::Create:

					if (it->data.try_emplace(result.desc).second)
					{
						TriggerEffectShaderUpdate(a_handle, *it, a_params);
						ImGui::SetNextItemOpen(true);
					}
					else
					{
						QueueNotification(
							UIL::LS(CommonStrings::Error),
							"%s",
							UIL::LS(UIBaseConfigString::EffectShaderEntryExists));
					}

					break;
				case UIEquipmentOverrideAction::Swap:

					if (IterSwap(list, it, result.dir))
					{
						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					}

					break;
				case UIEquipmentOverrideAction::Rename:
					it->description = result.desc;
					//TriggerEffectShaderUpdate(a_handle, *it, a_params);
					break;
				case UIEquipmentOverrideAction::Paste:
					if (auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>())
					{
						it = list.emplace(it, *clipData);

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					}
					[[fallthrough]];
				case UIEquipmentOverrideAction::PasteOver:
					ImGui::SetNextItemOpen(true);
					break;
				}

				if (it != list.end())
				{
					auto& e = *it;

					if (ImGui::TreeNodeEx(
							"es_item",
							ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							e.description.c_str()))
					{
						ImGui::Spacing();

						auto& luid = e.get_tag_data();

						ImGui::Text("LUID: %llx", luid.p1);

						ImGui::Spacing();

						const auto r = DrawEquipmentOverrideConditionHeaderContextMenu(
							e.conditions,
							[this, a_handle, a_params, &a_data, &e] {
								TriggerEffectShaderUpdate(
									a_handle,
									e,
									a_params);
							});

						const bool empty = e.conditions.empty();

						if (!empty)
						{
							if (r == UIEquipmentOverrideAction::PasteOver ||
							    r == UIEquipmentOverrideAction::Insert)
							{
								ImGui::SetNextItemOpen(true);
							}
						}

						UICommon::PushDisabled(empty);

						if (ImGui::TreeNodeEx(
								"es_item_match",
								ImGuiTreeNodeFlags_SpanAvailWidth |
									ImGuiTreeNodeFlags_DefaultOpen,
								"%s",
								UIL::LS(CommonStrings::Conditions)))
						{
							if (!empty)
							{
								ImGui::Spacing();

								DrawEquipmentOverrideEntryConditionTable(
									e.conditions,
									false,
									[this, a_handle, a_params, &a_data, &e] {
										TriggerEffectShaderUpdate(
											a_handle,
											e,
											a_params);
									});
							}

							ImGui::TreePop();
						}

						UICommon::PopDisabled(empty);

						ImGui::Spacing();

						ImGui::PushID("es_conf_entry");

						baseEffectShaderEditorParams_t<T> params{
							a_handle,
							a_params,
							e
						};

						this->DrawEffectShaderEditor(params, e);

						ImGui::PopID();

						ImGui::Spacing();

						ImGui::TreePop();
					}

					i++;
					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::TriggerEffectShaderUpdate(
			T                                 a_handle,
			Data::configEffectShaderHolder_t& a_data,
			const void*                       a_params)
		{
			a_data.update_tag();

			OnBaseConfigChange(
				a_handle,
				a_params,
				PostChangeAction::Evaluate);
		}

		template <class T>
		void UIBaseConfigWidget<T>::OnEffectShaderUpdate(
			const baseEffectShaderEditorParams_t<T>& a_params,
			bool                                     a_updateTag)
		{
			if (a_updateTag)
			{
				TriggerEffectShaderUpdate(
					a_params.handle,
					a_params.data,
					a_params.params);
			}
			else
			{
				OnBaseConfigChange(
					a_params.handle,
					a_params.params,
					PostChangeAction::Evaluate);
			}
		}

		template <class T>
		constexpr bool UIBaseConfigWidget<T>::IsProfileEditor() const
		{
			return false;
		}

	}
}