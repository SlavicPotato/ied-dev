#pragma once

#include "IED/UI//Controls/UICollapsibles.h"
#include "IED/UI//UIClipboard.h"
#include "IED/UI//UICommon.h"
#include "IED/UI//UITips.h"

#include "IED/ConfigOverride.h"
#include "IED/StringHolder.h"

#include "IED/UI//UIFormLookupInterface.h"
#include "IED/UI//UINotificationInterface.h"

#include "Form/UIFormFilterWidget.h"
#include "Form/UIFormSelectorWidget.h"
#include "UIBaseConfigWidgetStrings.h"
#include "UIConditionParamEditorWidget.h"
#include "UIDescriptionPopup.h"
#include "UINodeSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPopupToggleButtonWidget.h"
#include "UITransformSliderWidget.h"
#include "UIWidgetsCommon.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorStrings.h"

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
			PasteOver
		};

		struct EquipmentOverrideResult
		{
			BaseConfigEditorAction action{ BaseConfigEditorAction::None };
			Game::FormID form;
			Data::EquipmentOverrideConditionType entryType;
			Data::ObjectSlotExtra slot{ Data::ObjectSlotExtra::kNone };
			Biped::BIPED_OBJECT biped{ Biped::BIPED_OBJECT::kNone };
			const std::string* desc;
			SwapDirection dir;
		};

		template <class T>
		struct bcFormFilterParams_t
		{
			T handle;
			const void* params;
		};

		template <class T>
		class UIBaseConfigWidget :
			public UINodeSelectorWidget,
			public UIFormLookupInterface,
			UIConditionParamExtraInterface,
			virtual public UICollapsibles,
			virtual public UIDescriptionPopupWidget,
			virtual public UITransformSliderWidget,
			virtual public UITipsInterface,
			virtual public UIPopupToggleButtonWidget,
			virtual public UILocalizationInterface
		{
		public:
			UIBaseConfigWidget(
				Controller& a_controller);

			void DrawBaseConfig(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params,
				const stl::fixed_string& a_slotName);

			void DrawBaseConfigValues(
				T a_handle,
				Data::configBaseValues_t& a_data,
				const void* a_params,
				const stl::fixed_string& a_slotName,
				bool a_storecc,
				Data::configBase_t* a_baseConfig);

		protected:
			virtual void OnBaseConfigChange(
				T a_handle,
				const void* a_params,
				PostChangeAction a_action) = 0;

			template <class Tpv>
			void PropagateToEquipmentOverrides(
				Data::configBase_t* a_data,
				std::ptrdiff_t a_offset,
				Tpv* a_value);

			void PropagateFlagToEquipmentOverrides(
				Data::configBase_t* a_data,
				Data::FlagsBase a_mask);

		private:
			virtual void DrawExtraFlags(
				T a_handle,
				Data::configBaseValues_t& a_data,
				Data::configBase_t* a_baseConfig,
				const void* a_params);

			BaseConfigEditorAction DrawFiltersTreeContextMenu(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params);

			void DrawFiltersTree(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params);

			virtual constexpr bool BaseConfigStoreCC() const = 0;

			BaseConfigEditorAction DrawEquipmentOverrideTreeContextMenu(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params);

			void DrawEquipmentOverrideList(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params,
				const stl::fixed_string& a_slotName);

			BaseConfigEditorAction DrawEquipmentOverrideEntryConditionHeaderContextMenu(
				T a_handle,
				Data::equipmentOverrideConditionList_t& a_entry,
				const void* a_params);

			void DrawEquipmentOverrideEntryConditionTable(
				T a_handle,
				Data::configBase_t& a_baseData,
				Data::equipmentOverrideConditionList_t& a_entry,
				const void* a_params,
				bool a_isnested);

			EquipmentOverrideResult DrawEquipmentOverrideEntryContextMenu(
				bool a_drawDelete);

			virtual bool GetEnableEquipmentOverridePropagation() = 0;

			virtual bool SkipEquipmentOverride(
				const Data::equipmentOverride_t& a_override) const;

			virtual void DrawExtraEquipmentOverrideOptions(
				T a_handle,
				Data::configBase_t& a_data,
				const void* a_params,
				Data::equipmentOverride_t& a_override);

			EquipmentOverrideResult DrawEquipmentOverrideContextMenu(
				T a_handle,
				const void* a_params,
				Data::equipmentOverride_t& a_data);

			virtual bool DrawConditionParamExtra(
				void* a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem a_item,
				ConditionParamItemExtraArgs& a_args) override;

			void UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType a_type);

			Game::FormID m_aoNewEntryID;
			Game::FormID m_aoNewEntryKWID;
			Game::FormID m_aoNewEntryRaceID;
			Biped::BIPED_OBJECT m_ooNewBiped{ Biped::BIPED_OBJECT::kNone };

			Data::ObjectSlotExtra m_aoNewSlot{ Data::ObjectSlotExtra::kNone };

			UIConditionParamEditorWidget m_matchParamEditor;

			UIFormSelectorWidget m_ffFormSelector;
			UIFormFilterWidget<bcFormFilterParams_t<T>> m_formFilter;

			UINotificationInterface m_notif;

			struct
			{
				std::shared_ptr<const UIFormBrowser::tab_filter_type> form_common;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> furniture;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> race;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> actor;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> npc;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> quest;
			} m_type_filters;

			Controller& m_controller;
		};

		template <class T>
		UIBaseConfigWidget<T>::UIBaseConfigWidget(
			Controller& a_controller) :
			UINodeSelectorWidget(a_controller),
			UIFormLookupInterface(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_matchParamEditor(a_controller),
			m_notif(a_controller),
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

			m_type_filters.furniture = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>{
					TESFurniture::kTypeID });

			m_type_filters.race = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESRace::kTypeID });

			m_type_filters.actor = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ Actor::kTypeID });

			m_type_filters.npc = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESNPC::kTypeID });

			m_type_filters.quest = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESQuest::kTypeID });

			m_matchParamEditor.SetExtraInterface(this);

			m_formFilter.SetOnChangeFunc([this](auto& a_params) {
				OnBaseConfigChange(
					a_params.handle,
					a_params.params,
					PostChangeAction::Evaluate);
			});
		}

		template <class T>
		inline void UIBaseConfigWidget<T>::DrawBaseConfig(
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params,
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
				r = TreeEx("tree", true, "%s", LS(UIBaseConfigString::EquipmentOverrides));
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

					DrawEquipmentOverrideList(a_handle, a_data, a_params, a_slotName);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();

			const bool disabled = a_data.flags.test(Data::FlagsBase::kDisabled) &&
			                      a_data.equipmentOverrides.empty();

			UICommon::PushDisabled(disabled);

			DrawFiltersTree(a_handle, a_data, a_params);

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawFiltersTreeContextMenu(
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("context");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			DrawPopupToggleButton("open", "context_menu");

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
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params)
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
							m_ffFormSelector.SetAllowedTypes(m_type_filters.actor);
							m_ffFormSelector.SetFormBrowserEnabled(false);
						});

					ImGui::PopID();

					ImGui::PushID("2");

					m_formFilter.DrawFormFiltersTree(
						LS(UIWidgetCommonStrings::NPCFilters),
						ffparams,
						a_data.filters->npcFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(m_type_filters.npc);
							m_ffFormSelector.SetFormBrowserEnabled(true);
						});

					ImGui::PopID();

					ImGui::PushID("3");

					m_formFilter.DrawFormFiltersTree(
						LS(UIWidgetCommonStrings::RaceFilters),
						ffparams,
						a_data.filters->raceFilter,
						[&] {
							m_ffFormSelector.SetAllowedTypes(m_type_filters.race);
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
		void UIBaseConfigWidget<T>::PropagateToEquipmentOverrides(
			Data::configBase_t* a_data,
			std::ptrdiff_t a_offset,
			Tpv* a_value)
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
		void UIBaseConfigWidget<T>::PropagateFlagToEquipmentOverrides(
			Data::configBase_t* a_data,
			Data::FlagsBase a_mask)
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
			T a_handle,
			Data::configBaseValues_t& a_data,
			const void* a_params,
			const stl::fixed_string& a_slotName,
			bool a_storecc,
			Data::configBase_t* a_baseConfig)
		{
			auto storecc = BaseConfigStoreCC() && a_storecc;

			const bool disabled = a_data.flags.test(Data::FlagsBase::kDisabled);

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
							stl::underlying(Data::FlagsBase::kDisabled)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kDisabled);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					UICommon::PushDisabled(disabled);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::HideInFurniture, "2"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kHideIfUsingFurniture)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kHideIfUsingFurniture);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}
					DrawTip(UITip::HideInFurniture);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::HideLayingDown, "3"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kHideLayingDown)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kHideLayingDown);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}
					DrawTip(UITip::HideLayingDown);

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::kPlaySound, "4"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kPlaySound)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kPlaySound);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::RemoveScabbard, "5"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kRemoveScabbard)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kRemoveScabbard);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}
					DrawTip(UITip::RemoveScabbard);

					ImGui::NextColumn();

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::DropOnDeath, "6"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kDropOnDeath)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kDropOnDeath);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}
					DrawTip(UITip::DropOnDeath);

					const bool atmReference = a_data.flags.test(Data::FlagsBase::kReferenceMode);

					UICommon::PushDisabled(!atmReference);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::SyncReference, "8"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kSyncReferenceTransform)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kSyncReferenceTransform);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					UICommon::PopDisabled(!atmReference);

					DrawTip(UITip::SyncReferenceNode);

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::Invisible, "9"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kInvisible)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kInvisible);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
					}

					DrawTip(UITip::Invisible);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::Use1pWeaponModels, "A"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kLoad1pWeaponModel)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kLoad1pWeaponModel);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					DrawTip(UITip::Load1pWeaponModel);

					if (ImGui::CheckboxFlagsT(
							LS(UIBaseConfigString::KeepTorchFlame, "B"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::FlagsBase::kKeepTorchFlame)))
					{
						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kKeepTorchFlame);

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
							a_data.flags.test(Data::FlagsBase::kReferenceMode)))
					{
						a_data.flags.set(Data::FlagsBase::kReferenceMode);

						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kReferenceMode);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

					tmpb = a_data.targetNode.managed();

					UICommon::PushDisabled(tmpb);

					if (ImGui::RadioButton(
							LS(CommonStrings::Parent, "2"),
							!a_data.flags.test(Data::FlagsBase::kReferenceMode)))
					{
						a_data.flags.clear(Data::FlagsBase::kReferenceMode);

						PropagateFlagToEquipmentOverrides(
							a_baseConfig,
							Data::FlagsBase::kReferenceMode);

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
					}

					UICommon::PopDisabled(tmpb);

					DrawTip(UITip::AttachmentMode);

					ImGui::Spacing();

					if (DrawNodeSelector(
							LS(UIWidgetCommonStrings::TargetNode, "ns"),
							!a_data.flags.test(Data::FlagsBase::kReferenceMode),
							a_data.targetNode))
					{
						PropagateToEquipmentOverrides(
							a_baseConfig,
							offsetof(Data::configBaseValues_t, targetNode),
							std::addressof(a_data.targetNode));

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

					DrawTransformSliders(
						static_cast<Data::configTransform_t&>(a_data),
						[&](auto a_v) {
							switch (a_v)
							{
							case TransformUpdateValue::Position:
								PropagateToEquipmentOverrides(
									a_baseConfig,
									offsetof(Data::configBaseValues_t, position),
									std::addressof(a_data.position));
								break;
							case TransformUpdateValue::Rotation:
								PropagateToEquipmentOverrides(
									a_baseConfig,
									offsetof(Data::configBaseValues_t, rotation),
									std::addressof(a_data.rotation));
								break;
							case TransformUpdateValue::Scale:
								PropagateToEquipmentOverrides(
									a_baseConfig,
									offsetof(Data::configBaseValues_t, scale),
									std::addressof(a_data.scale));
								break;
							}

							OnBaseConfigChange(
								a_handle,
								a_params,
								PostChangeAction::UpdateTransform);
						});

					UICommon::PopDisabled(disabled);

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}
			ImGui::PopID();
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawExtraFlags(
			T a_handle,
			Data::configBaseValues_t& a_data,
			Data::configBase_t* a_baseConfig,
			const void* a_params)
		{
		}

		template <class T>
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawEquipmentOverrideTreeContextMenu(
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params)
		{
			BaseConfigEditorAction result{
				BaseConfigEditorAction::None
			};

			ImGui::PushID("ao_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
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

						ImGui::CloseCurrentPopup();

						result = BaseConfigEditorAction::Insert;
					}

					ImGui::EndMenu();
				}

				auto clipData = UIClipboard::Get<Data::equipmentOverride_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "2"),
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

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrideList(
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params,
			const stl::fixed_string& a_name)
		{
			if (a_data.equipmentOverrides.empty())
			{
				return;
			}

			ImGui::PushID("equipment_override_list");

			std::size_t i = 0;

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
					it = a_data.equipmentOverrides.emplace(it, a_data, *result.desc);
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
					it->description = *result.desc;
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

						/*auto disabled = !e.eoFlags.test_any(Data::EquipmentOverrideFlags::kConditioningEnabled);

						UICommon::PushDisabled(disabled);*/

						const auto result = DrawEquipmentOverrideEntryConditionHeaderContextMenu(
							a_handle,
							e.conditions,
							a_params);

						bool empty = e.conditions.empty();

						if (!empty)
						{
							if (result == BaseConfigEditorAction::PasteOver ||
							    result == BaseConfigEditorAction::Insert)
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
									a_params,
									false);
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

						//UICommon::PopDisabled(disabled);

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
		BaseConfigEditorAction UIBaseConfigWidget<T>::DrawEquipmentOverrideEntryConditionHeaderContextMenu(
			T a_handle,
			Data::equipmentOverrideConditionList_t& a_entry,
			const void* a_params)
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

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}
						break;
					case Data::EquipmentOverrideConditionType::Form:
					case Data::EquipmentOverrideConditionType::Race:
					case Data::EquipmentOverrideConditionType::Keyword:
					case Data::EquipmentOverrideConditionType::Quest:
						if (result.form)
						{
							a_entry.emplace_back(
								result.entryType,
								result.form);

							action = result.action;

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}
						break;
					case Data::EquipmentOverrideConditionType::Furniture:
					case Data::EquipmentOverrideConditionType::Group:

						a_entry.emplace_back(
							result.entryType);

						action = result.action;

						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);

						break;
					case Data::EquipmentOverrideConditionType::BipedSlot:
						if (result.biped != Biped::BIPED_OBJECT::kNone)
						{
							a_entry.emplace_back(
								result.biped);

							action = result.action;

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
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

					OnBaseConfigChange(
						a_handle,
						a_params,
						PostChangeAction::Evaluate);

					action = BaseConfigEditorAction::PasteOver;
				}
				break;
			case BaseConfigEditorAction::Delete:
				a_entry.clear();

				action = BaseConfigEditorAction::Delete;

				OnBaseConfigChange(
					a_handle,
					a_params,
					PostChangeAction::Evaluate);
				break;
			}

			return action;
		}

		template <class T>
		void UIBaseConfigWidget<T>::DrawEquipmentOverrideEntryConditionTable(
			T a_handle,
			Data::configBase_t& a_baseData,
			Data::equipmentOverrideConditionList_t& a_entry,
			const void* a_params,
			bool a_isnested)
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
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4.0f);
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
						OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						break;
					case BaseConfigEditorAction::ClearKeyword:
						if (it->keyword.get_id())
						{
							it->keyword = 0;
							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
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

								OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
							}
							break;
						case Data::EquipmentOverrideConditionType::Form:
						case Data::EquipmentOverrideConditionType::Race:
						case Data::EquipmentOverrideConditionType::Keyword:
						case Data::EquipmentOverrideConditionType::Quest:
							if (result.form)
							{
								it = a_entry.emplace(
									it,
									result.entryType,
									result.form);

								OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
							}
							break;
						case Data::EquipmentOverrideConditionType::Furniture:
						case Data::EquipmentOverrideConditionType::Group:

							it = a_entry.emplace(
								it,
								result.entryType);

							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);

							break;
						case Data::EquipmentOverrideConditionType::BipedSlot:
							if (result.biped != Biped::BIPED_OBJECT::kNone)
							{
								it = a_entry.emplace(
									it,
									result.biped);

								OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
							}
							break;
						}

						break;

					case BaseConfigEditorAction::Swap:

						if (IterSwap(a_entry, it, result.dir))
						{
							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
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

							const auto result = DrawEquipmentOverrideEntryConditionHeaderContextMenu(
								a_handle,
								e.group.conditions,
								a_params);

							DrawEquipmentOverrideEntryConditionTable(
								a_handle,
								a_baseData,
								e.group.conditions,
								a_params,
								true);

							ImGui::PopID();
						}
						else
						{
							m_matchParamEditor.Reset();

							UpdateMatchParamAllowedTypes(e.fbf.type);

							const char* tdesc;
							const char* vdesc;

							switch (e.fbf.type)
							{
							case Data::EquipmentOverrideConditionType::Form:

								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.form);
								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Form);

								break;
							case Data::EquipmentOverrideConditionType::Type:

								m_matchParamEditor.SetNext<ConditionParamItem::EquipmentSlotExtra>(
									e.slot);
								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.form);
								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlotExtra);
								tdesc = LS(CommonStrings::Type);

								break;
							case Data::EquipmentOverrideConditionType::Keyword:

								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = LS(CommonStrings::Keyword);
								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Keyword);

								break;
							case Data::EquipmentOverrideConditionType::Race:

								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.form);
								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Race);

								break;
							case Data::EquipmentOverrideConditionType::Furniture:

								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.form);
								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Furniture);
								tdesc = LS(CommonStrings::Furniture);

								break;
							case Data::EquipmentOverrideConditionType::BipedSlot:

								m_matchParamEditor.SetNext<ConditionParamItem::BipedSlot>(
									e.bipedSlot);
								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.form);
								m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::BipedSlot);
								tdesc = LS(CommonStrings::Biped);

								break;

							case Data::EquipmentOverrideConditionType::Quest:

								m_matchParamEditor.SetNext<ConditionParamItem::Form>(
									e.keyword.get_id());
								m_matchParamEditor.SetNext<ConditionParamItem::QuestCondType>(
									e.questCondType);

								vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Quest);

								break;
							default:
								tdesc = nullptr;
								vdesc = nullptr;
								break;
							}

							if (!vdesc)
							{
								vdesc = "N/A";
							}

							ImGui::Text("%s", tdesc);

							ImGui::TableSetColumnIndex(2);

							bool result = ImGui::Selectable(
								LMKID<2>(vdesc, "sel_ctl"),
								false,
								ImGuiSelectableFlags_DontClosePopups);

							UICommon::ToolTip(vdesc);

							if (result)
							{
								m_matchParamEditor.OpenConditionParamEditorPopup();
							}

							if (m_matchParamEditor.DrawConditionParamEditorPopup())
							{
								OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

						ImGui::TableSetColumnIndex(3);

						if (ImGui::CheckboxFlagsT(
								"##oper_and",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kAnd)))
						{
							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
						}

						ImGui::TableSetColumnIndex(4);

						if (ImGui::CheckboxFlagsT(
								"##oper_not",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kNot)))
						{
							OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
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

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				m_aoNewEntryID = {};
				m_aoNewEntryKWID = {};
				m_aoNewEntryRaceID = {};
				m_ooNewBiped = Biped::BIPED_OBJECT::kNone;
				m_aoNewSlot = Data::ObjectSlotExtra::kNone;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (!a_header)
			{
				if (ImGui::ArrowButton("up", ImGuiDir_Up))
				{
					result.action = BaseConfigEditorAction::Swap;
					result.dir = SwapDirection::Up;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("down", ImGuiDir_Down))
				{
					result.action = BaseConfigEditorAction::Swap;
					result.dir = SwapDirection::Down;
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
							result.action = BaseConfigEditorAction::Insert;
							result.slot = m_aoNewSlot;
							result.entryType = Data::EquipmentOverrideConditionType::Type;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Form, "3"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Form);

						if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
								LS(CommonStrings::Form, "fs"),
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action = BaseConfigEditorAction::Insert;
								result.form = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Form;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Keyword, "4"))
					{
						if (m_matchParamEditor.GetKeywordPicker().DrawFormSelector(
								LS(CommonStrings::Keyword, "fs"),
								m_aoNewEntryKWID))
						{
							if (m_aoNewEntryKWID)
							{
								result.action = BaseConfigEditorAction::Insert;
								result.form = m_aoNewEntryKWID;
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
							if (m_ooNewBiped != Biped::BIPED_OBJECT::kNone)
							{
								result.action = BaseConfigEditorAction::Insert;
								result.biped = m_ooNewBiped;
								result.entryType = Data::EquipmentOverrideConditionType::BipedSlot;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Race, "6"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Race);

						if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
								LS(CommonStrings::Race, "fs"),
								m_aoNewEntryRaceID))
						{
							if (m_aoNewEntryRaceID)
							{
								result.action = BaseConfigEditorAction::Insert;
								result.form = m_aoNewEntryRaceID;
								result.entryType = Data::EquipmentOverrideConditionType::Race;

								ImGui::CloseCurrentPopup();
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Furniture, "7"))
					{
						result.action = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Furniture;

						ImGui::CloseCurrentPopup();
					}

					if (LCG_BM(CommonStrings::Quest, "8"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Quest);

						if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
								LS(CommonStrings::Form, "fs"),
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action = BaseConfigEditorAction::Insert;
								result.form = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Quest;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Group, "9"))
					{
						result.action = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Group;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(CommonStrings::Delete, "A"))
				{
					result.action = BaseConfigEditorAction::Delete;
				}

				if (!a_header)
				{
					if (LCG_MI(UIWidgetCommonStrings::ClearKeyword, "B"))
					{
						result.action = BaseConfigEditorAction::ClearKeyword;
					}
				}
				else
				{
					ImGui::Separator();

					if (LCG_MI(CommonStrings::Copy, "B"))
					{
						result.action = BaseConfigEditorAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::PasteOver, "C"),
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
			T a_handle,
			Data::configBase_t& a_data,
			const void* a_params,
			Data::equipmentOverride_t& a_override)
		{
		}

		template <class T>
		auto UIBaseConfigWidget<T>::DrawEquipmentOverrideContextMenu(
			T a_handle,
			const void* a_params,
			Data::equipmentOverride_t& a_data)
			-> EquipmentOverrideResult
		{
			EquipmentOverrideResult result;

			ImGui::PushID("eo_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
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
				result.dir = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = BaseConfigEditorAction::Swap;
				result.dir = SwapDirection::Down;
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
							result.desc = std::addressof(GetDescriptionPopupBuffer());

							ImGui::CloseCurrentPopup();
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
						result.desc = std::addressof(GetDescriptionPopupBuffer());

						ImGui::CloseCurrentPopup();
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

					if (auto clipData = UIClipboard::Get<Data::equipmentOverride_t>())
					{
						a_data = *clipData;

						update = true;
					}
					else if (auto clipData = UIClipboard::Get<Data::configBaseValues_t>())
					{
						static_cast<Data::configBaseValues_t&>(a_data) = *clipData;

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

				break;

			case Data::EquipmentOverrideConditionType::Furniture:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::LayingDown, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kLayingDown));

				break;

			case Data::EquipmentOverrideConditionType::BipedSlot:

				result |= ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorStrings::MatchSkin, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchSkin));

				DrawTip(UITip::MatchSkin);

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		bool UIBaseConfigWidget<T>::DrawConditionItemExtra(
			ConditionParamItem a_item,
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

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					if (match->fbf.type == Data::EquipmentOverrideConditionType::BipedSlot &&
					    match->flags.test(Data::EquipmentOverrideConditionFlags::kMatchSkin))
					{
						a_args.disable = true;
					}

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Data::EquipmentOverrideConditionType::Form:
			case Data::EquipmentOverrideConditionType::Race:

				if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

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
			case Data::EquipmentOverrideConditionType::Furniture:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.furniture);
				break;
			case Data::EquipmentOverrideConditionType::Race:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.race);
				break;
			case Data::EquipmentOverrideConditionType::Quest:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.quest);
				break;
			default:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.form_common);
				break;
			}
		}

	}
}