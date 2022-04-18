#pragma once

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UINotificationInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/UITips.h"

#include "Form/UIFormFilterWidget.h"
#include "Form/UIFormSelectorWidget.h"
#include "UIBaseConfigWidgetStrings.h"
#include "UIConditionParamEditorWidget.h"
#include "UIDescriptionPopup.h"
#include "UIEffectShaderEditorWidget.h"
#include "UINodeSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPopupToggleButtonWidget.h"
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
		enum class BaseConfigEditorAction : std::uint8_t
		{
			None,
			Insert,
			Delete,
			Swap,
			Edit,
			Reset,
			Rename,
			ClearKeyword,
			Copy,
			Paste,
			PasteOver,
			Create
		};

		struct EquipmentOverrideResult
		{
			BaseConfigEditorAction               action{ BaseConfigEditorAction::None };
			Game::FormID                         form;
			Data::EquipmentOverrideConditionType entryType;

			union
			{
				BIPED_OBJECT             biped;
				Data::ExtraConditionType excond;
				Data::ObjectSlotExtra    slot;
			};

			std::string   desc;
			SwapDirection dir;
		};

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
			public UIComparisonOperatorSelector,
			UIConditionParamExtraInterface,
			UIEffectShaderEditorWidget<baseEffectShaderEditorParams_t<T>>,
			public virtual UICollapsibles,
			public virtual UIDescriptionPopupWidget,
			public virtual UITransformSliderWidget,
			public virtual UITipsInterface,
			public virtual UINotificationInterface,
			public virtual UILocalizationInterface,
			public virtual UISettingsInterface
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
			void PropagateToEquipmentOverrides(
				Data::configBase_t* a_data);

			void PropagateFlagToEquipmentOverrides(
				Data::configBase_t* a_data,
				Data::BaseFlags     a_mask);

		private:
			void DrawEquipmentOverrides(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

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
				T                   a_handle,
				Data::configBase_t& a_data,
				const void*         a_params);

			void DrawEquipmentOverrideList(
				T                        a_handle,
				Data::configBase_t&      a_data,
				const void*              a_params,
				const stl::fixed_string& a_slotName);

			template <class Tu>
			BaseConfigEditorAction DrawEquipmentOverrideEntryConditionHeaderContextMenu(
				T                                       a_handle,
				Data::equipmentOverrideConditionList_t& a_entry,
				Tu                                      a_updFunc);

			template <class Tu>
			void DrawEquipmentOverrideEntryConditionTable(
				T                                       a_handle,
				Data::configBase_t&                     a_baseData,
				Data::equipmentOverrideConditionList_t& a_entry,
				bool                                    a_isnested,
				Tu                                      a_updFunc);

			EquipmentOverrideResult DrawEquipmentOverrideEntryContextMenu(
				bool a_drawDelete);

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

			virtual bool DrawConditionParamExtra(
				void*       a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args) override;

			void UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType a_type);

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

			Game::FormID                                m_aoNewEntryID;
			Game::FormID                                m_aoNewEntryKWID;
			Game::FormID                                m_aoNewEntryRaceID;
			Game::FormID                                m_aoNewEntryActorID;
			Game::FormID                                m_aoNewEntryNPCID;
			Game::FormID                                m_aoNewEntryGlobID;
			BIPED_OBJECT                                m_ooNewBiped{ BIPED_OBJECT::kNone };
			Data::ExtraConditionType                    m_ooNewExtraCond{ Data::ExtraConditionType::kNone };
			Data::ObjectSlotExtra                       m_aoNewSlot{ Data::ObjectSlotExtra::kNone };
			UIConditionParamEditorWidget                m_condParamEditor;
			UIFormSelectorWidget                        m_ffFormSelector;
			UIFormFilterWidget<bcFormFilterParams_t<T>> m_formFilter;

			struct
			{
				std::shared_ptr<const UIFormBrowser::tab_filter_type> form_common;
			} m_type_filters;

			Controller& m_controller;
		};

		template <class T>
		UIBaseConfigWidget<T>::UIBaseConfigWidget(
			Controller& a_controller) :
			UINodeSelectorWidget(a_controller),
			UIFormLookupInterface(a_controller),
			UIComparisonOperatorSelector(a_controller),
			UIEffectShaderEditorWidget<baseEffectShaderEditorParams_t<T>>(a_controller),
			m_controller(a_controller),
			m_condParamEditor(a_controller),
			m_ffFormSelector(a_controller, FormInfoFlags::kNone, true),
			m_formFilter(a_controller, m_ffFormSelector)
		{
			m_type_filters.form_common = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESAmmo::kTypeID,
			      TESObjectLIGH::kTypeID,
			      TESObjectMISC::kTypeID,
			      IngredientItem::kTypeID,
			      AlchemyItem::kTypeID,
			      TESKey::kTypeID,
			      TESObjectBOOK::kTypeID,
			      TESSoulGem::kTypeID,
			      ScrollItem::kTypeID,
			      SpellItem::kTypeID,
			      IFormDatabase::EXTRA_TYPE_ARMOR });

			m_condParamEditor.SetExtraInterface(this);

			m_formFilter.SetOnChangeFunc([this](auto& a_params) {
				OnBaseConfigChange(
					a_params.handle,
					a_params.params,
					PostChangeAction::Evaluate);
			});
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
				a_params,
				a_slotName);

			const bool disabled = a_data.flags.test(Data::BaseFlags::kDisabled) &&
			                      a_data.equipmentOverrides.empty();

			UICommon::PushDisabled(disabled);

			DrawEffectShaders(
				a_handle,
				a_data,
				a_params,
				a_slotName);

			DrawFiltersTree(a_handle, a_data, a_params);

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

			auto srcVal = reinterpret_cast<Tpv*>(std::uintptr_t(src) + a_offset);

			for (auto& e : a_data->equipmentOverrides)
			{
				auto dstVal = reinterpret_cast<Tpv*>(std::uintptr_t(std::addressof(static_cast<Data::configBaseValues_t&>(e))) + a_offset);

				*dstVal = *srcVal;
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

			for (auto& e : a_data->equipmentOverrides)
			{
				auto& dst = static_cast<Tpv&>(e);

				dst = *src;
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

			for (auto& e : a_data->equipmentOverrides)
			{
				e.flags.value = (e.flags.value & ~a_mask) | (a_data->flags.value & a_mask);
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
			auto storecc = BaseConfigStoreCC() && a_storecc;

			const bool disabled = a_data.flags.test(Data::BaseFlags::kDisabled);

			ImGui::PushID("bc_flags");
			{
				bool r;

				if (storecc)
				{
					r = TreeEx("tree", true, "%s", LS(CommonStrings::Flags));
				}
				else
				{
					r = ImGui::TreeNodeEx(
						"tree",
						ImGuiTreeNodeFlags_DefaultOpen |
							ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						LS(CommonStrings::Flags));
				}

				if (r)
				{
					ImGui::Spacing();

					ImGui::Columns(2, nullptr, false);

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::Disabled, "1"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::BaseFlags::kDisabled)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kDisabled);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					UICommon::PushDisabled(disabled);

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
							LS(UIBaseConfigString::RemoveScabbard, "5"),
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
							LS(UIBaseConfigString::IgnoreRaceEquipTypes, "6"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::BaseFlags::kIgnoreRaceEquipTypes)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kIgnoreRaceEquipTypes);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					DrawTipWarn(UITip::IgnoreRaceEquipTypesSlot);

					bool paChanged = ImGui::CheckboxFlagsT(
						LS(UIWidgetCommonStrings::PlayAnimation, "7"),
						stl::underlying(std::addressof(a_data.flags.value)),
						stl::underlying(Data::BaseFlags::kPlayAnimation));

					if (paChanged)
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kPlayAnimation);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					DrawTip(UITip::PlayAnimation);

					if (a_data.flags.test(Data::BaseFlags::kPlayAnimation))
					{
						if (paChanged &&
						    a_data.niControllerSequence.empty())
						{
							ImGui::OpenPopup("7ctx");
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

					ImGui::NextColumn();

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::DropOnDeath, "9"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::BaseFlags::kDropOnDeath)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kDropOnDeath);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}
					DrawTip(UITip::DropOnDeath);

					const bool atmReference = a_data.flags.test(Data::BaseFlags::kReferenceMode);

					UICommon::PushDisabled(!atmReference);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::SyncReference, "A"),
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

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::Invisible, "B"),
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
							LS(UIBaseConfigString::Use1pWeaponModels, "C"),
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
							LS(UIBaseConfigString::UseWorldModel, "D"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::BaseFlags::kUseWorldModel)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kUseWorldModel);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					DrawTip(UITip::UseWorldModel);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::KeepTorchFlame, "E"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::BaseFlags::kKeepTorchFlame)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::BaseFlags::kKeepTorchFlame);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					DrawTip(UITip::KeepTorchFlame);

					UICommon::PopDisabled(disabled);

					ImGui::Columns();

					if (a_baseConfig)
					{
						ImGui::PushID("extra_flags");
						DrawExtraFlags(a_handle, a_data, a_baseConfig, a_params);
						ImGui::PopID();
					}

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}
			ImGui::PopID();

			ImGui::PushID("bc_node");
			{
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

					UICommon::PushDisabled(disabled);

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

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::AttachNode);
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

					UICommon::PopDisabled(disabled);

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}
			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrides(
			T                        a_handle,
			Data::configBase_t&      a_data,
			const void*              a_params,
			const stl::fixed_string& a_slotName)
		{
			auto storecc = BaseConfigStoreCC();

			ImGui::PushID("config_equipment_overrides");

			const auto result = DrawEquipmentOverrideTreeContextMenu(a_handle, a_data, a_params);

			const bool empty = a_data.equipmentOverrides.empty();

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
						a_data,
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
			T                   a_handle,
			Data::configBase_t& a_data,
			const void*         a_params)
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
					if (DrawDescriptionPopup())
					{
						a_data.equipmentOverrides.emplace_back(
							a_data,
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
					UIClipboard::Set<Data::equipmentOverrideList_t>(a_data.equipmentOverrides);
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
							a_data.equipmentOverrides.emplace_back(*clipData);
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
							a_data.equipmentOverrides = *clipData;

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
			T                        a_handle,
			Data::configBase_t&      a_data,
			const void*              a_params,
			const stl::fixed_string& a_name)
		{
			if (a_data.equipmentOverrides.empty())
			{
				return;
			}

			ImGui::PushID("equipment_override_list");

			int i = 0;

			auto it = a_data.equipmentOverrides.begin();

			while (it != a_data.equipmentOverrides.end())
			{
				if (SkipEquipmentOverride(*it))
				{
					++it;
					continue;
				}

				ImGui::PushID(i);

				const auto result = DrawEquipmentOverrideContextMenu(a_handle, a_params, *it);

				switch (result.action)
				{
				case BaseConfigEditorAction::Delete:
					it = a_data.equipmentOverrides.erase(it);
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					break;
				case BaseConfigEditorAction::Insert:
					it = a_data.equipmentOverrides.emplace(it, a_data, result.desc);
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					ImGui::SetNextItemOpen(true);
					break;
				case BaseConfigEditorAction::Swap:

					if (IterSwap(a_data.equipmentOverrides, it, result.dir))
					{
						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					break;
				case BaseConfigEditorAction::Reset:
					static_cast<Data::configBaseValues_t&>(*it) = a_data;
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					break;
				case BaseConfigEditorAction::Rename:
					it->description = result.desc;
					OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					break;
				case BaseConfigEditorAction::Paste:
					if (auto clipData = UIClipboard::Get<Data::equipmentOverride_t>())
					{
						it = a_data.equipmentOverrides.emplace(it, *clipData);
						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}
					// fallthrough
				case BaseConfigEditorAction::PasteOver:
					ImGui::SetNextItemOpen(true);
					break;
				}

				if (it != a_data.equipmentOverrides.end())
				{
					auto& e = *it;

					if (ImGui::TreeNodeEx(
							"eo_item",
							ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							e.description.c_str()))
					{
						ImGui::Spacing();

						DrawExtraEquipmentOverrideOptions(a_handle, a_data, a_params, e);

						const auto r = DrawEquipmentOverrideEntryConditionHeaderContextMenu(
							a_handle,
							e.conditions,
							[this, a_handle, a_params] {
								OnBaseConfigChange(
									a_handle,
									a_params,
									PostChangeAction::Evaluate);
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
								"eo_item_match",
								ImGuiTreeNodeFlags_SpanAvailWidth |
									ImGuiTreeNodeFlags_DefaultOpen,
								"%s",
								LS(CommonStrings::Conditions)))
						{
							if (!empty)
							{
								ImGui::Spacing();

								DrawEquipmentOverrideEntryConditionTable(
									a_handle,
									a_data,
									e.conditions,
									false,
									[this, a_handle, a_params] {
										OnBaseConfigChange(
											a_handle,
											a_params,
											PostChangeAction::Evaluate);
									});
							}

							ImGui::TreePop();
						}

						UICommon::PopDisabled(empty);

						ImGui::Spacing();

						ImGui::PushID("eo_bc_values");

						DrawBaseConfigValues(
							a_handle,
							e,
							a_params,
							a_name,
							false,
							nullptr);

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
		template <class Tu>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawEquipmentOverrideEntryConditionHeaderContextMenu(
			T                                       a_handle,
			Data::equipmentOverrideConditionList_t& a_entry,
			Tu                                      a_updFunc)
		{
			BaseConfigEditorAction action{ BaseConfigEditorAction ::None };

			const auto result = DrawEquipmentOverrideEntryContextMenu(true);

			switch (result.action)
			{
			case BaseConfigEditorAction::Insert:
				{
					switch (result.entryType)
					{
					case Data::EquipmentOverrideConditionType::Type:
						if (result.slot != Data::ObjectSlotExtra::kNone)
						{
							a_entry.emplace_back(
								result.slot);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Form:
					case Data::EquipmentOverrideConditionType::Actor:
					case Data::EquipmentOverrideConditionType::NPC:
					case Data::EquipmentOverrideConditionType::Keyword:
					case Data::EquipmentOverrideConditionType::Quest:
					case Data::EquipmentOverrideConditionType::Global:
						if (result.form)
						{
							a_entry.emplace_back(
								result.entryType,
								result.form);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Race:
					case Data::EquipmentOverrideConditionType::Furniture:
					case Data::EquipmentOverrideConditionType::Group:
					case Data::EquipmentOverrideConditionType::Location:
					case Data::EquipmentOverrideConditionType::Worldspace:
					case Data::EquipmentOverrideConditionType::Package:
					case Data::EquipmentOverrideConditionType::Weather:

						a_entry.emplace_back(
							result.entryType);

						action = result.action;

						a_updFunc();

						break;
					case Data::EquipmentOverrideConditionType::BipedSlot:
						if (result.biped != BIPED_OBJECT::kNone)
						{
							a_entry.emplace_back(
								result.biped);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Extra:
						if (result.excond != Data::ExtraConditionType::kNone)
						{
							a_entry.emplace_back(
								result.excond);

							action = result.action;

							a_updFunc();
						}
						break;
					}
				}

				ImGui::SetNextItemOpen(true);

				break;

			case BaseConfigEditorAction::Copy:
				UIClipboard::Set(a_entry);
				break;

			case BaseConfigEditorAction::PasteOver:
				if (auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>())
				{
					a_entry = *clipData;

					a_updFunc();

					action = BaseConfigEditorAction::PasteOver;
				}
				break;
			case BaseConfigEditorAction::Delete:
				a_entry.clear();

				action = BaseConfigEditorAction::Delete;

				a_updFunc();

				break;
			}

			return action;
		}

		template <class T>
		template <class Tu>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrideEntryConditionTable(
			T                                       a_handle,
			Data::configBase_t&                     a_baseData,
			Data::equipmentOverrideConditionList_t& a_entry,
			bool                                    a_isnested,
			Tu                                      a_updFunc)
		{
			if (a_isnested)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 2.f, 2.f });
			}
			else
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 5.f, 5.f });
			}

			constexpr int NUM_COLUMNS = 5;

			float width;

			if (a_isnested)
			{
				width = -1.0f;
			}
			else
			{
				width = -ImGui::GetFontSize();
			}

			if (ImGui::BeginTable(
					"eo_entry_match_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ width, 0.f }))
			{
				auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(LS(CommonStrings::Type), ImGuiTableColumnFlags_None, 40.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::Edit), ImGuiTableColumnFlags_None, 200.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::And), ImGuiTableColumnFlags_None, 15.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::Not), ImGuiTableColumnFlags_None, 15.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				int i = 0;

				auto it = a_entry.begin();

				while (it != a_entry.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();

					//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawEquipmentOverrideEntryContextMenu(false);

					switch (result.action)
					{
					case BaseConfigEditorAction::Delete:
						it = a_entry.erase(it);
						a_updFunc();
						break;
					case BaseConfigEditorAction::ClearKeyword:
						if (it->keyword.get_id())
						{
							it->keyword = 0;
							a_updFunc();
						}
						break;
					case BaseConfigEditorAction::Insert:

						switch (result.entryType)
						{
						case Data::EquipmentOverrideConditionType::Type:
							if (result.slot != Data::ObjectSlotExtra::kNone)
							{
								it = a_entry.emplace(
									it,
									result.slot);

								a_updFunc();
							}
							break;
						case Data::EquipmentOverrideConditionType::Form:
						case Data::EquipmentOverrideConditionType::Actor:
						case Data::EquipmentOverrideConditionType::NPC:
						case Data::EquipmentOverrideConditionType::Keyword:
						case Data::EquipmentOverrideConditionType::Quest:
						case Data::EquipmentOverrideConditionType::Global:
							if (result.form)
							{
								it = a_entry.emplace(
									it,
									result.entryType,
									result.form);

								a_updFunc();
							}
							break;
						case Data::EquipmentOverrideConditionType::Race:
						case Data::EquipmentOverrideConditionType::Furniture:
						case Data::EquipmentOverrideConditionType::Group:
						case Data::EquipmentOverrideConditionType::Location:
						case Data::EquipmentOverrideConditionType::Worldspace:
						case Data::EquipmentOverrideConditionType::Package:
						case Data::EquipmentOverrideConditionType::Weather:

							it = a_entry.emplace(
								it,
								result.entryType);

							a_updFunc();

							break;
						case Data::EquipmentOverrideConditionType::BipedSlot:
							if (result.biped != BIPED_OBJECT::kNone)
							{
								it = a_entry.emplace(
									it,
									result.biped);

								a_updFunc();
							}
							break;
						case Data::EquipmentOverrideConditionType::Extra:
							if (result.excond != Data::ExtraConditionType::kNone)
							{
								it = a_entry.emplace(
									it,
									result.excond);

								a_updFunc();
							}
							break;
						}

						break;

					case BaseConfigEditorAction::Swap:

						if (IterSwap(a_entry, it, result.dir))
						{
							a_updFunc();
						}

						break;
					}

					if (it != a_entry.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (e.fbf.type == Data::EquipmentOverrideConditionType::Group)
						{
							ImGui::TextUnformatted(LS(CommonStrings::Group));

							ImGui::TableSetColumnIndex(2);

							ImGui::PushID("cond_grp");

							DrawEquipmentOverrideEntryConditionHeaderContextMenu(
								a_handle,
								e.group.conditions,
								a_updFunc);

							DrawEquipmentOverrideEntryConditionTable(
								a_handle,
								a_baseData,
								e.group.conditions,
								true,
								a_updFunc);

							ImGui::PopID();
						}
						else
						{
							m_condParamEditor.Reset();

							UpdateMatchParamAllowedTypes(e.fbf.type);

							const char* tdesc;
							const char* vdesc;

							switch (e.fbf.type)
							{
							case Data::EquipmentOverrideConditionType::Form:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = LS(CommonStrings::Form);

								break;
							case Data::EquipmentOverrideConditionType::Type:

								m_condParamEditor.SetNext<ConditionParamItem::EquipmentSlotExtra>(
									e.slot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlotExtra);
								tdesc = LS(CommonStrings::Type);

								break;
							case Data::EquipmentOverrideConditionType::Keyword:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearKeyword);

								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = LS(CommonStrings::Keyword);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Keyword);

								break;
							case Data::EquipmentOverrideConditionType::Race:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = LS(CommonStrings::Race);

								break;
							case Data::EquipmentOverrideConditionType::Actor:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Actor);

								break;
							case Data::EquipmentOverrideConditionType::NPC:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::NPC);

								break;
							case Data::EquipmentOverrideConditionType::Furniture:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Furniture);
								tdesc = LS(CommonStrings::Furniture);

								break;
							case Data::EquipmentOverrideConditionType::BipedSlot:

								m_condParamEditor.SetNext<ConditionParamItem::BipedSlot>(
									e.bipedSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::BipedSlot);
								tdesc = LS(CommonStrings::Biped);

								break;
							case Data::EquipmentOverrideConditionType::Quest:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::QuestCondType>(
									e.questCondType);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Quest);

								break;
							case Data::EquipmentOverrideConditionType::Extra:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::CondExtra>(
									e.extraCondType);

								switch (e.extraCondType)
								{
								case Data::ExtraConditionType::kShoutEquipped:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Shout));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kInMerchantFaction:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Faction));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kCombatStyle:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::CombatStyle));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kClass:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Class));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kTimeOfDay:
									m_condParamEditor.SetNext<ConditionParamItem::TimeOfDay>(e.timeOfDay);
									break;
								}

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::CondExtra);
								tdesc = LS(CommonStrings::Extra);

								break;
							case Data::EquipmentOverrideConditionType::Location:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = LS(CommonStrings::Location);

								break;
							case Data::EquipmentOverrideConditionType::Worldspace:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Worldspace);

								break;
							case Data::EquipmentOverrideConditionType::Package:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::PackageType>(
									e.procedureType);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::PackageType);
								tdesc = LS(CommonStrings::Package);

								break;
							case Data::EquipmentOverrideConditionType::Weather:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::WeatherClass>(
									e.weatherClass);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::WeatherClass);
								tdesc = LS(CommonStrings::Weather);

								break;
							case Data::EquipmentOverrideConditionType::Global:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::CompOper>(
									e.compOperator);
								m_condParamEditor.SetNext<ConditionParamItem::Float>(
									e.f32a);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Global);

								break;
							default:
								tdesc = nullptr;
								vdesc = nullptr;
								break;
							}

							if (!vdesc)
							{
								vdesc = "";
							}

							ImGui::TextUnformatted(tdesc);

							ImGui::TableSetColumnIndex(2);

							bool r = ImGui::Selectable(
								LMKID<3>(vdesc, "sel_ctl"),
								false,
								ImGuiSelectableFlags_DontClosePopups);

							UICommon::ToolTip(vdesc);

							if (r)
							{
								m_condParamEditor.OpenConditionParamEditorPopup();
							}

							if (m_condParamEditor.DrawConditionParamEditorPopup())
							{
								a_updFunc();
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

						ImGui::TableSetColumnIndex(3);

						if (ImGui::CheckboxFlagsT(
								"##oper_and",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kAnd)))
						{
							a_updFunc();
						}

						ImGui::TableSetColumnIndex(4);

						if (ImGui::CheckboxFlagsT(
								"##oper_not",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kNot)))
						{
							a_updFunc();
						}

						ImGui::PopStyleVar();

						++it;
						i++;
					}

					//ImGui::PopStyleVar();

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		auto UIBaseConfigWidget<T>::DrawEquipmentOverrideEntryContextMenu(
			bool a_header)
			-> EquipmentOverrideResult
		{
			EquipmentOverrideResult result;

			ImGui::PushID("eo_entry_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_aoNewEntryID      = {};
				m_aoNewEntryKWID    = {};
				m_aoNewEntryRaceID  = {};
				m_aoNewEntryActorID = {};
				m_aoNewEntryNPCID   = {};
				m_aoNewEntryGlobID  = {};
				m_ooNewBiped        = BIPED_OBJECT::kNone;
				m_aoNewSlot         = Data::ObjectSlotExtra::kNone;
				m_ooNewExtraCond    = Data::ExtraConditionType::kNone;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (!a_header)
			{
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
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if (LCG_BM(CommonStrings::Type, "2"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector("##ss", m_aoNewSlot))
						{
							result.action    = BaseConfigEditorAction::Insert;
							result.slot      = m_aoNewSlot;
							result.entryType = Data::EquipmentOverrideConditionType::Type;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Form, "3"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Form);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Form;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Keyword, "4"))
					{
						if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
								m_aoNewEntryKWID))
						{
							if (m_aoNewEntryKWID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryKWID;
								result.entryType = Data::EquipmentOverrideConditionType::Keyword;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(UIWidgetCommonStrings::BipedSlot, "5"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								LS(CommonStrings::Biped, "bp"),
								m_ooNewBiped))
						{
							result.action    = BaseConfigEditorAction::Insert;
							result.biped     = m_ooNewBiped;
							result.entryType = Data::EquipmentOverrideConditionType::BipedSlot;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Actor, "6"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Actor);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryActorID))
						{
							if (m_aoNewEntryActorID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryActorID;
								result.entryType = Data::EquipmentOverrideConditionType::Actor;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::NPC, "7"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::NPC);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryNPCID))
						{
							if (m_aoNewEntryNPCID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryNPCID;
								result.entryType = Data::EquipmentOverrideConditionType::NPC;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Race, "8"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Race;
					}

					if (LCG_MI(CommonStrings::Furniture, "9"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Furniture;

						ImGui::CloseCurrentPopup();
					}

					if (LCG_BM(CommonStrings::Quest, "A"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Quest);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Quest;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Location, "B"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Location;
					}

					if (LCG_MI(CommonStrings::Worldspace, "C"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Worldspace;
					}

					if (LCG_MI(CommonStrings::Package, "D"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Package;
					}

					if (LCG_MI(CommonStrings::Weather, "E"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Weather;
					}

					if (LCG_BM(CommonStrings::Global, "F"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryGlobID))
						{
							if (m_aoNewEntryGlobID)
							{
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryGlobID;
								result.entryType = Data::EquipmentOverrideConditionType::Global;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Extra, "Y"))
					{
						if (m_condParamEditor.DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							result.action    = BaseConfigEditorAction::Insert;
							result.excond    = m_ooNewExtraCond;
							result.entryType = Data::EquipmentOverrideConditionType::Extra;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Group, "Z"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Group;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = BaseConfigEditorAction::Delete;
				}

				if (!a_header)
				{
					if (LCG_MI(UIWidgetCommonStrings::ClearKeyword, "3"))
					{
						result.action = BaseConfigEditorAction::ClearKeyword;
					}
				}
				else
				{
					ImGui::Separator();

					if (LCG_MI(CommonStrings::Copy, "3"))
					{
						result.action = BaseConfigEditorAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = BaseConfigEditorAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
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
		bool UIBaseConfigWidget<T>::DrawConditionParamExtra(void* a_p1, const void*)
		{
			auto match = static_cast<Data::equipmentOverrideCondition_t*>(a_p1);

			ImGui::PushID("match_param_extra");

			bool result = false;

			switch (match->fbf.type)
			{
			case Data::EquipmentOverrideConditionType::Form:
			case Data::EquipmentOverrideConditionType::Type:
			case Data::EquipmentOverrideConditionType::Keyword:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Equipped, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchEquipped));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Or, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchCategoryOperOR));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Displayed, "3"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchSlots));

				DrawTip(UITip::EquippedConditions);

				if (match->fbf.type == Data::EquipmentOverrideConditionType::Form &&
				    !match->flags.test_any(Data::EquipmentOverrideConditionFlags::kMatchAll))
				{
					ImGui::Spacing();

					result |= ImGui::CheckboxFlagsT(
						LS(CommonStrings::Count, "4"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

					bool disabled = !match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1);

					UICommon::PushDisabled(disabled);

					ImGui::SameLine();

					ImGui::PushItemWidth(ImGui::GetFontSize() * 6.5f);

					result |= DrawComparisonOperatorSelector(match->compOperator);

					ImGui::PopItemWidth();

					ImGui::SameLine();

					result |= ImGui::InputScalar(
						"##5",
						ImGuiDataType_U32,
						std::addressof(match->count),
						nullptr,
						nullptr,
						"%u",
						ImGuiInputTextFlags_EnterReturnsTrue);

					UICommon::PopDisabled(disabled);
				}

				break;

			case Data::EquipmentOverrideConditionType::Furniture:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::LayingDown, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				break;

			case Data::EquipmentOverrideConditionType::BipedSlot:

				result |= ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorWidgetStrings::MatchSkin, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

				DrawTip(UITip::MatchSkin);

				result |= ImGui::CheckboxFlagsT(
					"!##2",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorWidgetStrings::IsBolt, "3"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				break;

			case Data::EquipmentOverrideConditionType::Location:

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				DrawTip(UITip::MatchChildLoc);

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchEither, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchCategoryOperOR));

				DrawTip(UITip::MatchEitherFormKW);

				break;

			case Data::EquipmentOverrideConditionType::Worldspace:

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				DrawTip(UITip::MatchWorldspaceParent);

				break;

			case Data::EquipmentOverrideConditionType::Race:

				result |= ImGui::CheckboxFlagsT(
					"!##1",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::IsPlayable, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				result |= ImGui::CheckboxFlagsT(
					"!##3",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch4));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::ChildRace, "4"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		bool UIBaseConfigWidget<T>::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			auto match = static_cast<Data::equipmentOverrideCondition_t*>(a_args.p3);

			bool result = false;

			ImGui::PushID("match_item_extra");

			switch (match->fbf.type)
			{
			case Data::EquipmentOverrideConditionType::Type:
			case Data::EquipmentOverrideConditionType::Furniture:
			case Data::EquipmentOverrideConditionType::BipedSlot:
			case Data::EquipmentOverrideConditionType::Location:
			case Data::EquipmentOverrideConditionType::Package:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					if (match->fbf.type == Data::EquipmentOverrideConditionType::BipedSlot &&
					    match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag2))
					{
						a_args.disable = true;
					}

					ImGui::SameLine();
				}
				else if (
					a_item == ConditionParamItem::Keyword ||
					a_item == ConditionParamItem::PackageType)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Data::EquipmentOverrideConditionType::Form:

				if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;
			case Data::EquipmentOverrideConditionType::Race:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;

			case Data::EquipmentOverrideConditionType::Extra:

				if (a_item == ConditionParamItem::Form)
				{
					switch (match->extraCondType)
					{
					case Data::ExtraConditionType::kShoutEquipped:
					case Data::ExtraConditionType::kInMerchantFaction:
					case Data::ExtraConditionType::kCombatStyle:
					case Data::ExtraConditionType::kClass:

						result = ImGui::CheckboxFlagsT(
							"!##ctl_neg_1",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

						ImGui::SameLine();

						a_args.hide = false;
						break;
					default:
						a_args.hide = true;
						break;
					}
				}

				break;

			case Data::EquipmentOverrideConditionType::Weather:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::WeatherClass)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::UpdateMatchParamAllowedTypes(
			Data::EquipmentOverrideConditionType a_type)
		{
			switch (a_type)
			{
			case Data::EquipmentOverrideConditionType::Race:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Furniture:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Furniture));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Quest:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Quest));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Actor:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Data::EquipmentOverrideConditionType::NPC:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Location:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Location));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Worldspace:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Worldspace));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Package:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Package));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Weather:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Weather));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Global:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Global));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			default:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.form_common);
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			}
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
					// fallthrough
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

						ImGui::Text("LUID: %llx.%llx", luid.p1, luid.p2);

						ImGui::Spacing();

						const auto r = DrawEquipmentOverrideEntryConditionHeaderContextMenu(
							a_handle,
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
									a_handle,
									a_data,
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