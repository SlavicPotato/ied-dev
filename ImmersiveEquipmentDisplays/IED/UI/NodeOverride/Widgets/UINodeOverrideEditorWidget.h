#pragma once

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UISettingsInterface.h"

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#include "IED/UI/Widgets/Form/UIFormSelectorWidget.h"
#include "IED/UI/Widgets/UIBipedObjectSelectorWidget.h"
#include "IED/UI/Widgets/UICMNodeSelector.h"
#include "IED/UI/Widgets/UIConditionParamEditorWidget.h"
#include "IED/UI/Widgets/UICurrentData.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"
#include "IED/UI/Widgets/UIEditorPanelSettings.h"
#include "IED/UI/Widgets/UIObjectTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPhysicsValueEditorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UITransformSliderWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "IED/NodeOverrideData.h"

#include "UINodeOverrideConditionWidget.h"
#include "UINodeOverrideEditorWidgetStrings.h"
#include "UINodeOverrideWidgetCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{

		enum class NodeOverrideEditorFlags : Data::SettingHolder::EditorPanelCommonFlagsType
		{
			kNone = 0,

			kDrawNodePlacement         = 1u << 0,
			kUnrestrictedNodePlacement = 1u << 1,
			kDrawPhysics               = 1u << 2,

			kPanelMask =
				kDrawNodePlacement |
				kDrawPhysics
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideEditorFlags);

		struct NodeOverridePlacementOverrideResult
		{
			NodeOverrideCommonAction action{ NodeOverrideCommonAction::None };
			std::string              desc;
			SwapDirection            dir{ SwapDirection::None };
		};

		namespace detail
		{
			template <class T, class Enable = void>
			struct value_type_selector
			{
				using type = void;
			};

			template <std::same_as<Data::configNodeOverridePlacementOverride_t> T>
			struct value_type_selector<T>
			{
				using type = Data::configNodeOverridePlacementValues_t;
			};

			template <std::same_as<Data::configNodeOverridePlacement_t> T>
			struct value_type_selector<T>
			{
				using type = Data::configNodeOverridePlacementValues_t;
			};

			template <std::same_as<Data::configNodeOverridePhysicsOverride_t> T>
			struct value_type_selector<T>
			{
				using type = Data::configNodePhysicsValues_t;
			};

			template <std::same_as<Data::configNodeOverridePhysics_t> T>
			struct value_type_selector<T>
			{
				using type = Data::configNodePhysicsValues_t;
			};
		}

		template <class T>
		class UINodeOverrideEditorWidget :
			UINodeOverrideConditionWidget,
			UIPhysicsValueEditorWidget,
			public UICollapsibles,
			public UIFormLookupInterface,
			public UIEditorPanelSettings,
			public virtual UISettingsInterface,
			public virtual UITransformSliderWidget,
			public UIEditorInterface
		{
		public:
			UINodeOverrideEditorWidget(
				Controller& a_controller);

			void DrawNodeOverrideEditor(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

		protected:
			using NodeOverrideEditorCurrentData = UICurrentData<T, entryNodeOverrideData_t>;

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

			virtual void EditorDrawMenuBarItems() override;

		private:
			enum class PanelType
			{
				Transform,
				Placement,
				Physics
			};

			static PanelType GetPanelTypeFromFlags(stl::flag<NodeOverrideEditorFlags> a_flags);

			void QueueClearAllPopup(const NodeOverrideEditorCurrentData& a_data);
			void QueuePasteOverPopup(
				const NodeOverrideEditorCurrentData&                 a_data,
				const Data::configNodeOverrideHolderClipboardData_t& a_clipData);

			virtual void DrawMainHeaderControlsExtra(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

			virtual bool DrawExtraInfoAndWarnings(
				T                              a_handle,
				const entryNodeOverrideData_t& a_data) const;

			virtual void DrawExtraEditorPanelSettings(const void* a_params) override;

			void DrawItemFilter();

			void DrawConfigClassHeader(Data::ConfigClass a_class);

			void DrawTransformPanel(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

			void DrawPlacementPanel(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

			template <class Td>
			void DrawCMNodeListPanel(
				T                                     a_handle,
				const NodeOverrideData::cm_data_type& a_cmdata,
				entryNodeOverrideData_t&              a_data);

			void DrawEntry(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawEntry(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawEntry(
				T                                            a_handle,
				entryNodeOverrideData_t&                     a_data,
				const SingleNodeOverridePhysicsUpdateParams& a_params,
				const bool                                   a_exists);

			void DrawValues(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverridePlacementValues_t&     a_values,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawValues(
				T                                            a_handle,
				entryNodeOverrideData_t&                     a_data,
				Data::configNodePhysicsValues_t&             a_values,
				const SingleNodeOverridePhysicsUpdateParams& a_params,
				const bool                                   a_exists);

			void DrawValueFlags(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideTransformValues_t&     a_values,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			NodeOverrideCommonAction DrawOffsetListContextMenu(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			template <class Ta, class Tb>
			NodeOverrideCommonAction DrawOverrideListContextMenu(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				Ta&                      a_entry,
				const Tb&                a_params,
				const bool               a_exists);

			void DrawOffsets(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists,
				Data::configNodeOverrideOffset_t*              a_parent);

			template <class Ta, class Tb>
			void DrawOverrides(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				Ta&                      a_entry,
				const Tb&                a_params,
				const bool               a_exists);

			void DrawOffsetList(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			template <class Ta, class Tb>
			void DrawOverrideList(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				Ta&                      a_entry,
				const Tb&                a_params,
				const bool               a_exists);

			NodeOverrideCommonResult DrawOffsetContextMenu(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffset_t&              a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			template <class Ta, class Tb>
			NodeOverridePlacementOverrideResult DrawOverrideContextMenu(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				Ta&                      a_entry,
				const Tb&                a_params,
				const bool               a_exists);

			void DrawOffsetAdjust(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffset_t&              a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			template <class Td>
			typename Td::iterator DrawCommonEntryContextMenu(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				const stl::fixed_string& a_name,
				typename Td::iterator    a_it);

			/*void DrawOverridePosClamp(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffset_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);*/

			virtual void OnUpdate(
				T                                              a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T                                              a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T                                            a_handle,
				const SingleNodeOverridePhysicsUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T                               a_handle,
				const NodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearTransform(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearPlacement(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearPhysics(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearAllTransforms(
				T                                       a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearAllPlacement(
				T                                       a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearAllPhysics(
				T                                       a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) = 0;

			template <class Td>
			void HandleValueUpdate(
				T                        a_handle,
				entryNodeOverrideData_t& a_data,
				const Td&                a_params,
				const bool               a_exists);

			template <class Td>
			void DoOnClear(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params);

			virtual NodeOverrideEditorCurrentData GetCurrentData() = 0;

			virtual UIPopupQueue& GetPopupQueue() = 0;

			UIGenericFilter m_itemFilter;
		};

		template <class T>
		UINodeOverrideEditorWidget<T>::UINodeOverrideEditorWidget(
			Controller& a_controller) :
			UIFormLookupInterface(a_controller),
			UINodeOverrideConditionWidget(a_controller),
			m_itemFilter(true)
		{
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawNodeOverrideEditor(
			T                        a_handle,
			entryNodeOverrideData_t& a_data)
		{
			ImGui::PushID("et_editor_widget");

			DrawEditorPanelSettings();

			ImGui::Spacing();

			ImGui::PushID("extra_header");

			DrawMainHeaderControlsExtra(a_handle, a_data);

			ImGui::PopID();

			ImGui::Separator();

			DrawItemFilter();

			ImGui::Separator();

			if (DrawExtraInfoAndWarnings(a_handle, a_data))
			{
				ImGui::Separator();
			}

			ImGui::Spacing();

			const auto flags     = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();
			const auto panelType = GetPanelTypeFromFlags(flags.value);

			switch (panelType)
			{
			case PanelType::Physics:

				if (ImGui::BeginChild(
						"ph_editor_panel",
						{ -1.0f, 0.0f }))
				{
					if (!GetPhysicsProcessingEnabled())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped("%s", UIL::LS(UITip::PhysicsOffWarning));
						ImGui::PopStyleColor();

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
					}

					ImGui::PushItemWidth(ImGui::GetFontSize() * -11.f);

					DrawCMNodeListPanel<
						entryNodeOverrideData_t::physics_data_type>(
						a_handle,
						NodeOverrideData::GetMOVNodeData(),
						a_data);

					ImGui::PopItemWidth();
				}

				ImGui::EndChild();

				break;
			case PanelType::Placement:

				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
				ImGui::TextWrapped("%s", UIL::LS(UITip::AnimSupportWarning));
				ImGui::PopStyleColor();

				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::BeginChild(
						"ep_editor_panel",
						{ -1.0f, 0.0f }))
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * -11.f);

					DrawPlacementPanel(a_handle, a_data);

					ImGui::PopItemWidth();
				}

				ImGui::EndChild();

				break;
			default:

				if (ImGui::BeginChild(
						"et_editor_panel",
						{ -1.0f, 0.0f },
						false,
						ImGuiWindowFlags_HorizontalScrollbar))
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * 27.f);

					DrawCMNodeListPanel<
						entryNodeOverrideData_t::transform_data_type>(
						a_handle,
						NodeOverrideData::GetCMENodeData(),
						a_data);

					ImGui::PopItemWidth();
				}

				ImGui::EndChild();

				break;
			}

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawMainHeaderControlsExtra(
			T                        a_handle,
			entryNodeOverrideData_t& a_data)
		{
		}

		template <class T>
		bool UINodeOverrideEditorWidget<T>::DrawExtraInfoAndWarnings(
			T                              a_handle,
			const entryNodeOverrideData_t& a_data) const
		{
			return false;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawExtraEditorPanelSettings(const void* a_params)
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			auto flags     = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();
			auto panelType = GetPanelTypeFromFlags(flags.value);

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Positions, "1"),
					panelType == PanelType::Transform))
			{
				flags.clear(NodeOverrideEditorFlags::kPanelMask);
				OnEditorPanelSettingsChange();
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Placement, "2"),
					panelType == PanelType::Placement))
			{
				flags.clear(NodeOverrideEditorFlags::kPanelMask);
				flags.set(NodeOverrideEditorFlags::kDrawNodePlacement);
				OnEditorPanelSettingsChange();
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Physics, "3"),
					panelType == PanelType::Physics))
			{
				flags.clear(NodeOverrideEditorFlags::kPanelMask);
				flags.set(NodeOverrideEditorFlags::kDrawPhysics);
				OnEditorPanelSettingsChange();
			}

			panelType = GetPanelTypeFromFlags(flags.value);

			switch (panelType)
			{
			case PanelType::Placement:

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::UnrestrictedPlacement, "A"),
						stl::underlying(std::addressof(flags.value)),
						stl::underlying(NodeOverrideEditorFlags::kUnrestrictedNodePlacement)))
				{
					OnEditorPanelSettingsChange();
				}

				break;
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawItemFilter()
		{
			if (TreeEx(
					"item_filter",
					false,
					"%s",
					UIL::LS(CommonStrings::Filter)))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				ImGui::Spacing();

				m_itemFilter.Draw();

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawConfigClassHeader(
			Data::ConfigClass a_class)
		{
			ImGui::Text("%s:", UIL::LS(UIWidgetCommonStrings::ConfigInUse));
			ImGui::SameLine();
			DrawConfigClassInUse(a_class);
			ImGui::Spacing();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawTransformPanel(
			T                        a_handle,
			entryNodeOverrideData_t& a_data)
		{
			auto& data = NodeOverrideData::GetCMENodeData();

			const auto configClass = GetConfigClass();

			auto itb = data.getvec().begin();

			for (const auto& e : data.getvec())
			{
				if (!m_itemFilter.Test(*e->second.desc))
				{
					continue;
				}

				ImGui::PushID(e->first.c_str());

				auto it = a_data.transformData.find(e->first);

				bool svar = it == a_data.transformData.end();

				if (svar)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
				}
				else
				{
					if (configClass != Data::ConfigClass::Global)
					{
						svar = configClass != it->second.first;

						if (svar)
						{
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
						}
					}
				}

				it = DrawCommonEntryContextMenu<
					entryNodeOverrideData_t::transform_data_type>(
					a_handle,
					a_data,
					e->first,
					it);

				if (TreeEx(
						"entry_tree",
						e->first == (*itb)->first,
						"%s",
						e->second.desc.c_str()))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.transformData.end())
					{
						if (configClass != Data::ConfigClass::Global)
						{
							DrawConfigClassHeader(it->second.first);
						}

						DrawEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						decltype(it->second) tmp;

						DrawEntry(a_handle, a_data, { GetSex(), e->first, tmp }, false);
					}

					ImGui::Unindent();

					ImGui::TreePop();
				}

				if (svar)
				{
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawPlacementPanel(
			T                        a_handle,
			entryNodeOverrideData_t& a_data)
		{
			bool first = true;

			Data::configStoreNodeOverride_t::holderCache_t hc;

			const auto configClass = GetConfigClass();

			auto& data = NodeOverrideData::GetWeaponNodeData();

			for (auto& e : data.getvec())
			{
				if (!m_itemFilter.Test(e->second.desc))
				{
					continue;
				}

				ImGui::PushID(e->first.c_str());

				auto it = a_data.placementData.find(e->first);

				bool svar = it == a_data.placementData.end();

				if (svar)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
				}
				else
				{
					if (configClass != Data::ConfigClass::Global)
					{
						svar = configClass != it->second.first;

						if (svar)
						{
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
						}
					}
				}

				it = DrawCommonEntryContextMenu<
					entryNodeOverrideData_t::placement_data_type>(
					a_handle,
					a_data,
					e->first,
					it);

				if (TreeEx(
						"entry_tree",
						true,
						"%s",
						e->second.desc))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.placementData.end())
					{
						if (configClass != Data::ConfigClass::Global)
						{
							DrawConfigClassHeader(it->second.first);
						}

						DrawEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						stl::strip_type<decltype(it->second)> tmp;

						DrawEntry(a_handle, a_data, { GetSex(), e->first, tmp }, false);
					}

					ImGui::Unindent();

					ImGui::TreePop();
				}

				if (svar)
				{
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();

				ImGui::PopID();

				first = false;
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawEntry(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			auto& data = a_params.entry.second(a_params.sex);

			DrawTransformTree(
				data.transform,
				false,
				[&](auto a_v) {
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
				},
				[&] {
					ImGui::Spacing();

					DrawValueFlags(
						a_handle,
						a_data,
						data,
						a_params,
						a_exists);
				});

			if (TreeEx(
					"tree2",
					true,
					"%s",
					UIL::LS(CommonStrings::Visibility)))
			{
				ImGui::Spacing();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Invisible, "ctl_1"),
						stl::underlying(std::addressof(data.overrideFlags.value)),
						stl::underlying(Data::NodeOverrideFlags::kInvisible)))
				{
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
				}

				bool disabled = !data.overrideFlags.test(Data::NodeOverrideFlags::kInvisible);

				UICommon::PushDisabled(disabled);

				if (ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::RequiresMatchList, "ctl_2"),
						stl::underlying(std::addressof(data.overrideFlags.value)),
						stl::underlying(Data::NodeOverrideFlags::kVisibilityRequiresConditionList)))
				{
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
				}

				ImGui::Spacing();

				DrawConditionTree(
					a_params.name,
					GetNodeOverrideDataTypeFromParams<SingleNodeOverrideTransformUpdateParams>(),
					a_data,
					data.visibilityConditionList,
					a_exists,
					true,
					[&] {
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					});

				UICommon::PopDisabled(disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			DrawOffsets(
				a_handle,
				a_data,
				data.offsets,
				a_params,
				a_exists,
				nullptr);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawEntry(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			const SingleNodeOverridePlacementUpdateParams& a_params,
			const bool                                     a_exists)
		{
			auto& data = a_params.entry.second(a_params.sex);

			ImGui::PushID("pl_entry");

			DrawValues(
				a_handle,
				a_data,
				data,
				a_params,
				a_exists);

			ImGui::Spacing();

			DrawOverrides(
				a_handle,
				a_data,
				data.overrides,
				a_params,
				a_exists);

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawEntry(
			T                                            a_handle,
			entryNodeOverrideData_t&                     a_data,
			const SingleNodeOverridePhysicsUpdateParams& a_params,
			const bool                                   a_exists)
		{
			auto& data = a_params.entry.second(a_params.sex);

			ImGui::PushID("phy_entry");

			DrawValues(
				a_handle,
				a_data,
				data,
				a_params,
				a_exists);

			ImGui::Spacing();

			DrawOverrides(
				a_handle,
				a_data,
				data.overrides,
				a_params,
				a_exists);

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawValues(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverridePlacementValues_t&     a_values,
			const SingleNodeOverridePlacementUpdateParams& a_params,
			const bool                                     a_exists)
		{
			const auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			const NodeOverrideData::weaponNodeEntry_t* entry;

			if (!flags.test(NodeOverrideEditorFlags::kUnrestrictedNodePlacement))
			{
				auto& nodedata = NodeOverrideData::GetWeaponNodeData();

				auto it = nodedata.find(a_params.name);
				if (it == nodedata.end())
				{
					return;
				}

				entry = std::addressof(it->second);
			}
			else
			{
				entry = nullptr;
			}

			ImGui::PushID("pl_vals");

			ImGui::BeginGroup();

			if (ImGui::Button(UIL::LS(CommonStrings::Clear, "ctl_1")))
			{
				a_values.targetNode.clear();

				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::SameLine();

			bool result;

			if (entry)
			{
				result = UICMNodeSelectorWidget::DrawCMNodeSelector(
					UIL::LS(UIWidgetCommonStrings::TargetNode, "node_sel"),
					a_values.targetNode,
					entry->movs,
					nullptr,
					std::addressof(NodeOverrideData::GetMOVNodeData()));
			}
			else
			{
				result = UICMNodeSelectorWidget::DrawCMNodeSelector(
					UIL::LS(UIWidgetCommonStrings::TargetNode, "node_sel"),
					a_values.targetNode,
					NodeOverrideData::GetMOVNodeData());
			}

			if (result)
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::EndGroup();

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawValues(
			T                                            a_handle,
			entryNodeOverrideData_t&                     a_data,
			Data::configNodePhysicsValues_t&             a_values,
			const SingleNodeOverridePhysicsUpdateParams& a_params,
			const bool                                   a_exists)
		{
			ImGui::PushID("phy_vals");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -14.5f);

			if (DrawPhysicsValues(a_values))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::PopItemWidth();

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOffsets(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffsetList_t&          a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists,
			Data::configNodeOverrideOffset_t*              a_parent)
		{
			const auto result = DrawOffsetListContextMenu(
				a_handle,
				a_data,
				a_entry,
				a_params,
				a_exists);

			const bool empty = a_entry.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result == NodeOverrideCommonAction::Insert ||
				    result == NodeOverrideCommonAction::PasteOver)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			if (ImGui::TreeNodeEx(
					"offsets",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Offsets)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOffsetList(
						a_handle,
						a_data,
						a_entry,
						a_params,
						a_exists);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);
		}

		template <class T>
		template <class Ta, class Tb>
		void UINodeOverrideEditorWidget<T>::DrawOverrides(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			Ta&                      a_entry,
			const Tb&                a_params,
			const bool               a_exists)
		{
			const auto result = DrawOverrideListContextMenu(
				a_handle,
				a_data,
				a_entry,
				a_params,
				a_exists);

			const bool empty = a_entry.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result == NodeOverrideCommonAction::Insert ||
				    result == NodeOverrideCommonAction::PasteOver)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			if (TreeEx(
					"tree_ovr",
					true,
					"%s",
					UIL::LS(CommonStrings::Overrides)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOverrideList(a_handle, a_data, a_entry, a_params, a_exists);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawValueFlags(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideTransformValues_t&     a_values,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			ImGui::PushID("value_flags");

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UINodeOverrideEditorWidgetStrings::AbsolutePosition, "1"),
					stl::underlying(std::addressof(a_values.flags.value)),
					stl::underlying(Data::NodeOverrideValuesFlags::kAbsolutePosition)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::PopID();
		}

		template <class T>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOffsetListContextMenu(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffsetList_t&          a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("list_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::Item, "2"))
					{
						if (DrawDescriptionPopup())
						{
							a_entry.emplace_back().description = GetDescriptionPopupBuffer();

							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);

							ClearDescriptionPopupBuffer();

							result = NodeOverrideCommonAction::Insert;
						}
						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Group, "3"))
					{
						if (DrawDescriptionPopup())
						{
							auto& e       = a_entry.emplace_back();
							e.description = GetDescriptionPopupBuffer();
							e.offsetFlags.set(Data::NodeOverrideOffsetFlags::kIsGroup);

							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);

							ClearDescriptionPopupBuffer();

							result = NodeOverrideCommonAction::Insert;
						}
						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "4")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipDataOffset = UIClipboard::Get<Data::configNodeOverrideOffset_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "5"),
						nullptr,
						false,
						clipDataOffset != nullptr))
				{
					if (clipDataOffset)
					{
						a_entry.emplace_back(*clipDataOffset);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideOffsetList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "6"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_entry = *clipData;

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class T>
		template <class Ta, class Tb>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverrideListContextMenu(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			Ta&                      a_entry,
			const Tb&                a_params,
			const bool               a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_entry.emplace_back().description = GetDescriptionPopupBuffer();

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						ClearDescriptionPopupBuffer();

						result = NodeOverrideCommonAction::Insert;
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipData1 = UIClipboard::Get<typename Ta::value_type>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "B"),
						nullptr,
						false,
						clipData1 != nullptr))
				{
					if (clipData1)
					{
						a_entry.emplace_back(*clipData1);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}
				}

				auto clipData = UIClipboard::Get<Ta>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "C"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_entry = *clipData;

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOffsetList(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffsetList_t&          a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			if (a_entry.empty())
			{
				return;
			}

			ImGui::PushID("offset_list");

			int i = 0;

			auto it = a_entry.begin();

			while (it != a_entry.end())
			{
				ImGui::PushID(i);

				const auto result = DrawOffsetContextMenu(a_handle, a_data, *it, a_params, a_exists);

				switch (result.action)
				{
				case NodeOverrideCommonAction::Delete:

					it = a_entry.erase(it);
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);

					break;
				case NodeOverrideCommonAction::Insert:

					it              = a_entry.emplace(it);
					it->description = result.desc;
					if (result.isGroup)
					{
						it->offsetFlags.set(Data::NodeOverrideOffsetFlags::kIsGroup);
					}
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
					ImGui::SetNextItemOpen(true);

					break;
				case NodeOverrideCommonAction::Swap:

					if (IterSwap(a_entry, it, result.dir))
					{
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}

					break;
				case NodeOverrideCommonAction::Reset:

					break;
				case NodeOverrideCommonAction::Rename:

					it->description = result.desc;
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);

					break;
				case NodeOverrideCommonAction::Paste:

					if (auto clipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>())
					{
						it = a_entry.emplace(it, *clipData);
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}
					[[fallthrough]];
				case NodeOverrideCommonAction::PasteOver:
					ImGui::SetNextItemOpen(true);
					break;
				}

				if (it != a_entry.end())
				{
					auto& e = *it;

					/*if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kIsGroup))
					{
						ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
						ImGui::TextUnformatted("[G]");
					}*/

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::TreeNodeEx(
							"oo_item",
							ImGuiTreeNodeFlags_SpanAvailWidth |
								ImGuiTreeNodeFlags_DefaultOpen,
							"%s",
							e.description.c_str()))
					{
						ImGui::Spacing();

						DrawConditionTree(
							a_params.name,
							GetNodeOverrideDataTypeFromParams<SingleNodeOverrideTransformUpdateParams>(),
							a_data,
							e.conditions,
							a_exists,
							false,
							[&] {
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							});

						if (ImGui::CheckboxFlagsT(
								UIL::LS(CommonStrings::Continue, "hctl_1"),
								stl::underlying(std::addressof(e.offsetFlags.value)),
								stl::underlying(Data::NodeOverrideOffsetFlags::kContinue)))
						{
							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);
						}

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UIWidgetCommonStrings::RequiresMatchList, "hctl_2"),
								stl::underlying(std::addressof(e.offsetFlags.value)),
								stl::underlying(Data::NodeOverrideOffsetFlags::kRequiresConditionList)))
						{
							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);
						}

						if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kIsGroup))
						{
							ImGui::PushID("group");

							DrawOffsets(
								a_handle,
								a_data,
								e.group,
								a_params,
								a_exists,
								std::addressof(e));

							ImGui::PopID();
						}
						else
						{
							ImGui::PushID("override");

							if (ImGui::CheckboxFlagsT(
									UIL::LS(UINodeOverrideEditorWidgetStrings::WeaponAdjust, "3"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kWeaponAdjust)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							ImGui::SameLine(0.0f, 10.0f);

							if (ImGui::CheckboxFlagsT(
									UIL::LS(UINodeOverrideEditorWidgetStrings::WeightAdjust, "4"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kWeightAdjust)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							if (e.offsetFlags.test_any(Data::NodeOverrideOffsetFlags::kWeaponAdjust))
							{
								ImGui::Indent();

								if (ImGui::CheckboxFlagsT(
										UIL::LS(UINodeOverrideEditorWidgetStrings::UseAdjustsFromMatched, "5"),
										stl::underlying(std::addressof(e.offsetFlags.value)),
										stl::underlying(Data::NodeOverrideOffsetFlags::kWeaponAdjustMatchedOnly)))
								{
									HandleValueUpdate(
										a_handle,
										a_data,
										a_params,
										a_exists);
								}

								ImGui::Unindent();
							}

							if (ImGui::CheckboxFlagsT(
									UIL::LS(UINodeOverrideEditorWidgetStrings::AccumulatePosition, "6"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kAccumulatePos)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							bool disabled = e.offsetFlags.test_any(Data::NodeOverrideOffsetFlags::kAdjustFlags);

							UICommon::PushDisabled(disabled);

							if (ImGui::CheckboxFlagsT(
									UIL::LS(UINodeOverrideEditorWidgetStrings::LockToAccum, "7"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kLockToAccum)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							UICommon::PopDisabled(disabled);

							ImGui::SameLine(0.0f, 10.0f);

							if (ImGui::CheckboxFlagsT(
									UIL::LS(CommonStrings::Reset, "8"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kResetAccum)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							ImGui::Spacing();

							if (e.offsetFlags.test_any(Data::NodeOverrideOffsetFlags::kAdjustFlags))
							{
								if (ImGui::TreeNodeEx(
										"atree",
										ImGuiTreeNodeFlags_SpanAvailWidth |
											ImGuiTreeNodeFlags_DefaultOpen,
										"%s",
										UIL::LS(CommonStrings::Adjust)))
								{
									ImGui::Spacing();

									DrawValueFlags(a_handle, a_data, e, a_params, a_exists);

									ImGui::Spacing();

									DrawOffsetAdjust(
										a_handle,
										a_data,
										e,
										a_params,
										a_exists);

									ImGui::Spacing();

									ImGui::TreePop();
								}
							}
							else
							{
								ImGui::Spacing();

								DrawTransformTree(
									e.transform,
									false,
									[&](auto a_v) {
										HandleValueUpdate(
											a_handle,
											a_data,
											a_params,
											a_exists);
									},
									[&] {
										ImGui::Spacing();

										DrawValueFlags(
											a_handle,
											a_data,
											e,
											a_params,
											a_exists);
									});
							}

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
		template <class Ta, class Tb>
		void UINodeOverrideEditorWidget<T>::DrawOverrideList(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			Ta&                      a_entry,
			const Tb&                a_params,
			const bool               a_exists)
		{
			if (a_entry.empty())
			{
				return;
			}

			ImGui::PushID("override_list");

			int i = 0;

			auto it = a_entry.begin();

			while (it != a_entry.end())
			{
				ImGui::PushID(i);

				const auto result = DrawOverrideContextMenu(
					a_handle,
					a_data,
					*it,
					a_params,
					a_exists);

				switch (result.action)
				{
				case NodeOverrideCommonAction::Delete:
					it = a_entry.erase(it);
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
					break;
				case NodeOverrideCommonAction::Insert:
					it              = a_entry.emplace(it);
					it->description = result.desc;
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
					ImGui::SetNextItemOpen(true);
					break;
				case NodeOverrideCommonAction::Swap:

					if (IterSwap(a_entry, it, result.dir))
					{
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}

					break;
				case NodeOverrideCommonAction::Reset:

					break;
				case NodeOverrideCommonAction::Rename:
					it->description = result.desc;
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
					break;
				case NodeOverrideCommonAction::Paste:
					if (auto clipData = UIClipboard::Get<typename Ta::value_type>())
					{
						it = a_entry.emplace(it, *clipData);
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}
					[[fallthrough]];
				case NodeOverrideCommonAction::PasteOver:
					ImGui::SetNextItemOpen(true);
					break;
				}

				if (it != a_entry.end())
				{
					auto& e = *it;

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::TreeNodeEx(
							"op_item",
							ImGuiTreeNodeFlags_SpanAvailWidth |
								ImGuiTreeNodeFlags_DefaultOpen,
							"%s",
							e.description.c_str()))
					{
						ImGui::Spacing();

						DrawConditionTree(
							a_params.name,
							GetNodeOverrideDataTypeFromParams<Tb>(),
							a_data,
							e.conditions,
							a_exists,
							true,
							[&] {
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							});

						ImGui::Spacing();

						DrawValues(
							a_handle,
							a_data,
							e,
							a_params,
							a_exists);

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
		template <class Td>
		void UINodeOverrideEditorWidget<T>::DrawCMNodeListPanel(
			T                                     a_handle,
			const NodeOverrideData::cm_data_type& a_cmdata,
			entryNodeOverrideData_t&              a_data)
		{
			const auto configClass = GetConfigClass();

			auto itb = a_cmdata.getvec().begin();

			for (const auto& e : a_cmdata.getvec())
			{
				if (!m_itemFilter.Test(*e->second.desc))
				{
					continue;
				}

				ImGui::PushID(e->first.c_str());

				auto it = a_data.get_data<Td>().find(e->first);

				bool svar = it == a_data.get_data<Td>().end();

				if (svar)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
				}
				else
				{
					if (configClass != Data::ConfigClass::Global)
					{
						svar = configClass != it->second.first;

						if (svar)
						{
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
						}
					}
				}

				it = DrawCommonEntryContextMenu<Td>(
					a_handle,
					a_data,
					e->first,
					it);

				if (TreeEx(
						"entry_tree",
						e->first == (*itb)->first,
						"%s",
						e->second.desc.c_str()))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.get_data<Td>().end())
					{
						if (configClass != Data::ConfigClass::Global)
						{
							DrawConfigClassHeader(it->second.first);
						}

						DrawEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						decltype(it->second) tmp;

						DrawEntry(a_handle, a_data, { GetSex(), e->first, tmp }, false);
					}

					ImGui::Unindent();

					ImGui::TreePop();
				}

				if (svar)
				{
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		template <class T>
		NodeOverrideCommonResult UINodeOverrideEditorWidget<T>::DrawOffsetContextMenu(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffset_t&              a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			NodeOverrideCommonResult result;

			ImGui::PushID("oo_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				if (!a_entry.description.empty())
				{
					SetDescriptionPopupBuffer(a_entry.description);
				}
				else
				{
					ClearDescriptionPopupBuffer();
				}
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::New, "2"))
					{
						if (UIL::LCG_BM(CommonStrings::Item, "0"))
						{
							if (DrawDescriptionPopup())
							{
								result.action  = NodeOverrideCommonAction::Insert;
								result.desc    = GetDescriptionPopupBuffer();
								result.isGroup = false;

								ClearDescriptionPopupBuffer();
							}
							ImGui::EndMenu();
						}

						if (UIL::LCG_BM(CommonStrings::Group, "1"))
						{
							if (DrawDescriptionPopup())
							{
								result.action  = NodeOverrideCommonAction::Insert;
								result.desc    = GetDescriptionPopupBuffer();
								result.isGroup = true;

								ClearDescriptionPopupBuffer();
							}
							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "5"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = NodeOverrideCommonAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "6")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				if (UIL::LCG_BM(CommonStrings::Rename, "7"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = NodeOverrideCommonAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Reset, "8")))
				{
					result.action = NodeOverrideCommonAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "9")))
				{
					UIClipboard::Set(a_entry);
				}

				bool hasClipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>() ||
				                   UIClipboard::Get<Data::configNodeOverrideTransformValues_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "A"),
						nullptr,
						false,
						hasClipData))
				{
					bool update = false;

					if (auto cd1 = UIClipboard::Get<Data::configNodeOverrideOffset_t>())
					{
						a_entry = *cd1;

						update = true;
					}
					else if (auto cd2 = UIClipboard::Get<Data::configNodeOverrideTransformValues_t>())
					{
						static_cast<Data::configNodeOverrideTransformValues_t&>(a_entry) = *cd2;

						update = true;
					}

					if (update)
					{
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result.action = NodeOverrideCommonAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class T>
		template <class Ta, class Tb>
		NodeOverridePlacementOverrideResult UINodeOverrideEditorWidget<T>::DrawOverrideContextMenu(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			Ta&                      a_entry,
			const Tb&                a_params,
			const bool               a_exists)
		{
			NodeOverridePlacementOverrideResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				if (!a_entry.description.empty())
				{
					SetDescriptionPopupBuffer(a_entry.description);
				}
				else
				{
					ClearDescriptionPopupBuffer();
				}
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::New, "2"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = NodeOverrideCommonAction::Insert;
							result.desc   = GetDescriptionPopupBuffer();

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Ta>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = NodeOverrideCommonAction::Paste;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "4")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				if (UIL::LCG_BM(CommonStrings::Rename, "5"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = NodeOverrideCommonAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Reset, "6")))
				{
					result.action = NodeOverrideCommonAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_entry);
				}

				using secondary_type = typename detail::value_type_selector<Ta>::type;

				bool hasClipData = UIClipboard::Get<Ta>() ||
				                   UIClipboard::Get<secondary_type>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "8"),
						nullptr,
						false,
						hasClipData))
				{
					bool update = false;

					if (auto cd1 = UIClipboard::Get<Ta>())
					{
						a_entry = *cd1;

						update = true;
					}
					else if (auto cd2 = UIClipboard::Get<secondary_type>())
					{
						static_cast<secondary_type&>(a_entry) = *cd2;

						update = true;
					}

					if (update)
					{
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result.action = NodeOverrideCommonAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOffsetAdjust(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffset_t&              a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			/*if (ImGui::CheckboxFlagsT(
					"X##ctl",
					stl::underlying(std::addressof(a_entry.offsetFlags.value)),
					stl::underlying(Data::NodeOverrideOffsetFlags::kAdjustX)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::SameLine(0.0, 10.0f);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine(0.0, 10.0f);

			if (ImGui::CheckboxFlagsT(
					"Y##ctl",
					stl::underlying(std::addressof(a_entry.offsetFlags.value)),
					stl::underlying(Data::NodeOverrideOffsetFlags::kAdjustY)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::SameLine(0.0, 10.0f);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine(0.0, 10.0f);

			if (ImGui::CheckboxFlagsT(
					"Z##ctl",
					stl::underlying(std::addressof(a_entry.offsetFlags.value)),
					stl::underlying(Data::NodeOverrideOffsetFlags::kAdjustZ)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}*/

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.00005f : 0.005f;

			if (ImGui::DragFloat3(
					UIL::LS(CommonStrings::Scale, "ctl_1"),
					a_entry.adjustScale,
					dragSpeed,
					-20.0f,
					20.0f,
					"%.3f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::IgnoreDead, "ctl_2"),
					stl::underlying(std::addressof(a_entry.offsetFlags.value)),
					stl::underlying(Data::NodeOverrideOffsetFlags::kAdjustIgnoreDead)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}
		}

		template <class T>
		template <class Td>
		auto UINodeOverrideEditorWidget<T>::DrawCommonEntryContextMenu(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			const stl::fixed_string& a_name,
			typename Td::iterator    a_it)
			-> typename Td::iterator
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });

			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				auto configClass = GetConfigClass();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Clear, "1"),
						nullptr,
						false,
						a_it != a_data.get_data<Td>().end() &&
							a_it->second.first == configClass))
				{
					if (a_it != a_data.get_data<Td>().end() &&
					    a_it->second.first == configClass)
					{
						a_data.get_data<Td>().erase(a_it);

						DoOnClear<Td>(a_handle, { a_name, a_data });

						a_it = a_data.get_data<Td>().find(a_name);
					}
				}

				//if constexpr (std::is_same_v<Td, entryNodeOverrideData_t::transform_data_type>)
				//{
				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Reset, "2"),
						nullptr,
						false,
						a_it != a_data.get_data<Td>().end()))
				{
					if (a_it != a_data.get_data<Td>().end())
					{
						const auto sex = GetSex();

						a_it->second.second(sex).clear();

						OnUpdate(a_handle, { sex, a_name, a_it->second });
					}
				}
				//}

				ImGui::Separator();

				bool has = a_it != a_data.get_data<Td>().end();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Copy, "A"),
						nullptr,
						false,
						has))
				{
					if (has)
					{
						UIClipboard::Set(a_it->second.second(GetSex()));
					}
				}

				using primary_type   = typename Td::mapped_type::second_type::config_type;
				using secondary_type = typename detail::value_type_selector<primary_type>::type;

				bool hasClipData = static_cast<bool>(UIClipboard::Get<primary_type>());

				if constexpr (!std::is_same_v<secondary_type, void>)
				{
					hasClipData |= static_cast<bool>(UIClipboard::Get<secondary_type>());
				}

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "B"),
						nullptr,
						false,
						hasClipData))
				{
					bool       update = false;
					const auto sex    = GetSex();

					if (auto cd1 = UIClipboard::Get<primary_type>())
					{
						if (!has)
						{
							a_it = a_data.get_data<Td>().try_emplace(a_name).first;
						}

						a_it->second.second(sex) = *cd1;
						a_it->second.first       = GetConfigClass();

						update = true;
					}
					else
					{
						if constexpr (!std::is_same_v<secondary_type, void>)
						{
							if (auto cd2 = UIClipboard::Get<secondary_type>())
							{
								if (!has)
								{
									a_it = a_data.get_data<Td>().try_emplace(a_name).first;
								}

								static_cast<secondary_type&>(a_it->second.second(sex)) = *cd2;

								update = true;
							}
						}
					}

					if (update)
					{
						OnUpdate(a_handle, { sex, a_name, a_it->second });
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return a_it;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::EditorDrawMenuBarItems()
		{
			auto current = GetCurrentData();

			bool disabled = !current;

			UICommon::PushDisabled(disabled);

			if (UIL::LCG_MI(UIWidgetCommonStrings::ClearAll, "1"))
			{
				if (current.data)
				{
					QueueClearAllPopup(current);
				}
			}

			ImGui::Separator();

			if (UIL::LCG_MI(CommonStrings::Copy, "3"))
			{
				if (current.data)
				{
					auto confClass = GetConfigClass();

					UIClipboard::Set<Data::configNodeOverrideHolderClipboardData_t>(
						confClass,
						GetSex(),
						current.data->copy_cc(confClass));
				}
			}

			auto clipData = UIClipboard::Get<Data::configNodeOverrideHolderClipboardData_t>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::PasteOver, "4"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData && current.data)
				{
					QueuePasteOverPopup(current, *clipData);
				}
			}

			UICommon::PopDisabled(disabled);
		}

		template <class T>
		auto UINodeOverrideEditorWidget<T>::GetPanelTypeFromFlags(
			stl::flag<NodeOverrideEditorFlags> a_flags) -> PanelType
		{
			if (a_flags.test(NodeOverrideEditorFlags::kDrawPhysics))
			{
				return PanelType::Physics;
			}
			else if (a_flags.test(NodeOverrideEditorFlags::kDrawNodePlacement))
			{
				return PanelType::Placement;
			}
			else
			{
				return PanelType::Transform;
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::QueueClearAllPopup(
			const NodeOverrideEditorCurrentData& a_data)
		{
			const auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			auto& queue = GetPopupQueue();
			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UINodeOverrideEditorWidgetStrings::ClearAllPrompt))
				.call([this,
			           handle     = a_data.handle,
			           panel_type = GetPanelTypeFromFlags(flags.value)](const auto&) {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (handle != current.handle)
					{
						return;
					}

					switch (panel_type)
					{
					case PanelType::Physics:
						current.data->physicsData.clear();
						OnClearAllPhysics(current.handle, { *current.data });
						break;
					case PanelType::Placement:
						current.data->placementData.clear();
						OnClearAllPlacement(current.handle, { *current.data });
						break;
					case PanelType::Transform:
						current.data->transformData.clear();
						OnClearAllTransforms(current.handle, { *current.data });
						break;
					}
				});
		}

		template <class Ts, class Td>
		static constexpr void paste_move_entries(
			Ts&&              a_src,
			Td&               a_dst,
			Data::ConfigSex   a_srcSex,
			Data::ConfigSex   a_dstSex,
			Data::ConfigClass a_class)
		{
			for (auto& e : a_src)
			{
				auto it = a_dst.try_emplace(e.first).first;

				it->second.first = a_class;
				it->second.second(a_dstSex) =
					std::move(e.second(a_srcSex));
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::QueuePasteOverPopup(
			const NodeOverrideEditorCurrentData&                 a_data,
			const Data::configNodeOverrideHolderClipboardData_t& a_clipData)
		{
			const auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			auto& queue = GetPopupQueue();
			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UINodeOverrideEditorWidgetStrings::PasteOverFullPrompt))
				.call([this,
			           handle     = a_data.handle,
			           dstSex     = GetSex(),
			           panel_type = GetPanelTypeFromFlags(flags.value),
			           data       = a_clipData](const auto&) mutable {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (current.handle != handle)
					{
						return;
					}

					switch (panel_type)
					{
					case PanelType::Physics:
						paste_move_entries(
							std::move(data.data.physicsData),
							current.data->physicsData,
							data.sex,
							dstSex,
							GetConfigClass());
						break;
					case PanelType::Placement:
						paste_move_entries(
							std::move(data.data.placementData),
							current.data->placementData,
							data.sex,
							dstSex,
							GetConfigClass());
						break;
					case PanelType::Transform:
						paste_move_entries(
							std::move(data.data.transformData),
							current.data->transformData,
							data.sex,
							dstSex,
							GetConfigClass());
						break;
					}

					OnUpdate(handle, { *current.data });
				});
		}

		template <class T>
		template <class Td>
		void UINodeOverrideEditorWidget<T>::DoOnClear(
			T                                    a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			if constexpr (std::is_same_v<Td, entryNodeOverrideData_t::transform_data_type>)
			{
				OnClearTransform(a_handle, a_params);
			}
			else if constexpr (std::is_same_v<Td, entryNodeOverrideData_t::placement_data_type>)
			{
				OnClearPlacement(a_handle, a_params);
			}
			else if constexpr (std::is_same_v<Td, entryNodeOverrideData_t::physics_data_type>)
			{
				OnClearPhysics(a_handle, a_params);
			}
			else
			{
				//static_assert(false);
				HALT("fixme");
			}
		}

		template <class T>
		template <class Td>
		void UINodeOverrideEditorWidget<T>::HandleValueUpdate(
			T                        a_handle,
			entryNodeOverrideData_t& a_data,
			const Td&                a_params,
			const bool               a_exists)
		{
			if (!a_exists)
			{
				auto r = a_data.get_data<typename Td::data_type>().insert_or_assign(a_params.name, a_params.entry);

				OnUpdate(a_handle, { a_params.sex, r.first->first, r.first->second });
			}
			else
			{
				OnUpdate(a_handle, a_params);
			}
		}

	}
}