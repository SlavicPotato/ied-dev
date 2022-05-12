#pragma once

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UILocalizationInterface.h"
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
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UITransformSliderWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "IED/Controller/NodeOverrideData.h"

#include "UINodeOverrideEditorWidgetStrings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		using entryNodeOverrideData_t = Data::configNodeOverrideHolderCopy_t;

		template <class T>
		struct profileSelectorParamsNodeOverride_t
		{
			T                        handle;
			entryNodeOverrideData_t& data;
		};

		struct NodeOverrideUpdateParams
		{
			entryNodeOverrideData_t& data;
		};

		struct SingleNodeOverrideTransformUpdateParams
		{
			Data::ConfigSex                                            sex;
			stl::fixed_string                                          name;
			entryNodeOverrideData_t::transform_data_type::mapped_type& entry;
		};

		struct SingleNodeOverridePlacementUpdateParams
		{
			Data::ConfigSex                                            sex;
			stl::fixed_string                                          name;
			entryNodeOverrideData_t::placement_data_type::mapped_type& entry;
		};

		struct ClearNodeOverrideUpdateParams
		{
			stl::fixed_string        name;
			entryNodeOverrideData_t& entry;
		};

		struct ClearAllNodeOverrideUpdateParams
		{
			entryNodeOverrideData_t& entry;
		};

		enum class NodeOverrideCommonAction : std::uint8_t
		{
			None,
			Insert,
			Delete,
			Clear,
			Swap,
			Reset,
			Rename,
			Copy,
			Paste,
			PasteOver
		};

		enum class NodeOverrideEditorFlags : Data::SettingHolder::EditorPanelCommonFlagsType
		{
			kNone = 0,

			kDrawNodePlacement         = 1u << 0,
			kUnrestrictedNodePlacement = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideEditorFlags);

		struct NodeOverrideCommonResult
		{
			NodeOverrideCommonAction        action{ NodeOverrideCommonAction::None };
			stl::fixed_string               str;
			std::string                     desc;
			Data::NodeOverrideConditionType matchType;
			Game::FormID                    form;
			SwapDirection                   dir;

			union
			{
				BIPED_OBJECT             biped;
				Data::ExtraConditionType excond;
				Data::ObjectSlotExtra    type;
			};

			bool isGroup;
		};

		struct NodeOverridePlacementOverrideResult
		{
			NodeOverrideCommonAction action{ NodeOverrideCommonAction::None };
			std::string              desc;
			SwapDirection            dir;
		};

		template <class T>
		class UINodeOverrideEditorWidget :
			public UIEditorInterface,
			public UIEditorPanelSettings,
			public UICollapsibles,
			public UIDescriptionPopupWidget,
			public UIFormLookupInterface,
			UIConditionParamExtraInterface,
			public virtual UISettingsInterface,
			public virtual UITransformSliderWidget,
			public virtual UILocalizationInterface
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

			virtual void DrawMenuBarItems() override;

		private:
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

			virtual void DrawExtraEditorPanelSettings() override;

			void DrawItemFilter();

			void DrawConfigClassHeader(Data::ConfigClass a_class);

			void DrawTransformPanel(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

			void DrawPlacementPanel(
				T                        a_handle,
				entryNodeOverrideData_t& a_data);

			entryNodeOverrideData_t::transform_data_type::iterator DrawTransformEntryContextMenu(
				T                                                      a_handle,
				entryNodeOverrideData_t&                               a_data,
				const stl::fixed_string&                               a_name,
				entryNodeOverrideData_t::transform_data_type::iterator a_it);

			entryNodeOverrideData_t::placement_data_type::iterator DrawPlacementEntryContextMenu(
				T                                                      a_handle,
				entryNodeOverrideData_t&                               a_data,
				const stl::fixed_string&                               a_name,
				entryNodeOverrideData_t::placement_data_type::iterator a_it);

			void DrawTransformEntry(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawPlacementEntry(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawPlacementValues(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverridePlacementValues_t&     a_values,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawValueFlags(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideValues_t&              a_values,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			NodeOverrideCommonAction DrawOverrideOffsetListContextMenu(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			NodeOverrideCommonAction DrawOverridePlacementOverrideListContextMenu(
				T                                                a_handle,
				entryNodeOverrideData_t&                         a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverridePlacementUpdateParams&   a_params,
				const bool                                       a_exists);

			void DrawOverrideOffsets(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists,
				Data::configNodeOverrideOffset_t*              a_parent);

			void DrawOverridePlacementOverrides(
				T                                                a_handle,
				entryNodeOverrideData_t&                         a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverridePlacementUpdateParams&   a_params,
				const bool                                       a_exists);

			void DrawOverrideOffsetList(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffsetList_t&          a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			void DrawOverridePlacementOverrideList(
				T                                                a_handle,
				entryNodeOverrideData_t&                         a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverridePlacementUpdateParams&   a_params,
				const bool                                       a_exists);

			template <class Tp>
			void DrawOverrideConditionTree(
				T                                        a_handle,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp&                                a_params,
				const bool                               a_exists,
				const bool                               a_ignoreNode);

			NodeOverrideCommonResult DrawOverrideOffsetContextMenu(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffset_t&              a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			NodeOverridePlacementOverrideResult DrawOverridePlacementOverrideContextMenu(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverridePlacementOverride_t&   a_entry,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			template <class Tp>
			NodeOverrideCommonAction DrawOverrideConditionHeaderContextMenu(
				T                                        a_handle,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp&                                a_params,
				const bool                               a_exists,
				const bool                               a_ignoreNode);

			template <class Tp>
			void DrawOverrideConditionTable(
				T                                        a_handle,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp&                                a_params,
				const bool                               a_exists,
				bool                                     a_isnested,
				bool                                     a_ignoreNode);

			void DrawOverrideOffsetAdjust(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				Data::configNodeOverrideOffset_t&              a_entry,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			/*void DrawOverridePosClamp(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffset_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);*/

			template <class Tp>
			NodeOverrideCommonResult DrawOverrideConditionContextMenu(
				T                                    a_handle,
				entryNodeOverrideData_t&             a_data,
				Data::configNodeOverrideCondition_t& a_entry,
				const Tp&                            a_params,
				const bool                           a_exists,
				const bool                           a_ignoreNode);

			virtual void OnUpdate(
				T                                              a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T                                              a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T                               a_handle,
				const NodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearTransform(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearPlacement(
				T                                    a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearAllTransforms(
				T                                       a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearAllPlacement(
				T                                       a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) = 0;

			void HandleValueUpdate(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverrideTransformUpdateParams& a_params,
				const bool                                     a_exists);

			void HandleValueUpdate(
				T                                              a_handle,
				entryNodeOverrideData_t&                       a_data,
				const SingleNodeOverridePlacementUpdateParams& a_params,
				const bool                                     a_exists);

			virtual NodeOverrideEditorCurrentData GetCurrentData() = 0;

			virtual bool DrawConditionParamExtra(
				void*       a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args) override;

			void UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType a_type);

			virtual UIPopupQueue& GetPopupQueue() = 0;

			Game::FormID m_ooNewEntryID;
			Game::FormID m_ooNewEntryIDKW;
			Game::FormID m_ooNewEntryIDRace;
			Game::FormID m_ooNewEntryIDActor;
			Game::FormID m_ooNewEntryIDNPC;
			Game::FormID m_ooNewEntryIDGlob;

			BIPED_OBJECT             m_ooNewBiped{ BIPED_OBJECT::kNone };
			Data::ObjectSlotExtra    m_ooNewSlot{ Data::ObjectSlotExtra::kNone };
			Data::ExtraConditionType m_ooNewExtraCond{ Data::ExtraConditionType::kNone };

			UIConditionParamEditorWidget m_condParamEditor;

			struct
			{
				std::shared_ptr<const UIFormBrowser::tab_filter_type> form_common;
			} m_type_filters;

			char m_buffer1[260]{ 0 };

			UIGenericFilter m_itemFilter;
		};

		template <class T>
		UINodeOverrideEditorWidget<T>::UINodeOverrideEditorWidget(
			Controller& a_controller) :
			UIFormLookupInterface(a_controller),
			UIEditorPanelSettings(a_controller),
			m_itemFilter(true),
			m_condParamEditor(a_controller)
		{
			m_type_filters.form_common = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESAmmo::kTypeID,
			      TESObjectLIGH::kTypeID,
			      SpellItem::kTypeID,
			      BGSHeadPart::kTypeID,
			      IFormDatabase::EXTRA_TYPE_ARMOR });

			m_condParamEditor.SetExtraInterface(this);
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

			auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			if (flags.test(NodeOverrideEditorFlags::kDrawNodePlacement))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
				ImGui::TextWrapped("%s", LS(UITip::AnimSupportWarning));
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
			}
			else
			{
				if (ImGui::BeginChild(
						"et_editor_panel",
						{ -1.0f, 0.0f },
						false,
						ImGuiWindowFlags_HorizontalScrollbar))
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * 27.f);

					DrawTransformPanel(a_handle, a_data);

					ImGui::PopItemWidth();
				}

				ImGui::EndChild();
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
		void UINodeOverrideEditorWidget<T>::DrawExtraEditorPanelSettings()
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			if (ImGui::RadioButton(
					LS(CommonStrings::Positions, "1"),
					!flags.test(NodeOverrideEditorFlags::kDrawNodePlacement)))
			{
				flags.clear(NodeOverrideEditorFlags::kDrawNodePlacement);
				OnEditorPanelSettingsChange();
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					LS(CommonStrings::Placement, "2"),
					flags.test(NodeOverrideEditorFlags::kDrawNodePlacement)))
			{
				flags.set(NodeOverrideEditorFlags::kDrawNodePlacement);
				OnEditorPanelSettingsChange();
			}

			if (flags.test(NodeOverrideEditorFlags::kDrawNodePlacement))
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::CheckboxFlagsT(
						LS(UINodeOverrideEditorWidgetStrings::UnrestrictedPlacement, "3"),
						stl::underlying(std::addressof(flags.value)),
						stl::underlying(NodeOverrideEditorFlags::kUnrestrictedNodePlacement)))
				{
					OnEditorPanelSettingsChange();
				}
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawItemFilter()
		{
			if (TreeEx(
					"item_filter",
					false,
					"%s",
					LS(CommonStrings::Filter)))
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
			ImGui::Text("%s:", LS(UIWidgetCommonStrings::ConfigInUse));
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

			Data::configStoreNodeOverride_t::holderCache_t hc;

			auto configClass = GetConfigClass();

			auto itb = data.getvec().begin();

			for (const auto& e : data.getvec())
			{
				if (!m_itemFilter.Test(e->second.desc))
				{
					continue;
				}

				ImGui::PushID(e->first.c_str());

				auto it = a_data.data.find(e->first);

				bool svar = it == a_data.data.end();

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

				it = DrawTransformEntryContextMenu(a_handle, a_data, e->first, it);

				if (TreeEx(
						"entry_tree",
						e->first == (*itb)->first,
						"%s",
						e->second.desc))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.data.end())
					{
						if (configClass != Data::ConfigClass::Global)
						{
							DrawConfigClassHeader(it->second.first);
						}

						DrawTransformEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						decltype(it->second) tmp;

						DrawTransformEntry(a_handle, a_data, { GetSex(), e->first, tmp }, false);
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

			auto configClass = GetConfigClass();

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

				it = DrawPlacementEntryContextMenu(a_handle, a_data, e->first, it);

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

						DrawPlacementEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						stl::strip_type<decltype(it->second)> tmp;

						DrawPlacementEntry(a_handle, a_data, { GetSex(), e->first, tmp }, false);
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
		auto UINodeOverrideEditorWidget<T>::DrawTransformEntryContextMenu(
			T                                                      a_handle,
			entryNodeOverrideData_t&                               a_data,
			const stl::fixed_string&                               a_name,
			entryNodeOverrideData_t::transform_data_type::iterator a_it)
			-> entryNodeOverrideData_t::transform_data_type::iterator
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });

			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				auto confClass = GetConfigClass();

				if (ImGui::MenuItem(
						LS(CommonStrings::Clear, "1"),
						nullptr,
						false,
						a_it != a_data.data.end() &&
							a_it->second.first == confClass))
				{
					if (a_it != a_data.data.end() &&
					    a_it->second.first == confClass)
					{
						a_data.data.erase(a_it);

						OnClearTransform(a_handle, { a_name, a_data });

						a_it = a_data.data.find(a_name);
					}
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::Reset, "2"),
						nullptr,
						false,
						a_it != a_data.data.end()))
				{
					if (a_it != a_data.data.end())
					{
						auto sex = GetSex();

						auto& data = a_it->second.second(sex);

						data.transform = Data::configTransform_t();

						OnUpdate(a_handle, { sex, a_name, a_it->second });
					}
				}

				ImGui::Separator();

				bool has = a_it != a_data.data.end();

				if (ImGui::MenuItem(
						LS(CommonStrings::Copy, "3"),
						nullptr,
						false,
						has))
				{
					if (has)
					{
						UIClipboard::Set(a_it->second.second(GetSex()));
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideTransform_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						auto sex = GetSex();

						if (!has)
						{
							a_it = a_data.data.try_emplace(a_name).first;
						}

						a_it->second.second(sex) = *clipData;
						a_it->second.first       = GetConfigClass();

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
		entryNodeOverrideData_t::placement_data_type::iterator UINodeOverrideEditorWidget<T>::DrawPlacementEntryContextMenu(
			T                                                      a_handle,
			entryNodeOverrideData_t&                               a_data,
			const stl::fixed_string&                               a_name,
			entryNodeOverrideData_t::placement_data_type::iterator a_it)
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
						LS(CommonStrings::Clear, "1"),
						nullptr,
						false,
						a_it != a_data.placementData.end() &&
							a_it->second.first == configClass))
				{
					if (a_it != a_data.placementData.end() &&
					    a_it->second.first == configClass)
					{
						a_data.placementData.erase(a_it);

						OnClearPlacement(a_handle, { a_name, a_data });

						a_it = a_data.placementData.find(a_name);
					}
				}

				ImGui::Separator();

				bool has = a_it != a_data.placementData.end();

				if (ImGui::MenuItem(
						LS(CommonStrings::Copy, "2"),
						nullptr,
						false,
						has))
				{
					if (has)
					{
						UIClipboard::Set(a_it->second.second(GetSex()));
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverridePlacement_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						auto sex = GetSex();

						if (!has)
						{
							a_it = a_data.placementData.try_emplace(a_name).first;
						}

						a_it->second.second(sex) = *clipData;
						a_it->second.first       = GetConfigClass();

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
		void UINodeOverrideEditorWidget<T>::DrawTransformEntry(
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
					LS(CommonStrings::Visibility)))
			{
				ImGui::Spacing();

				if (ImGui::CheckboxFlagsT(
						LS(CommonStrings::Invisible, "ctl_1"),
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
						LS(UIWidgetCommonStrings::RequiresMatchList, "ctl_2"),
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

				DrawOverrideConditionTree(
					a_handle,
					a_data,
					data.visibilityConditionList,
					a_params,
					a_exists,
					true);

				UICommon::PopDisabled(disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			DrawOverrideOffsets(
				a_handle,
				a_data,
				data.offsets,
				a_params,
				a_exists,
				nullptr);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawPlacementEntry(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			const SingleNodeOverridePlacementUpdateParams& a_params,
			const bool                                     a_exists)
		{
			auto& data = a_params.entry.second(a_params.sex);

			ImGui::PushID("pl_entry");

			DrawPlacementValues(
				a_handle,
				a_data,
				data,
				a_params,
				a_exists);

			ImGui::Spacing();

			ImGui::Spacing();

			DrawOverridePlacementOverrides(
				a_handle,
				a_data,
				data.overrides,
				a_params,
				a_exists);

			ImGui::PopID();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawPlacementValues(
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

			if (ImGui::Button(LS(CommonStrings::Clear, "ctl_1")))
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
					LS(UIWidgetCommonStrings::TargetNode, "node_sel"),
					a_values.targetNode,
					entry->movs,
					nullptr,
					std::addressof(NodeOverrideData::GetMOVNodeData()));
			}
			else
			{
				result = UICMNodeSelectorWidget::DrawCMNodeSelector(
					LS(UIWidgetCommonStrings::TargetNode, "node_sel"),
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
		void UINodeOverrideEditorWidget<T>::DrawOverrideOffsets(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffsetList_t&          a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists,
			Data::configNodeOverrideOffset_t*              a_parent)
		{
			const auto result = DrawOverrideOffsetListContextMenu(
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
					LS(CommonStrings::Offsets)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOverrideOffsetList(
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
		void UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrides(
			T                                                a_handle,
			entryNodeOverrideData_t&                         a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverridePlacementUpdateParams&   a_params,
			const bool                                       a_exists)
		{
			const auto result = DrawOverridePlacementOverrideListContextMenu(
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
					LS(CommonStrings::Overrides)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOverridePlacementOverrideList(a_handle, a_data, a_entry, a_params, a_exists);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawValueFlags(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideValues_t&              a_values,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			ImGui::PushID("value_flags");

			if (ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorWidgetStrings::AbsolutePosition, "1"),
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
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverrideOffsetListContextMenu(
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
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if (LCG_BM(CommonStrings::Item, "2"))
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

					if (LCG_BM(CommonStrings::Group, "3"))
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

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "4")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipDataOffset = UIClipboard::Get<Data::configNodeOverrideOffset_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "5"),
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
						LS(CommonStrings::PasteOver, "6"),
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
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrideListContextMenu(
			T                                                a_handle,
			entryNodeOverrideData_t&                         a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverridePlacementUpdateParams&   a_params,
			const bool                                       a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("op_list_tree_context_area");

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
				if (LCG_BM(CommonStrings::Add, "1"))
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

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipDataOffset = UIClipboard::Get<Data::configNodeOverridePlacementOverride_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "3"),
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

				auto clipData = UIClipboard::Get<Data::configNodeOverridePlacementOverrideList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
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
		void UINodeOverrideEditorWidget<T>::DrawOverrideOffsetList(
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

				const auto result = DrawOverrideOffsetContextMenu(a_handle, a_data, *it, a_params, a_exists);

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
					// fallthrough
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

						DrawOverrideConditionTree(
							a_handle,
							a_data,
							e.conditions,
							a_params,
							a_exists,
							false);

						if (ImGui::CheckboxFlagsT(
								LS(CommonStrings::Continue, "hctl_1"),
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
								LS(UIWidgetCommonStrings::RequiresMatchList, "hctl_2"),
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

							DrawOverrideOffsets(
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
									LS(UINodeOverrideEditorWidgetStrings::WeaponAdjust, "3"),
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
									LS(UINodeOverrideEditorWidgetStrings::WeightAdjust, "4"),
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
										LS(UINodeOverrideEditorWidgetStrings::UseAdjustsFromMatched, "5"),
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
									LS(UINodeOverrideEditorWidgetStrings::AccumulatePosition, "6"),
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
									LS(UINodeOverrideEditorWidgetStrings::LockToAccum, "7"),
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
									LS(CommonStrings::Reset, "8"),
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
										LS(CommonStrings::Adjust)))
								{
									ImGui::Spacing();

									DrawValueFlags(a_handle, a_data, e, a_params, a_exists);

									ImGui::Spacing();

									DrawOverrideOffsetAdjust(
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
		void UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrideList(
			T                                                a_handle,
			entryNodeOverrideData_t&                         a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverridePlacementUpdateParams&   a_params,
			const bool                                       a_exists)
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

				const auto result = DrawOverridePlacementOverrideContextMenu(
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
					if (auto clipData = UIClipboard::Get<Data::configNodeOverridePlacementOverride_t>())
					{
						it = a_entry.emplace(it, *clipData);
						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);
					}
					// fallthrough
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

						DrawOverrideConditionTree(
							a_handle,
							a_data,
							e.conditions,
							a_params,
							a_exists,
							true);

						ImGui::Spacing();

						DrawPlacementValues(
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
		template <class Tp>
		void UINodeOverrideEditorWidget<T>::DrawOverrideConditionTree(
			T                                        a_handle,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp&                                a_params,
			const bool                               a_exists,
			const bool                               a_ignoreNode)
		{
			ImGui::PushID("ovr_cond_tree");

			const auto result = DrawOverrideConditionHeaderContextMenu(
				a_handle,
				a_data,
				a_entry,
				a_params,
				a_exists,
				a_ignoreNode);

			bool empty = a_entry.empty();

			if (!empty)
			{
				if (result == NodeOverrideCommonAction::Insert ||
				    result == NodeOverrideCommonAction::PasteOver)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(CommonStrings::Conditions)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOverrideConditionTable(
						a_handle,
						a_data,
						a_entry,
						a_params,
						a_exists,
						false,
						a_ignoreNode);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
		}

		template <class T>
		NodeOverrideCommonResult UINodeOverrideEditorWidget<T>::DrawOverrideOffsetContextMenu(
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
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "2"))
					{
						if (LCG_BM(CommonStrings::Group, "3"))
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

						if (LCG_BM(CommonStrings::Item, "4"))
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

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "5"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = NodeOverrideCommonAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "6")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				if (LCG_BM(CommonStrings::Rename, "7"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = NodeOverrideCommonAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Reset, "8")))
				{
					result.action = NodeOverrideCommonAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "9")))
				{
					UIClipboard::Set(a_entry);
				}

				bool hasClipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>() ||
				                   UIClipboard::Get<Data::configNodeOverrideValues_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "A"),
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
					else if (auto cd2 = UIClipboard::Get<Data::configNodeOverrideValues_t>())
					{
						static_cast<Data::configNodeOverrideValues_t&>(a_entry) = *cd2;

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
		NodeOverridePlacementOverrideResult UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrideContextMenu(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverridePlacementOverride_t&   a_entry,
			const SingleNodeOverridePlacementUpdateParams& a_params,
			const bool                                     a_exists)
		{
			NodeOverridePlacementOverrideResult result;

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
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "2"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = NodeOverrideCommonAction::Insert;
							result.desc   = GetDescriptionPopupBuffer();

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					auto clipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>();

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = NodeOverrideCommonAction::Paste;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "4")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				if (LCG_BM(CommonStrings::Rename, "5"))
				{
					if (DrawDescriptionPopup())
					{
						result.action = NodeOverrideCommonAction::Rename;
						result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Reset, "6")))
				{
					result.action = NodeOverrideCommonAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_entry);
				}

				bool hasClipData = UIClipboard::Get<Data::configNodeOverridePlacementOverride_t>() ||
				                   UIClipboard::Get<Data::configNodeOverridePlacementValues_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "8"),
						nullptr,
						false,
						hasClipData))
				{
					bool update = false;

					if (auto cd1 = UIClipboard::Get<Data::configNodeOverridePlacementOverride_t>())
					{
						a_entry = *cd1;

						update = true;
					}
					else if (auto cd2 = UIClipboard::Get<Data::configNodeOverridePlacementValues_t>())
					{
						static_cast<Data::configNodeOverridePlacementValues_t&>(a_entry) = *cd2;

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
		template <class Tp>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverrideConditionHeaderContextMenu(
			T                                        a_handle,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp&                                a_params,
			const bool                               a_exists,
			const bool                               a_ignoreNode)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("oo_matchlist_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_ooNewEntryID      = {};
				m_ooNewEntryIDKW    = {};
				m_ooNewEntryIDRace  = {};
				m_ooNewEntryIDActor = {};
				m_ooNewEntryIDNPC   = {};
				m_ooNewEntryIDGlob  = {};
				m_ooNewSlot         = Data::ObjectSlotExtra::kNone;
				m_ooNewBiped        = BIPED_OBJECT::kNone;
				m_ooNewExtraCond    = Data::ExtraConditionType::kNone;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if constexpr (std::is_same_v<Tp, SingleNodeOverrideTransformUpdateParams>)
					{
						if (!a_ignoreNode)
						{
							if (LCG_BM(CommonStrings::Node, "2"))
							{
								stl::fixed_string c;

								if (UICMNodeSelectorWidget::DrawCMNodeSelector(
										"##node_sel",
										c,
										NodeOverrideData::GetCMENodeData(),
										std::addressof(a_params.name)))
								{
									a_entry.emplace_back(std::move(c));

									HandleValueUpdate(
										a_handle,
										a_data,
										a_params,
										a_exists);

									result = NodeOverrideCommonAction::Insert;

									ImGui::CloseCurrentPopup();
								}

								ImGui::EndMenu();
							}
						}
					}

					if (LCG_BM(CommonStrings::Item, "3"))
					{
						if (LCG_BM(CommonStrings::Form, "1"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_condParamEditor.GetFormPicker().DrawFormSelector(
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									a_entry.emplace_back(
										Data::NodeOverrideConditionType::Form,
										m_ooNewEntryID);

									HandleValueUpdate(
										a_handle,
										a_data,
										a_params,
										a_exists);

									result = NodeOverrideCommonAction::Insert;
								}

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}

						if (LCG_BM(CommonStrings::Keyword, "2"))
						{
							if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									a_entry.emplace_back(
										Data::NodeOverrideConditionType::Keyword,
										m_ooNewEntryIDKW);

									HandleValueUpdate(
										a_handle,
										a_data,
										a_params,
										a_exists);

									result = NodeOverrideCommonAction::Insert;
								}

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(UIWidgetCommonStrings::BipedSlot, "3"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								LS(CommonStrings::Biped, "1"),
								m_ooNewBiped))
						{
							if (m_ooNewBiped != BIPED_OBJECT::kNone)
							{
								a_entry.emplace_back(m_ooNewBiped);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Type, "4"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								LS(CommonStrings::Type, "1"),
								m_ooNewSlot))
						{
							if (m_ooNewSlot < Data::ObjectSlotExtra::kMax)
							{
								a_entry.emplace_back(m_ooNewSlot);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Actor, "5"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Actor);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDActor))
						{
							if (m_ooNewEntryIDActor)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::Actor,
									m_ooNewEntryIDActor);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::NPC, "6"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::NPC);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDNPC))
						{
							if (m_ooNewEntryIDNPC)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::NPC,
									m_ooNewEntryIDNPC);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Race, "7"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Race,
							m_ooNewEntryIDRace);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Furniture, "8")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Furniture);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Location, "9")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Location);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Worldspace, "A")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Worldspace);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Package, "B")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Package);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Weather, "C")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Weather);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (LCG_BM(CommonStrings::Global, "D"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDGlob))
						{
							if (m_ooNewEntryIDGlob)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::Global,
									m_ooNewEntryIDGlob);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(LS(UIWidgetCommonStrings::Mount, "F")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Mount);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						result = NodeOverrideCommonAction::Insert;
					}

					if (LCG_BM(CommonStrings::Extra, "Y"))
					{
						if (m_condParamEditor.DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							if (m_ooNewExtraCond != Data::ExtraConditionType::kNone)
							{
								a_entry.emplace_back(
									m_ooNewExtraCond);

								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(LS(CommonStrings::Group, "Z")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Group);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Clear, "2")))
				{
					a_entry.clear();

					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);

					result = NodeOverrideCommonAction::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideConditionList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
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
		template <class Tp>
		void UINodeOverrideEditorWidget<T>::DrawOverrideConditionTable(
			T                                        a_handle,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp&                                a_params,
			const bool                               a_exists,
			bool                                     a_isnested,
			bool                                     a_ignoreNode)
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

			if constexpr (std::is_same_v<Tp, SingleNodeOverridePlacementUpdateParams>)
			{
				width = -1.0f;
			}
			else
			{
				if (a_isnested)
				{
					width = -1.0f;
				}
				else
				{
					width = std::max(
						ImGui::GetFontSize() * 30.0f,
						ImGui::GetContentRegionAvail().x - ImGui::GetFontSize());
				}
			}

			if (ImGui::BeginTable(
					"offset_match_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ width, 0.0f }))
			{
				auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 40.0f);
				ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_None, 200.0f);
				ImGui::TableSetupColumn("And", ImGuiTableColumnFlags_None, 17.0f);
				ImGui::TableSetupColumn("Not", ImGuiTableColumnFlags_None, 17.0f);

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

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawOverrideConditionContextMenu(
						a_handle,
						a_data,
						*it,
						a_params,
						a_exists,
						a_ignoreNode);

					switch (result.action)
					{
					case NodeOverrideCommonAction::Insert:

						switch (result.matchType)
						{
						case Data::NodeOverrideConditionType::Node:

							it = a_entry.emplace(
								it,
								std::move(result.str));

							break;
						case Data::NodeOverrideConditionType::Form:
						case Data::NodeOverrideConditionType::Keyword:
						case Data::NodeOverrideConditionType::Actor:
						case Data::NodeOverrideConditionType::NPC:
						case Data::NodeOverrideConditionType::Global:
						case Data::NodeOverrideConditionType::Mount:

							it = a_entry.emplace(
								it,
								result.matchType,
								result.form);

							break;
						case Data::NodeOverrideConditionType::BipedSlot:
							if (result.biped != BIPED_OBJECT::kNone)
							{
								it = a_entry.emplace(
									it,
									result.biped);
							}

							break;
						case Data::NodeOverrideConditionType::Type:

							if (result.type < Data::ObjectSlotExtra::kMax)
							{
								it = a_entry.emplace(
									it,
									result.type);
							}

							break;

						case Data::NodeOverrideConditionType::Race:
						case Data::NodeOverrideConditionType::Furniture:
						case Data::NodeOverrideConditionType::Group:
						case Data::NodeOverrideConditionType::Location:
						case Data::NodeOverrideConditionType::Worldspace:
						case Data::NodeOverrideConditionType::Package:
						case Data::NodeOverrideConditionType::Weather:

							it = a_entry.emplace(
								it,
								result.matchType);

							break;
						case Data::NodeOverrideConditionType::Extra:

							if (result.excond != Data::ExtraConditionType::kNone)
							{
								it = a_entry.emplace(
									it,
									result.excond);
							}

							break;
						}

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

						break;
					case NodeOverrideCommonAction::Delete:
						it = a_entry.erase(it);

						HandleValueUpdate(
							a_handle,
							a_data,
							a_params,
							a_exists);

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
					}

					if (it != a_entry.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (e.fbf.type == Data::NodeOverrideConditionType::Group)
						{
							ImGui::TextUnformatted(LS(CommonStrings::Group));

							ImGui::TableSetColumnIndex(2);

							ImGui::PushID("cond_grp");

							DrawOverrideConditionHeaderContextMenu(
								a_handle,
								a_data,
								e.group.conditions,
								a_params,
								a_exists,
								a_ignoreNode);

							DrawOverrideConditionTable(
								a_handle,
								a_data,
								e.group.conditions,
								a_params,
								a_exists,
								true,
								a_ignoreNode);

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
							case Data::NodeOverrideConditionType::Node:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kAllowBipedNone);

								m_condParamEditor.SetNext<ConditionParamItem::CMENode>(
									e.node,
									a_params.name);
								m_condParamEditor.SetNext<ConditionParamItem::BipedSlot>(
									e.bipedSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = LS(CommonStrings::Node);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::CMENode);

								break;
							case Data::NodeOverrideConditionType::Form:

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
							case Data::NodeOverrideConditionType::Keyword:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearKeyword);

								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = LS(CommonStrings::Keyword);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Keyword);

								break;
							case Data::NodeOverrideConditionType::BipedSlot:

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
							case Data::NodeOverrideConditionType::Type:

								m_condParamEditor.SetNext<ConditionParamItem::EquipmentSlotExtra>(
									e.typeSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlotExtra);
								tdesc = LS(CommonStrings::Type);

								break;
							case Data::NodeOverrideConditionType::Race:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = LS(CommonStrings::Race);

								break;
							case Data::NodeOverrideConditionType::Actor:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Actor);

								break;
							case Data::NodeOverrideConditionType::NPC:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::NPC);

								break;
							case Data::NodeOverrideConditionType::Furniture:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Furniture);
								tdesc = LS(CommonStrings::Furniture);

								break;
							case Data::NodeOverrideConditionType::Extra:

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

							case Data::NodeOverrideConditionType::Location:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = LS(CommonStrings::Location);

								break;
							case Data::NodeOverrideConditionType::Worldspace:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = LS(CommonStrings::Worldspace);

								break;
							case Data::NodeOverrideConditionType::Package:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::PackageType>(
									e.procedureType);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::PackageType);
								tdesc = LS(CommonStrings::Package);

								break;

							case Data::NodeOverrideConditionType::Weather:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::WeatherClass>(
									e.weatherClass);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::WeatherClass);
								tdesc = LS(CommonStrings::Weather);

								break;

							case Data::NodeOverrideConditionType::Global:

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
								
							case Data::NodeOverrideConditionType::Mount:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Race>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr, true);
								tdesc = LS(UIWidgetCommonStrings::Mount);

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
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

						ImGui::TableSetColumnIndex(3);

						if (ImGui::CheckboxFlagsT(
								"##oper_and",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::NodeOverrideConditionFlags::kAnd)))
						{
							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);
						}

						ImGui::TableSetColumnIndex(4);

						if (ImGui::CheckboxFlagsT(
								"##oper_not",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::NodeOverrideConditionFlags::kNot)))
						{
							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);
						}

						ImGui::PopStyleVar();

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOverrideOffsetAdjust(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			Data::configNodeOverrideOffset_t&              a_entry,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			if (ImGui::CheckboxFlagsT(
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
			}

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.00005f : 0.005f;

			if (ImGui::DragFloat3(
					LS(CommonStrings::Scale, "ctl_1"),
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
					LS(UIWidgetCommonStrings::IgnoreDead, "ctl_2"),
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
		void UINodeOverrideEditorWidget<T>::DrawMenuBarItems()
		{
			auto current = GetCurrentData();

			bool disabled = !current;

			UICommon::PushDisabled(disabled);

			if (LCG_MI(UIWidgetCommonStrings::ClearAll, "1"))
			{
				if (current.data)
				{
					QueueClearAllPopup(current);
				}
			}

			ImGui::Separator();

			if (LCG_MI(CommonStrings::Copy, "3"))
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
					LS(CommonStrings::PasteOver, "4"),
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
		void UINodeOverrideEditorWidget<T>::QueueClearAllPopup(
			const NodeOverrideEditorCurrentData& a_data)
		{
			const auto flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			auto& queue = GetPopupQueue();
			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UINodeOverrideEditorWidgetStrings::ClearAllPrompt))
				.call([this,
			           handle       = a_data.handle,
			           is_placement = flags.test(NodeOverrideEditorFlags::kDrawNodePlacement)](const auto&) {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (handle != current.handle)
					{
						return;
					}

					if (is_placement)
					{
						current.data->placementData.clear();
						OnClearAllPlacement(current.handle, { *current.data });
					}
					else
					{
						current.data->data.clear();
						OnClearAllTransforms(current.handle, { *current.data });
					}
				});
		}

		template <class Ts, class Td>
		static inline constexpr void paste_move_entries(
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
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UINodeOverrideEditorWidgetStrings::PasteOverFullPrompt))
				.call([this,
			           handle       = a_data.handle,
			           dstSex       = GetSex(),
			           is_placement = flags.test(NodeOverrideEditorFlags::kDrawNodePlacement),
			           data         = a_clipData](const auto&) mutable {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (current.handle != handle)
					{
						return;
					}

					if (is_placement)
					{
						paste_move_entries(
							std::move(data.data.placementData),
							current.data->placementData,
							data.sex,
							dstSex,
							GetConfigClass());
					}
					else
					{
						paste_move_entries(
							std::move(data.data.data),
							current.data->data,
							data.sex,
							dstSex,
							GetConfigClass());
					}

					OnUpdate(handle, { *current.data });
				});
		}

		template <class T>
		template <class Tp>
		NodeOverrideCommonResult UINodeOverrideEditorWidget<T>::DrawOverrideConditionContextMenu(
			T                                    a_handle,
			entryNodeOverrideData_t&             a_data,
			Data::configNodeOverrideCondition_t& a_entry,
			const Tp&                            a_params,
			const bool                           a_exists,
			const bool                           a_ignoreNode)
		{
			NodeOverrideCommonResult result;

			ImGui::PushID("oo_offset_match_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_ooNewEntryID      = {};
				m_ooNewEntryIDKW    = {};
				m_ooNewEntryIDRace  = {};
				m_ooNewEntryIDActor = {};
				m_ooNewEntryIDNPC   = {};
				m_ooNewEntryIDGlob  = {};
				m_ooNewBiped        = BIPED_OBJECT::kNone;
				m_ooNewSlot         = Data::ObjectSlotExtra::kNone;
				m_ooNewExtraCond    = Data::ExtraConditionType::kNone;
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

			//ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if constexpr (std::is_same_v<Tp, SingleNodeOverrideTransformUpdateParams>)
					{
						if (!a_ignoreNode)
						{
							if (LCG_BM(CommonStrings::Node, "2"))
							{
								stl::fixed_string c;

								if (UICMNodeSelectorWidget::DrawCMNodeSelector(
										"##node_sel",
										c,
										NodeOverrideData::GetCMENodeData(),
										std::addressof(a_params.name)))
								{
									result.action    = NodeOverrideCommonAction::Insert;
									result.str       = std::move(c);
									result.matchType = Data::NodeOverrideConditionType::Node;

									ImGui::CloseCurrentPopup();
								}

								ImGui::EndMenu();
							}
						}
					}

					if (LCG_BM(CommonStrings::Item, "3"))
					{
						if (LCG_BM(CommonStrings::Form, "4"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_condParamEditor.GetFormPicker().DrawFormSelector(
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									result.action    = NodeOverrideCommonAction::Insert;
									result.form      = m_ooNewEntryID;
									result.matchType = Data::NodeOverrideConditionType::Form;
								}

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						if (LCG_BM(CommonStrings::Keyword, "5"))
						{
							if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									result.action    = NodeOverrideCommonAction::Insert;
									result.form      = m_ooNewEntryIDKW;
									result.matchType = Data::NodeOverrideConditionType::Keyword;
								}

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(UIWidgetCommonStrings::BipedSlot, "6"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								LS(CommonStrings::Biped, "bp"),
								m_ooNewBiped))
						{
							result.action    = NodeOverrideCommonAction::Insert;
							result.biped     = m_ooNewBiped;
							result.matchType = Data::NodeOverrideConditionType::BipedSlot;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Type, "7"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								LS(CommonStrings::Type, "ss"),
								m_ooNewSlot))
						{
							result.action    = NodeOverrideCommonAction::Insert;
							result.type      = m_ooNewSlot;
							result.matchType = Data::NodeOverrideConditionType::Type;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Actor, "8"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Actor);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDActor))
						{
							if (m_ooNewEntryIDActor)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDActor;
								result.matchType = Data::NodeOverrideConditionType::Actor;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::NPC, "9"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::NPC);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDNPC))
						{
							if (m_ooNewEntryIDNPC)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDNPC;
								result.matchType = Data::NodeOverrideConditionType::NPC;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (LCG_MI(CommonStrings::Race, "A"))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Race;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Furniture, "B")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Furniture;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Location, "C")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Location;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Worldspace, "D")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Worldspace;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Package, "E")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Package;
					}

					if (ImGui::MenuItem(LS(CommonStrings::Weather, "F")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Weather;
					}

					if (LCG_BM(CommonStrings::Global, "G"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDGlob))
						{
							if (m_ooNewEntryIDGlob)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDGlob;
								result.matchType = Data::NodeOverrideConditionType::Global;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(LS(UIWidgetCommonStrings::Mount, "H")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Mount;
					}

					if (LCG_BM(CommonStrings::Extra, "Y"))
					{
						if (m_condParamEditor.DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							if (m_ooNewExtraCond != Data::ExtraConditionType::kNone)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.excond    = m_ooNewExtraCond;
								result.matchType = Data::NodeOverrideConditionType::Extra;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(LS(CommonStrings::Group, "Z")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Group;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "2")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::HandleValueUpdate(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			const SingleNodeOverrideTransformUpdateParams& a_params,
			const bool                                     a_exists)
		{
			if (!a_exists)
			{
				auto r = a_data.data.insert_or_assign(a_params.name, a_params.entry);

				OnUpdate(a_handle, { a_params.sex, r.first->first, r.first->second });
			}
			else
			{
				OnUpdate(a_handle, a_params);
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::HandleValueUpdate(
			T                                              a_handle,
			entryNodeOverrideData_t&                       a_data,
			const SingleNodeOverridePlacementUpdateParams& a_params,
			const bool                                     a_exists)
		{
			if (!a_exists)
			{
				auto r = a_data.placementData.insert_or_assign(a_params.name, a_params.entry);

				OnUpdate(a_handle, { a_params.sex, r.first->first, r.first->second });
			}
			else
			{
				OnUpdate(a_handle, a_params);
			}
		}

		template <class T>
		bool UINodeOverrideEditorWidget<T>::DrawConditionParamExtra(void* a_p1, const void*)
		{
			auto match = static_cast<Data::configNodeOverrideCondition_t*>(a_p1);

			ImGui::PushID("match_param_extra");

			bool result = false;

			switch (match->fbf.type)
			{
			case Data::NodeOverrideConditionType::BipedSlot:

				result |= ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorWidgetStrings::MatchSkin, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag2));

				DrawTip(UITip::MatchSkin);

				result |= ImGui::CheckboxFlagsT(
					"!##2",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UINodeOverrideEditorWidgetStrings::IsBolt, "3"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

				break;
			case Data::NodeOverrideConditionType::Form:
			case Data::NodeOverrideConditionType::Keyword:
			case Data::NodeOverrideConditionType::Type:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Equipped, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchEquipped));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Or, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::Displayed, "3"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchSlots));

				DrawTip(UITip::EquippedConditions);

				if (!match->flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
					ImGui::TextWrapped("%s", LS(UINodeOverrideEditorWidgetStrings::CondMatchWarn));
					ImGui::PopStyleColor();
				}

				break;

			case Data::NodeOverrideConditionType::Node:

				{
					bool disabled = match->bipedSlot < BIPED_OBJECT::kTotal;

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						LS(UIWidgetCommonStrings::IgnoreScabbards, "1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag0));

					UICommon::PopDisabled(disabled);
				}

				break;

			case Data::NodeOverrideConditionType::Furniture:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::LayingDown, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

				break;

			case Data::NodeOverrideConditionType::Location:

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

				DrawTip(UITip::MatchChildLoc);

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchEither, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR));

				DrawTip(UITip::MatchEitherFormKW);

				break;

			case Data::NodeOverrideConditionType::Worldspace:

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

				DrawTip(UITip::MatchWorldspaceParent);

				break;

			case Data::NodeOverrideConditionType::Race:

				result |= ImGui::CheckboxFlagsT(
					"!##1",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::IsPlayable, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

				result |= ImGui::CheckboxFlagsT(
					"!##3",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch4));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::ChildRace, "4"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag2));

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		bool UINodeOverrideEditorWidget<T>::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			auto match = static_cast<Data::configNodeOverrideCondition_t*>(a_args.p3);

			bool result = false;

			ImGui::PushID("match_item_extra");

			switch (match->fbf.type)
			{
			case Data::NodeOverrideConditionType::BipedSlot:
			case Data::NodeOverrideConditionType::Type:
			case Data::NodeOverrideConditionType::Furniture:
			case Data::NodeOverrideConditionType::Location:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					if (match->fbf.type == Data::NodeOverrideConditionType::BipedSlot &&
					    match->flags.test(Data::NodeOverrideConditionFlags::kExtraFlag2))
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
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Data::NodeOverrideConditionType::Form:

				if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;
			case Data::NodeOverrideConditionType::Race:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;

			case Data::NodeOverrideConditionType::Extra:

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
							stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

						ImGui::SameLine();

						a_args.hide = false;

						break;
					default:
						a_args.hide = true;
						break;
					}
				}

				break;

			case Data::NodeOverrideConditionType::Weather:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::WeatherClass)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;
			case Data::NodeOverrideConditionType::Mount:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Race)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::UpdateMatchParamAllowedTypes(
			Data::NodeOverrideConditionType a_type)
		{
			switch (a_type)
			{
			case Data::NodeOverrideConditionType::Keyword:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Keyword));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Race:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Furniture:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Furniture));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Actor:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Data::NodeOverrideConditionType::NPC:
			case Data::NodeOverrideConditionType::Mount:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Location:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Location));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Worldspace:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Worldspace));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Package:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Package));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Weather:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Weather));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::NodeOverrideConditionType::Global:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Global));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			default:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.form_common);
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			}
		}

	}
}