#pragma once

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/UIAllowedModelTypes.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UINotificationInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/UITips.h"

#include "EquipmentOverrideResult.h"
#include "Form/UIFormFilterWidget.h"
#include "Form/UIFormSelectorWidget.h"
#include "UIBaseConfigWidgetStrings.h"
#include "UIConditionParamEditorWidget.h"
#include "UIDescriptionPopup.h"
#include "UIEffectShaderEditorWidget.h"
#include "UIEquipmentOverrideConditionsWidget.h"
#include "UINodeSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPopupToggleButtonWidget.h"
#include "UISimpleStringSet.h"
#include "UITransformSliderWidget.h"
#include "UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/StringHolder.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"

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
		class UIBaseConfigWidget :
			public UINodeSelectorWidget,
			public UIFormLookupInterface,
			public UIEquipmentOverrideConditionsWidget,
			UIEffectShaderEditorWidget<baseEffectShaderEditorParams_t<T>>,
			public virtual UICollapsibles,
			public virtual UIDescriptionPopupWidget,
			public virtual UITransformSliderWidget,
			public virtual UITipsInterface,
			public virtual UINotificationInterface,
			public virtual UILocalizationInterface,
			public virtual UISettingsInterface,
			public virtual UISimpleStringSetWidget
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

			virtual void OnBaseConfigChange(
				T                a_handle,
				const void*      a_params,
				PostChangeAction a_action) = 0;

			template <class Tpv>
			void PropagateMemberToEquipmentOverrides(
				Data::configBase_t* a_data,
				std::ptrdiff_t      a_offset,
				const Tpv&          a_value);

			template <class Tpv>
			static void RecursivePropagateMemberToEquipmentOverrides(
				Data::equipmentOverrideList_t& a_list,
				std::ptrdiff_t                 a_offset,
				const Tpv&                     a_srcVal);

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

			void DrawEquipmentOverrides(
				T                              a_handle,
				Data::configBase_t&            a_baseConfig,
				Data::equipmentOverrideList_t& a_list,
				const void*                    a_params,
				const stl::fixed_string&       a_slotName);

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

			BaseConfigEditorAction DrawEquipmentOverrideTreeContextMenu(
				T                              a_handle,
				Data::configBase_t&            a_baseConfig,
				Data::equipmentOverrideList_t& a_list,
				const void*                    a_params);

			void DrawEquipmentOverrideList(
				T                              a_handle,
				Data::configBase_t&            a_baseConfig,
				Data::equipmentOverrideList_t& a_list,
				const void*                    a_params,
				const stl::fixed_string&       a_slotName);

		protected:
		private:
			virtual bool GetEnableEquipmentOverridePropagation() = 0;

			virtual bool SkipEquipmentOverride(
				const Data::equipmentOverride_t& a_override) const;

			virtual void DrawExtraEquipmentOverrideOptions(
				T                          a_handle,
				Data::configBase_t&        a_data,
				const void*                a_params,
				Data::equipmentOverride_t& a_override);

			EquipmentOverrideResult DrawEquipmentOverrideContextMenu(
				T                          a_handle,
				const void*                a_params,
				Data::equipmentOverride_t& a_data);

			BaseConfigEditorAction DrawEffectShaderTreeContextMenu(
				T                   a_handle,
				Data::configBase_t& a_data,
				const void*         a_params);

			void DrawEffectShaders(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

			EquipmentOverrideResult DrawEffectShaderHolderContextMenu(
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
			UIEquipmentOverrideConditionsWidget(a_controller),
			UIEffectShaderEditorWidget<baseEffectShaderEditorParams_t<T>>(a_controller),
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

			DrawEquipmentOverrides(
				a_handle,
				a_data,
				a_data.equipmentOverrides,
				a_params,
				a_slotName);

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
					if (LCG_MI(UIWidgetCommonStrings::ClearAll, "1"))
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
					if (LCG_MI(CommonStrings::Create, "2"))
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
					LS(CommonStrings::Filters)))
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
						LS(UIWidgetCommonStrings::ActorFilters),
						ffparams,
						a_data.filters->actorFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
							m_ffFormSelector.SetFormBrowserEnabled(false);
						});

					ImGui::PopID();

					ImGui::PushID("2");

					m_formFilter.DrawFormFiltersTree(
						LS(UIWidgetCommonStrings::NPCFilters),
						ffparams,
						a_data.filters->npcFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
							m_ffFormSelector.SetFormBrowserEnabled(true);
						});

					ImGui::PopID();

					ImGui::PushID("3");

					m_formFilter.DrawFormFiltersTree(
						LS(UIWidgetCommonStrings::RaceFilters),
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
		template <class Tpv>
		void UIBaseConfigWidget<T>::PropagateMemberToEquipmentOverrides(
			Data::configBase_t* a_data,
			std::ptrdiff_t      a_offset,
			const Tpv&          a_value)
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
				*srcVal);
		}

		template <class T>
		template <class Tpv>
		void UIBaseConfigWidget<T>::RecursivePropagateMemberToEquipmentOverrides(
			Data::equipmentOverrideList_t& a_list,
			std::ptrdiff_t                 a_offset,
			const Tpv&                     a_srcVal)
		{
			for (auto& e : a_list)
			{
				if (e.eoFlags.test(Data::EquipmentOverrideFlags::kIsGroup))
				{
					RecursivePropagateMemberToEquipmentOverrides(e.group, a_offset, a_srcVal);
				}
				else
				{
					auto dstVal = reinterpret_cast<Tpv*>(
						reinterpret_cast<std::uintptr_t>(
							std::addressof(static_cast<Data::configBaseValues_t&>(e))) +
						a_offset);

					*dstVal = a_srcVal;
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
				if (e.eoFlags.test(Data::EquipmentOverrideFlags::kIsGroup))
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
				if (e.eoFlags.test(Data::EquipmentOverrideFlags::kIsGroup))
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

			if (a_baseConfig)
			{
				bool result;

				if (storecc)
				{
					result = TreeEx(
						"f_ex",
						true,
						"%s",
						LS(CommonStrings::Extra));
				}
				else
				{
					result = ImGui::TreeNodeEx(
						"f_ex",
						ImGuiTreeNodeFlags_DefaultOpen |
							ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						LS(CommonStrings::Extra));
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
				r = TreeEx(
					"tree",
					true,
					"%s",
					LS(CommonStrings::Node));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Node));
			}

			if (r)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(LS(UIBaseConfigString::AttachmentModeColon));
				ImGui::SameLine();

				bool tmpb;

				if (ImGui::RadioButton(
						LS(CommonStrings::Reference, "1"),
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
						LS(CommonStrings::Parent, "2"),
						!a_data.flags.test(Data::BaseFlags::kReferenceMode)))
				{
					a_data.flags.clear(Data::BaseFlags::kReferenceMode);

					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kReferenceMode);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(tmpb);

				DrawTip(UITip::AttachmentMode);

				ImGui::Spacing();

				if (DrawNodeSelector(
						LS(UIWidgetCommonStrings::TargetNode, "ns"),
						!a_data.flags.test(Data::BaseFlags::kReferenceMode),
						a_data.targetNode))
				{
					PropagateMemberToEquipmentOverrides(
						a_baseConfig,
						offsetof(Data::configBaseValues_t, targetNode),
						a_data.targetNode);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				DrawTip(UITip::TargetNode);

				if (!a_data.targetNode)
				{
					ImGui::Spacing();

					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						LS(UIBaseConfigString::EmptyNodeWarning));
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
								a_data.position);
							break;
						case TransformUpdateValue::Rotation:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, rotation),
								a_data.rotation);
							break;
						case TransformUpdateValue::Scale:
							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, scale),
								a_data.scale);
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
				r = TreeEx(
					"tree",
					true,
					"%s",
					LS(CommonStrings::Other));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Other));
			}

			if (r)
			{
				ImGui::Spacing();

				UICommon::PushDisabled(a_disabled);

				if (m_formPicker.DrawFormPicker(
						"fp_m",
						static_cast<Localization::StringID>(UIWidgetCommonStrings::OverrideModel),
						a_data.forceModel,
						GetTipText(UITip::OverrideModel)))
				{
					PropagateMemberToEquipmentOverrides(
						a_baseConfig,
						offsetof(Data::configBaseValues_t, forceModel),
						a_data.forceModel);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
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
				tresult = TreeEx(
					"f_gn",
					true,
					"%s",
					LS(CommonStrings::General));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_gn",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::General));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				if (ImGui::CheckboxFlagsT(
						LS(CommonStrings::Disabled, "0"),
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
						LS(CommonStrings::Invisible, "1"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kInvisible)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kInvisible);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				DrawTip(UITip::Invisible);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::HideInFurniture, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kHideIfUsingFurniture)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kHideIfUsingFurniture);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				DrawTip(UITip::HideInFurniture);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::HideLayingDown, "3"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kHideLayingDown)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kHideLayingDown);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				DrawTip(UITip::HideLayingDown);

				ImGui::NextColumn();

				if (ImGui::CheckboxFlagsT(
						LS(CommonStrings::kPlaySound, "4"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kPlaySound)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kPlaySound);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::IgnoreRaceEquipTypes, "5"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kIgnoreRaceEquipTypes)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kIgnoreRaceEquipTypes);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}
				DrawTip(UITip::IgnoreRaceEquipTypesSlot);

				const bool atmReference = a_data.flags.test(Data::BaseFlags::kReferenceMode);

				UICommon::PushDisabled(!atmReference);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::SyncReference, "6"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kSyncReferenceTransform)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kSyncReferenceTransform);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}

				UICommon::PopDisabled(!atmReference);

				DrawTip(UITip::SyncReferenceNode);

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
				tresult = TreeEx(
					"f_3d",
					true,
					"%s",
					LS(CommonStrings::Model));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_3d",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Model));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				UICommon::PushDisabled(a_disabled);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::RemoveScabbard, "1"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kRemoveScabbard)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kRemoveScabbard);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::RemoveScabbard);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::Use1pWeaponModels, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kLoad1pWeaponModel)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kLoad1pWeaponModel);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::Load1pWeaponModel);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::UseWorldModel, "3"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kUseWorldModel)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kUseWorldModel);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::UseWorldModel);

				ImGui::NextColumn();

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::KeepTorchFlame, "4"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kKeepTorchFlame)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kKeepTorchFlame);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::KeepTorchFlame);

				if (ImGui::CheckboxFlagsT(
						LS(UIWidgetCommonStrings::DisableHavok, "5"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDisableHavok)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisableHavok);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::DisableHavok);

				if (ImGui::CheckboxFlagsT(
						LS(UIBaseConfigString::DropOnDeath, "6"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDropOnDeath)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDropOnDeath);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::DropOnDeath);

				UICommon::PopDisabled(a_disabled);

				ImGui::Columns();

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
				tresult = TreeEx(
					"f_an",
					true,
					"%s",
					LS(CommonStrings::Animation));
			}
			else
			{
				tresult = ImGui::TreeNodeEx(
					"f_an",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Animation));
			}

			if (tresult)
			{
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				UICommon::PushDisabled(a_disabled);

				bool paChanged = ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::PlayAnimation, "1"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::BaseFlags::kPlaySequence));

				if (paChanged)
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kPlaySequence);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::PlayAnimation);

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
						if (DrawDescriptionPopup(LS(CommonStrings::Sequence, "1")))
						{
							a_data.niControllerSequence = GetDescriptionPopupBuffer();

							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, niControllerSequence),
								a_data.niControllerSequence);

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}

						ImGui::EndPopup();
					}
				}

				if (ImGui::CheckboxFlagsT(
						LS(UIWidgetCommonStrings::DisableWeaponAnims, "2"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kDisableWeaponAnims)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisableWeaponAnims);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::DisableWeaponAnims);

				ImGui::NextColumn();

				paChanged = ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::AnimationEvent, "3"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::BaseFlags::kAnimationEvent));

				if (paChanged)
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kAnimationEvent);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
				}

				DrawTip(UITip::AnimationEvent);

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
						if (DrawDescriptionPopup(LS(CommonStrings::Event, "1")))
						{
							a_data.animationEvent = GetDescriptionPopupBuffer();

							PropagateMemberToEquipmentOverrides(
								a_baseConfig,
								offsetof(Data::configBaseValues_t, animationEvent),
								a_data.animationEvent);

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}

						ImGui::EndPopup();
					}
				}

				/*if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::DisableAnimEventForwarding, "4"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::BaseFlags::kDisableAnimEventForwarding)))
				{
					PropagateFlagToEquipmentOverrides(
						a_baseConfig,
						Data::BaseFlags::kDisableAnimEventForwarding);

					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
				}
				DrawTip(UITip::DisableAnimEventForwarding);*/

				UICommon::PopDisabled(a_disabled);

				ImGui::Columns();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrides(
			T                              a_handle,
			Data::configBase_t&            a_baseConfig,
			Data::equipmentOverrideList_t& a_list,
			const void*                    a_params,
			const stl::fixed_string&       a_slotName)
		{
			auto storecc = BaseConfigStoreCC();

			ImGui::PushID("config_equipment_overrides");

			const auto result = DrawEquipmentOverrideTreeContextMenu(a_handle, a_baseConfig, a_list, a_params);

			const bool empty = a_list.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result == BaseConfigEditorAction::Insert)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			bool r;

			if (storecc)
			{
				r = TreeEx(
					"tree",
					true,
					"%s",
					LS(UIBaseConfigString::EquipmentOverrides));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(UIBaseConfigString::EquipmentOverrides));
			}

			if (r)
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawEquipmentOverrideList(
						a_handle,
						a_baseConfig,
						a_list,
						a_params,
						a_slotName);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
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
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawEquipmentOverrideTreeContextMenu(
			T                              a_handle,
			Data::configBase_t&            a_baseConfig,
			Data::equipmentOverrideList_t& a_list,
			const void*                    a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("eo_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::New, "1"))
				{
					if (LCG_BM(CommonStrings::Item, "0"))
					{
						if (DrawDescriptionPopup())
						{
							a_list.emplace_back(
								a_baseConfig,
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

					if (LCG_BM(CommonStrings::Group, "1"))
					{
						if (DrawDescriptionPopup())
						{
							auto& e = a_list.emplace_back(
								a_baseConfig,
								GetDescriptionPopupBuffer());

							e.eoFlags.set(Data::EquipmentOverrideFlags::kIsGroup);

							OnBaseConfigChange(
								a_handle,
								a_params,
								PostChangeAction::Evaluate);

							ClearDescriptionPopupBuffer();

							result = BaseConfigEditorAction::Insert;
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set<Data::equipmentOverrideList_t>(a_list);
				}

				{
					auto clipData = UIClipboard::Get<Data::equipmentOverride_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_list.emplace_back(*clipData);
							OnBaseConfigChange(
								a_handle,
								a_params,
								PostChangeAction::Evaluate);

							result = BaseConfigEditorAction::Paste;
						}
					}
				}

				{
					auto clipData = UIClipboard::Get<Data::equipmentOverrideList_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_list = *clipData;

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
		void UIBaseConfigWidget<T>::DrawEquipmentOverrideList(
			T                              a_handle,
			Data::configBase_t&            a_baseConfig,
			Data::equipmentOverrideList_t& a_list,
			const void*                    a_params,
			const stl::fixed_string&       a_name)
		{
			if (a_list.empty())
			{
				return;
			}

			ImGui::PushID("equipment_override_list");

			int i = 0;

			auto it = a_list.begin();

			while (it != a_list.end())
			{
				/*if (SkipEquipmentOverride(*it))
				{
					++it;
					continue;
				}*/

				ImGui::PushID(i);

				const auto result = DrawEquipmentOverrideContextMenu(a_handle, a_params, *it);

				switch (result.action)
				{
				case BaseConfigEditorAction::Delete:

					it = a_list.erase(it);
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);

					break;
				case BaseConfigEditorAction::Insert:

					it = a_list.emplace(it, a_baseConfig, result.desc);
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					ImGui::SetNextItemOpen(true);

					break;
				case BaseConfigEditorAction::InsertGroup:

					it = a_list.emplace(it, a_baseConfig, result.desc);
					it->eoFlags.set(Data::EquipmentOverrideFlags::kIsGroup);
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					ImGui::SetNextItemOpen(true);

					break;
				case BaseConfigEditorAction::Swap:

					if (IterSwap(a_list, it, result.dir))
					{
						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					break;
				case BaseConfigEditorAction::Reset:

					static_cast<Data::configBaseValues_t&>(*it) = a_baseConfig;
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);

					break;
				case BaseConfigEditorAction::Rename:

					it->description = result.desc;
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);

					break;
				case BaseConfigEditorAction::Paste:

					if (auto clipData = UIClipboard::Get<Data::equipmentOverride_t>())
					{
						it = a_list.emplace(it, *clipData);
						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}
					[[fallthrough]];

				case BaseConfigEditorAction::PasteOver:

					ImGui::SetNextItemOpen(true);

					break;
				}

				if (it != a_list.end())
				{
					auto& e = *it;

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::TreeNodeEx(
							"eo_item",
							ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							e.description.c_str()))
					{
						ImGui::Spacing();

						DrawExtraEquipmentOverrideOptions(a_handle, a_baseConfig, a_params, e);

						DrawEquipmentOverrideConditionTree(
							e.conditions,
							[this, a_handle, a_params] {
								OnBaseConfigChange(
									a_handle,
									a_params,
									PostChangeAction::Evaluate);
							});

						ImGui::Spacing();

						if (e.eoFlags.test(Data::EquipmentOverrideFlags::kIsGroup))
						{
							ImGui::PushID("eo_group");
							ImGui::PushID("header");

							if (ImGui::CheckboxFlagsT(
									LS(CommonStrings::Continue, "0"),
									stl::underlying(std::addressof(e.eoFlags.value)),
									stl::underlying(Data::EquipmentOverrideFlags::kContinue)))
							{
								OnBaseConfigChange(
									a_handle,
									a_params,
									PostChangeAction::Evaluate);
							}
							DrawTip(UITip::EquipmentOverrideGroupContinue);

							ImGui::PopID();

							ImGui::Spacing();

							ImGui::PushID("item");

							DrawEquipmentOverrides(
								a_handle,
								a_baseConfig,
								e.group,
								a_params,
								a_name);

							ImGui::PopID();

							ImGui::PopID();
						}
						else
						{
							ImGui::PushID("eo_bc_values");

							DrawBaseConfigValues(
								a_handle,
								e,
								a_params,
								a_name,
								false,
								nullptr);

							ImGui::PopID();
						}

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
		bool UIBaseConfigWidget<T>::SkipEquipmentOverride(
			const Data::equipmentOverride_t& a_override) const
		{
			return false;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawExtraEquipmentOverrideOptions(
			T                          a_handle,
			Data::configBase_t&        a_data,
			const void*                a_params,
			Data::equipmentOverride_t& a_override)
		{
		}

		template <class T>
		auto UIBaseConfigWidget<T>::DrawEquipmentOverrideContextMenu(
			T                          a_handle,
			const void*                a_params,
			Data::equipmentOverride_t& a_data)
			-> EquipmentOverrideResult
		{
			EquipmentOverrideResult result;

			ImGui::PushID("eo_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				if (!a_data.description.empty())
				{
					SetDescriptionPopupBuffer(a_data.description);
				}
				else
				{
					ClearDescriptionPopupBuffer();
				}
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = BaseConfigEditorAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = BaseConfigEditorAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "2"))
					{
						if (LCG_BM(CommonStrings::Item, "0"))
						{
							if (DrawDescriptionPopup())
							{
								result.action = BaseConfigEditorAction::Insert;
								result.desc   = GetDescriptionPopupBuffer();

								ClearDescriptionPopupBuffer();
							}

							ImGui::EndMenu();
						}

						if (LCG_BM(CommonStrings::Group, "1"))
						{
							if (DrawDescriptionPopup())
							{
								result.action = BaseConfigEditorAction::InsertGroup;
								result.desc   = GetDescriptionPopupBuffer();

								ClearDescriptionPopupBuffer();
							}

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverride_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = BaseConfigEditorAction::Paste;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "4")))
				{
					result.action = BaseConfigEditorAction::Delete;
				}

				if (LCG_BM(CommonStrings::Rename, "5"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = BaseConfigEditorAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Reset, "6")))
				{
					result.action = BaseConfigEditorAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_data);
				}

				bool hasClipData = UIClipboard::Get<Data::equipmentOverride_t>() ||
				                   UIClipboard::Get<Data::configBaseValues_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "8"),
						nullptr,
						false,
						hasClipData))
				{
					bool update = false;

					if (auto cd1 = UIClipboard::Get<Data::equipmentOverride_t>())
					{
						a_data = *cd1;

						update = true;
					}
					else if (auto cd2 = UIClipboard::Get<Data::configBaseValues_t>())
					{
						static_cast<Data::configBaseValues_t&>(a_data) = *cd2;

						update = true;
					}

					if (update)
					{
						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);

						result.action = BaseConfigEditorAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
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
				if (LCG_BM(CommonStrings::New, "1"))
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

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set<Data::effectShaderList_t>(a_data.effectShaders);
				}

				{
					auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "3"),
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
							LS(CommonStrings::PasteOver, "4"),
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
			bool disabled = !GetEffectControllerEnabled();

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
				r = TreeEx(
					"tree",
					true,
					"%s",
					LS(UIBaseConfigString::EffectShaders));
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(UIBaseConfigString::EffectShaders));
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
		EquipmentOverrideResult UIBaseConfigWidget<T>::DrawEffectShaderHolderContextMenu(
			T                                 a_handle,
			const void*                       a_params,
			Data::effectShaderList_t&         a_list,
			Data::configEffectShaderHolder_t& a_data)
		{
			EquipmentOverrideResult result;

			ImGui::PushID("es_holder_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = BaseConfigEditorAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = BaseConfigEditorAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "2"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = BaseConfigEditorAction::Insert;
							result.desc   = GetDescriptionPopupBuffer();

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = BaseConfigEditorAction::Paste;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_BM(UIBaseConfigString::AddShaderEntry, "2"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = BaseConfigEditorAction::Create;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "4")))
				{
					result.action = BaseConfigEditorAction::Delete;
				}

				if (LCG_BM(CommonStrings::Rename, "5"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = BaseConfigEditorAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "8"),
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

						result.action = BaseConfigEditorAction::PasteOver;
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
				case BaseConfigEditorAction::Delete:
					it = list.erase(it);

					OnBaseConfigChange(
						a_handle,
						a_params,
						PostChangeAction::Evaluate);

					break;
				case BaseConfigEditorAction::Insert:
					it = list.emplace(it, result.desc);

					OnBaseConfigChange(
						a_handle,
						a_params,
						PostChangeAction::Evaluate);

					ImGui::SetNextItemOpen(true);
					break;
				case BaseConfigEditorAction::Create:

					if (it->data.try_emplace(result.desc).second)
					{
						TriggerEffectShaderUpdate(a_handle, *it, a_params);
						ImGui::SetNextItemOpen(true);
					}
					else
					{
						QueueNotification(
							LS(CommonStrings::Error),
							"%s",
							LS(UIBaseConfigString::EffectShaderEntryExists));
					}

					break;
				case BaseConfigEditorAction::Swap:

					if (IterSwap(list, it, result.dir))
					{
						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					}

					break;
				case BaseConfigEditorAction::Rename:
					it->description = result.desc;
					//TriggerEffectShaderUpdate(a_handle, *it, a_params);
					break;
				case BaseConfigEditorAction::Paste:
					if (auto clipData = UIClipboard::Get<Data::configEffectShaderHolder_t>())
					{
						it = list.emplace(it, *clipData);

						OnBaseConfigChange(
							a_handle,
							a_params,
							PostChangeAction::Evaluate);
					}
					[[fallthrough]];
				case BaseConfigEditorAction::PasteOver:
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

						ImGui::Text("LUID: %llx.%lld", luid.p1, luid.p2);

						ImGui::Spacing();

						const auto r = DrawEquipmentOverrideEntryConditionHeaderContextMenu(
							e.conditions,
							[this, a_handle, a_params, &a_data, &e] {
								TriggerEffectShaderUpdate(
									a_handle,
									e,
									a_params);
							});

						bool empty = e.conditions.empty();

						if (!empty)
						{
							if (r == BaseConfigEditorAction::PasteOver ||
							    r == BaseConfigEditorAction::Insert)
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
								LS(CommonStrings::Conditions)))
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

						DrawEffectShaderEditor(params, e);

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

	}
}