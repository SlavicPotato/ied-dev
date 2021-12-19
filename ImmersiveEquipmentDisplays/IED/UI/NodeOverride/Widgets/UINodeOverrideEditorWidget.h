#pragma once

#include "../../UIClipboard.h"
#include "../../UICommon.h"
#include "../../UIEditorInterface.h"
#include "../../UIFormLookupInterface.h"
#include "../../UILocalizationInterface.h"
#include "../../UISettingsInterface.h"
#include "../../Widgets/Filters/UIGenericFilter.h"
#include "../../Widgets/Form/UIFormSelectorWidget.h"
#include "../../Widgets/UIBipedObjectSelectorWidget.h"
#include "../../Widgets/UICMNodeSelector.h"
#include "../../Widgets/UIConditionParamEditorWidget.h"
#include "../../Widgets/UIDescriptionPopup.h"
#include "../../Widgets/UIEditorPanelSettings.h"
#include "../../Widgets/UIObjectTypeSelectorWidget.h"
#include "../../Widgets/UIPopupToggleButtonWidget.h"
#include "../../Widgets/UITransformSliderWidget.h"
#include "../../Widgets/UIWidgetsCommon.h"

#include "UINodeOverrideEditorStrings.h"

#include "IED/Controller/NodeOverrideData.h"

#include "IED/ConfigOverride.h"
#include "IED/Controller/NodeOverrideData.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct SingleNodeOverrideUpdateParams
		{
			Data::ConfigSex sex;
			stl::fixed_string name;
			Data::configNodeOverrideEntry_t& entry;
		};

		struct SingleNodeOverrideParentUpdateParams
		{
			Data::ConfigSex sex;
			stl::fixed_string name;
			Data::configNodeOverrideEntryParent_t& entry;
		};

		struct ClearNodeOverrideUpdateParams
		{
			stl::fixed_string name;
			entryNodeOverrideData_t& entry;
		};

		struct NodeOverrideUpdateParams
		{
			Data::configNodeOverrideHolder_t& data;
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

			kDrawNodePlacement = 1u << 0,
			kUnrestrictedNodePlacement = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideEditorFlags);

		struct NodeOverrideCommonResult
		{
			NodeOverrideCommonAction action{ NodeOverrideCommonAction::None };
			stl::fixed_string str;
			const std::string* desc;
			Data::NodeOverrideConditionType matchType;
			Game::FormID form;
			SwapDirection dir;
			bool isGroup;
			Biped::BIPED_OBJECT biped;
			Data::ObjectSlot slot;
		};

		struct NodeOverridePlacementOverrideResult
		{
			NodeOverrideCommonAction action{ NodeOverrideCommonAction::None };
			const std::string* desc;
			SwapDirection dir;
		};

		template <class T>
		class UINodeOverrideEditorWidget :
			public UIEditorInterface,
			public UIEditorPanelSettings,
			public UISettingsInterface,
			public UICollapsibles,
			public UIDescriptionPopupWidget,
			public UIFormLookupInterface,
			UIConditionParamExtraInterface,
			public virtual UITransformSliderWidget,
			public virtual UIPopupToggleButtonWidget,
			public virtual UILocalizationInterface
		{
		public:
			UINodeOverrideEditorWidget(
				Controller& a_controller);

			void DrawNodeOverrideEditor(
				T a_handle,
				entryNodeOverrideData_t& a_data);

		protected:
			struct NodeOverrideEditorCurrentData
			{
				T handle;
				entryNodeOverrideData_t* data;
			};

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

		private:
			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				entryNodeOverrideData_t& a_data);

			virtual void DrawExtraEditorPanelSettings() override;

			void DrawItemFilter();

			void DrawTransformPanel(
				T a_handle,
				entryNodeOverrideData_t& a_data);

			void DrawPlacementPanel(
				T a_handle,
				entryNodeOverrideData_t& a_data);

			Data::configNodeOverrideHolder_t::transform_data_type::iterator DrawTransformEntryContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const stl::fixed_string& a_name,
				Data::configNodeOverrideHolder_t::transform_data_type::iterator a_it);

			Data::configNodeOverrideHolder_t::placement_data_type::iterator DrawPlacementEntryContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const stl::fixed_string& a_name,
				Data::configNodeOverrideHolder_t::placement_data_type::iterator a_it);

			void DrawTransformEntry(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			void DrawPlacementEntry(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			void DrawPlacementValues(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverridePlacementValues_t& a_values,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			void DrawValueFlags(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideValues_t& a_values,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			NodeOverrideCommonAction DrawTransformHeaderContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configTransform_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			NodeOverrideCommonAction DrawOverrideOffsetListContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffsetList_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			NodeOverrideCommonAction DrawOverridePlacementOverrideListContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			void DrawOverrideOffsets(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffsetList_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists,
				Data::configNodeOverrideOffset_t* a_parent);

			void DrawOverridePlacementOverrides(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			void DrawOverrideOffsetList(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffsetList_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			void DrawOverridePlacementOverrideList(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverridePlacementOverrideList_t& a_entry,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			template <class Tp>
			void DrawOverrideConditionTree(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp& a_params,
				const bool a_exists);

			NodeOverrideCommonResult DrawOverrideOffsetContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffset_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			NodeOverridePlacementOverrideResult DrawOverridePlacementOverrideContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverridePlacementOverride_t& a_entry,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			template <class Tp>
			NodeOverrideCommonAction DrawOverrideConditionHeaderContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp& a_params,
				const bool a_exists);

			template <class Tp>
			void DrawOverrideConditionTable(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const Tp& a_params,
				const bool a_exists);

			void DrawOverrideOffsetAdjust(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffset_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			/*void DrawOverridePosClamp(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideOffset_t& a_entry,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);*/

			template <class Tp>
			NodeOverrideCommonResult DrawOverrideConditionContextMenu(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				Data::configNodeOverrideCondition_t& a_entry,
				const Tp& a_params,
				const bool a_exists);

			virtual void OnUpdate(
				T a_handle,
				const SingleNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T a_handle,
				const SingleNodeOverrideParentUpdateParams& a_params) = 0;

			virtual void OnUpdate(
				T a_handle,
				const NodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClear(
				T a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			virtual void OnClearParent(
				T a_handle,
				const ClearNodeOverrideUpdateParams& a_params) = 0;

			void HandleValueUpdate(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const SingleNodeOverrideUpdateParams& a_params,
				const bool a_exists);

			void HandleValueUpdate(
				T a_handle,
				entryNodeOverrideData_t& a_data,
				const SingleNodeOverrideParentUpdateParams& a_params,
				const bool a_exists);

			virtual NodeOverrideEditorCurrentData GetCurrentData() = 0;

			virtual bool DrawConditionParamExtra(
				void* a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem a_item,
				void* a_p1,
				const void* a_p2,
				void* a_p3) override;

			void UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType a_type);

			//Game::FormID m_ooEditEntryID;
			Game::FormID m_ooNewEntryID;
			Game::FormID m_ooNewEntryIDKW;
			Game::FormID m_ooNewEntryIDRace;

			/*UIFormSelectorWidget m_formSelector;
			UIFormSelectorWidget m_formSelectorKW;*/

			Biped::BIPED_OBJECT m_ooNewBiped{ Biped::BIPED_OBJECT::kNone };
			Data::ObjectSlot m_ooNewSlot{ Data::ObjectSlot::kMax };

			UIConditionParamEditorWidget m_matchParamEditor;

			struct
			{
				std::shared_ptr<const UIFormBrowser::tab_filter_type> form_common;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> furniture;
				std::shared_ptr<const UIFormBrowser::tab_filter_type> race;
			} m_type_filters;

			char m_buffer1[260]{ 0 };

			UIGenericFilter m_itemFilter;
		};

		template <class T>
		UINodeOverrideEditorWidget<T>::UINodeOverrideEditorWidget(
			Controller& a_controller) :
			UISettingsInterface(a_controller),
			UIFormLookupInterface(a_controller),
			UIEditorPanelSettings(a_controller),
			UILocalizationInterface(a_controller),
			m_itemFilter(true),
			m_matchParamEditor(a_controller)
		{
			m_type_filters.form_common = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESAmmo::kTypeID,
			      TESObjectLIGH::kTypeID,
			      IFormDatabase::EXTRA_TYPE_ARMOR });

			m_type_filters.furniture = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESFurniture::kTypeID });

			m_type_filters.race = std::make_unique<
				UIFormBrowser::tab_filter_type,
				std::initializer_list<UIFormBrowser::tab_filter_type::value_type>>(
				{ TESRace::kTypeID });

			m_matchParamEditor.SetExtraInterface(this);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawNodeOverrideEditor(
			T a_handle,
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
			ImGui::Spacing();

			auto& flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			if (flags.test(NodeOverrideEditorFlags::kDrawNodePlacement))
			{
				if (ImGui::BeginChild(
						"ep_editor_panel",
						{ -1.0f, 0.0f },
						false))
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
			T a_handle,
			entryNodeOverrideData_t& a_data)
		{
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawExtraEditorPanelSettings()
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			auto& flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			if (ImGui::RadioButton(
					LS(CommonStrings::Transforms, "1"),
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
						LS(UINodeOverrideEditorStrings::UnrestrictedPlacement, "3"),
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
		void UINodeOverrideEditorWidget<T>::DrawTransformPanel(
			T a_handle,
			entryNodeOverrideData_t& a_data)
		{
			auto& data = OverrideNodeInfo::GetCMENodeData();

			bool first = true;

			Data::configStoreNodeOverride_t::holderCache_t hc;

			auto configClass = GetConfigClass();

			auto df = data.getvec().begin();

			for (auto e : data.getvec())
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
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				else
				{
					if (configClass != Data::ConfigClass::Global)
					{
						svar = !GetConfigStore().transforms.HasCMEClass(a_handle, configClass, e->first, hc);

						if (svar)
						{
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.75f);
						}
					}
				}

				it = DrawTransformEntryContextMenu(a_handle, a_data, e->first, it);

				ImGui::PushID("entry_area");

				if (TreeEx("tree", e->first == (*df)->first, "%s", e->second.desc.c_str()))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.data.end())
					{
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
				ImGui::PopID();

				first = false;
			}
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawPlacementPanel(
			T a_handle,
			entryNodeOverrideData_t& a_data)
		{
			auto& data = OverrideNodeInfo::GetWeaponNodeData();

			bool first = true;

			Data::configStoreNodeOverride_t::holderCache_t hc;

			auto configClass = GetConfigClass();

			for (auto e : data.getvec())
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
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				else
				{
					if (configClass != Data::ConfigClass::Global)
					{
						svar = !GetConfigStore().transforms.HasPlacementClass(a_handle, configClass, e->first, hc);

						if (svar)
						{
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.75f);
						}
					}
				}

				it = DrawPlacementEntryContextMenu(a_handle, a_data, e->first, it);

				ImGui::PushID("entry_area");

				if (TreeEx("tree", true, "%s", e->second.desc.c_str()))
				{
					ImGui::Spacing();

					ImGui::Indent();

					if (it != a_data.placementData.end())
					{
						DrawPlacementEntry(a_handle, a_data, { GetSex(), e->first, it->second }, true);
					}
					else
					{
						decltype(it->second) tmp;

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
				ImGui::PopID();

				first = false;
			}
		}

		template <class T>
		auto UINodeOverrideEditorWidget<T>::DrawTransformEntryContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const stl::fixed_string& a_name,
			Data::configNodeOverrideHolder_t::transform_data_type::iterator a_it)
			-> Data::configNodeOverrideHolder_t::transform_data_type::iterator
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });

			ImGui::PushID("context_area");

			DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						LS(CommonStrings::Clear, "1"),
						nullptr,
						false,
						a_it != a_data.data.end()))
				{
					if (a_it != a_data.data.end())
					{
						a_data.data.erase(a_it);

						OnClear(a_handle, { a_name, a_data });

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

						auto& data = a_it->second(sex);

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
						UIClipboard::Set(a_it->second(GetSex()));
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverride_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					bool update = false;

					if (clipData)
					{
						auto sex = GetSex();

						if (!has)
						{
							a_it = a_data.data.try_emplace(a_name).first;
						}

						a_it->second(sex) = *clipData;

						OnUpdate(a_handle, { sex, a_name, a_it->second });
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			ImGui::PopStyleVar();

			return a_it;
		}

		template <class T>
		Data::configNodeOverrideHolder_t::placement_data_type::iterator UINodeOverrideEditorWidget<T>::DrawPlacementEntryContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const stl::fixed_string& a_name,
			Data::configNodeOverrideHolder_t::placement_data_type::iterator a_it)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });

			ImGui::PushID("context_area");

			DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						LS(CommonStrings::Clear, "1"),
						nullptr,
						false,
						a_it != a_data.placementData.end()))
				{
					if (a_it != a_data.placementData.end())
					{
						a_data.placementData.erase(a_it);

						OnClearParent(a_handle, { a_name, a_data });

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
						UIClipboard::Set(a_it->second(GetSex()));
					}
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverridePlacement_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					bool update = false;

					if (clipData)
					{
						auto sex = GetSex();

						if (!has)
						{
							a_it = a_data.placementData.try_emplace(a_name).first;
						}

						a_it->second(sex) = *clipData;

						OnUpdate(a_handle, { sex, a_name, a_it->second });
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			ImGui::PopStyleVar();

			return a_it;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawTransformEntry(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			auto& data = a_params.entry(a_params.sex);

			if (DrawTransformHeaderContextMenu(
					a_handle,
					a_data,
					data.transform,
					a_params,
					a_exists) == NodeOverrideCommonAction::PasteOver)
			{
				ImGui::SetNextItemOpen(true);
			}

			if (TreeEx("tree1", true, "%s", LS(CommonStrings::Transform)))
			{
				ImGui::Spacing();

				//DrawValueFlags(a_handle, a_data, data, a_params, a_exists);

				DrawTransformSliders(data.transform, [&](auto a_v) {
					HandleValueUpdate(
						a_handle,
						a_data,
						a_params,
						a_exists);
				});

				ImGui::Spacing();

				ImGui::TreePop();
			}

			if (TreeEx("tree2", true, "%s", LS(CommonStrings::Visibility)))
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
					a_exists);

				UICommon::PopDisabled(disabled);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			DrawOverrideOffsets(a_handle, a_data, data.offsets, a_params, a_exists, nullptr);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawPlacementEntry(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("pl_entry");

			DrawPlacementValues(
				a_handle,
				a_data,
				data,
				a_params,
				a_exists);

			ImGui::Spacing();

			/*DrawOverrideConditionTree(
				a_handle,
				a_data,
				data.matches,
				a_params,
				a_exists);*/

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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverridePlacementValues_t& a_values,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
		{
			auto& flags = GetEditorPanelSettings().get_flags<NodeOverrideEditorFlags>();

			const OverrideNodeInfo::weaponNodeEntry_t* entry;

			if (!flags.test(NodeOverrideEditorFlags::kUnrestrictedNodePlacement))
			{
				auto& nodedata = OverrideNodeInfo::GetWeaponNodeData();

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
					std::addressof(OverrideNodeInfo::GetMOVNodeData()));
			}
			else
			{
				result = UICMNodeSelectorWidget::DrawCMNodeSelector(
					LS(UIWidgetCommonStrings::TargetNode, "node_sel"),
					a_values.targetNode,
					OverrideNodeInfo::GetMOVNodeData());
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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideOffsetList_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists,
			Data::configNodeOverrideOffset_t* a_parent)
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

			if (TreeEx("offsets", true, "%s", LS(CommonStrings::Offsets)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawOverrideOffsetList(a_handle, a_data, a_entry, a_params, a_exists);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrides(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideValues_t& a_values,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			/*ImGui::PushID("value_flags");

			if (ImGui::CheckboxFlagsT(
					"Invisible##ctl",
					stl::underlying(std::addressof(a_values.flags.value)),
					stl::underlying(Data::NodeOverrideValuesFlags::kInvisible)))
			{
				HandleValueUpdate(
					a_handle,
					a_data,
					a_params,
					a_exists);
			}

			ImGui::PopID();*/
		}

		template <class T>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawTransformHeaderContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configTransform_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("xfrm_tree_ctx");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(LS(CommonStrings::Copy, "1")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipData = UIClipboard::Get<Data::configTransform_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "2"),
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

			ImGui::PopID();

			return result;
		}

		template <class T>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverrideOffsetListContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideOffsetList_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("list_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

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

							ImGui::CloseCurrentPopup();

							result = NodeOverrideCommonAction::Insert;
						}
						ImGui::EndMenu();
					}

					if (LCG_BM(CommonStrings::Group, "3"))
					{
						if (DrawDescriptionPopup())
						{
							auto& e = a_entry.emplace_back();
							e.description = GetDescriptionPopupBuffer();
							e.offsetFlags.set(Data::NodeOverrideOffsetFlags::kIsGroup);

							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);

							ImGui::CloseCurrentPopup();

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

			ImGui::PopID();

			return result;
		}

		template <class T>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrideListContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("op_list_tree_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

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

						ImGui::CloseCurrentPopup();

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

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOverrideOffsetList(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideOffsetList_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			if (a_entry.empty())
			{
				return;
			}

			ImGui::PushID("offset_list");

			std::size_t i = 0;

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
					it = a_entry.emplace(it);
					it->description = *result.desc;
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
					it->description = *result.desc;
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

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::TreeNodeEx(
							"oo_item",
							ImGuiTreeNodeFlags_SpanAvailWidth |
								ImGuiTreeNodeFlags_DefaultOpen,
							"%s",
							e.description.c_str()))
					{
						if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kIsGroup))
						{
							ImGui::PushID("group");

							ImGui::Spacing();

							if (ImGui::CheckboxFlagsT(
									LS(CommonStrings::Continue, "1"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kContinue)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							DrawOverrideOffsets(a_handle, a_data, e.group, a_params, a_exists, std::addressof(e));

							ImGui::PopID();
						}
						else
						{
							ImGui::Spacing();

							DrawOverrideConditionTree(
								a_handle,
								a_data,
								e.matches,
								a_params,
								a_exists);

							if (ImGui::CheckboxFlagsT(
									LS(CommonStrings::Continue, "1"),
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
									LS(UIWidgetCommonStrings::RequiresMatchList, "2"),
									stl::underlying(std::addressof(e.offsetFlags.value)),
									stl::underlying(Data::NodeOverrideOffsetFlags::kRequiresConditionList)))
							{
								HandleValueUpdate(
									a_handle,
									a_data,
									a_params,
									a_exists);
							}

							if (ImGui::CheckboxFlagsT(
									LS(UINodeOverrideEditorStrings::WeaponAdjust, "3"),
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
									LS(UINodeOverrideEditorStrings::WeightAdjust, "4"),
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
										LS(UINodeOverrideEditorStrings::UseAdjustsFromMatched, "5"),
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
									LS(UINodeOverrideEditorStrings::AccumulatePosition, "6"),
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
									LS(UINodeOverrideEditorStrings::LockToAccum, "7"),
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
								/*if (ImGui::CheckboxFlagsT(
										"Absolute rotation##ctl",
										stl::underlying(std::addressof(e.offsetFlags.value)),
										stl::underlying(Data::NodeOverrideOffsetFlags::kAbsoluteRotation)))
								{
									HandleValueUpdate(
										a_handle,
										a_data,
										a_params,
										a_exists);
								}*/

								ImGui::Spacing();

								if (DrawTransformHeaderContextMenu(
										a_handle,
										a_data,
										e.transform,
										a_params,
										a_exists) == NodeOverrideCommonAction::PasteOver)
								{
									ImGui::SetNextItemOpen(true);
								}

								if (ImGui::TreeNodeEx(
										"xtree",
										ImGuiTreeNodeFlags_SpanAvailWidth |
											ImGuiTreeNodeFlags_DefaultOpen,
										"%s",
										LS(CommonStrings::Transform)))
								{
									DrawTransformSliders(e.transform, [&](auto a_v) {
										HandleValueUpdate(
											a_handle,
											a_data,
											a_params,
											a_exists);
									});

									ImGui::TreePop();
								}
							}
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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverridePlacementOverrideList_t& a_entry,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
		{
			if (a_entry.empty())
			{
				return;
			}

			ImGui::PushID("override_list");

			std::size_t i = 0;

			auto it = a_entry.begin();

			while (it != a_entry.end())
			{
				ImGui::PushID(i);

				const auto result = DrawOverridePlacementOverrideContextMenu(a_handle, a_data, *it, a_params, a_exists);

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
					it = a_entry.emplace(it);
					it->description = *result.desc;
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
					it->description = *result.desc;
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
							e.matches,
							a_params,
							a_exists);

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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp& a_params,
			const bool a_exists)
		{
			ImGui::PushID("override_match_tree");

			const auto result = DrawOverrideConditionHeaderContextMenu(
				a_handle,
				a_data,
				a_entry,
				a_params,
				a_exists);

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
						a_exists);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
		}

		template <class T>
		NodeOverrideCommonResult UINodeOverrideEditorWidget<T>::DrawOverrideOffsetContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideOffset_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonResult result;

			ImGui::PushID("oo_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
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
				result.dir = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

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
								result.action = NodeOverrideCommonAction::Insert;
								result.desc = std::addressof(GetDescriptionPopupBuffer());
								result.isGroup = true;

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						if (LCG_BM(CommonStrings::Item, "4"))
						{
							if (DrawDescriptionPopup())
							{
								result.action = NodeOverrideCommonAction::Insert;
								result.desc = std::addressof(GetDescriptionPopupBuffer());
								result.isGroup = false;

								ImGui::CloseCurrentPopup();
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

						ImGui::CloseCurrentPopup();
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
						result.desc = std::addressof(GetDescriptionPopupBuffer());

						ImGui::CloseCurrentPopup();
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

					if (auto clipData = UIClipboard::Get<Data::configNodeOverrideOffset_t>())
					{
						a_entry = *clipData;

						update = true;
					}
					else if (auto clipData = UIClipboard::Get<Data::configNodeOverrideValues_t>())
					{
						static_cast<Data::configNodeOverrideValues_t&>(a_entry) = *clipData;

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

			ImGui::PopID();

			return result;
		}

		template <class T>
		NodeOverridePlacementOverrideResult UINodeOverrideEditorWidget<T>::DrawOverridePlacementOverrideContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverridePlacementOverride_t& a_entry,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
		{
			NodeOverridePlacementOverrideResult result;

			ImGui::PushID("oo_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
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
				result.dir = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "2"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = NodeOverrideCommonAction::Insert;
							result.desc = std::addressof(GetDescriptionPopupBuffer());

							ImGui::CloseCurrentPopup();
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
						result.desc = std::addressof(GetDescriptionPopupBuffer());

						ImGui::CloseCurrentPopup();
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

					if (auto clipData = UIClipboard::Get<Data::configNodeOverridePlacementOverride_t>())
					{
						a_entry = *clipData;

						update = true;
					}
					else if (auto clipData = UIClipboard::Get<Data::configNodeOverridePlacementValues_t>())
					{
						static_cast<Data::configNodeOverridePlacementValues_t&>(a_entry) = *clipData;

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

			ImGui::PopID();

			return result;
		}

		template <class T>
		template <class Tp>
		NodeOverrideCommonAction UINodeOverrideEditorWidget<T>::DrawOverrideConditionHeaderContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("oo_matchlist_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				m_ooNewEntryID = {};
				m_ooNewEntryIDKW = {};
				m_ooNewEntryIDRace = {};
				m_ooNewSlot = Data::ObjectSlot::kMax;
				m_ooNewBiped = Biped::BIPED_OBJECT::kNone;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if constexpr (!std::is_same_v<Tp, SingleNodeOverrideParentUpdateParams>)
					{
						if (LCG_BM(CommonStrings::Node, "2"))
						{
							stl::fixed_string c;

							if (UICMNodeSelectorWidget::DrawCMNodeSelector(
									"##node_sel",
									c,
									OverrideNodeInfo::GetCMENodeData(),
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

					if (LCG_BM(CommonStrings::Item, "3"))
					{
						if (LCG_BM(CommonStrings::Form, "4"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
									LS(CommonStrings::Form, "fs"),
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									a_entry.emplace_back(m_ooNewEntryID, Data::NodeOverrideConditionType::Form);

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

						if (LCG_BM(CommonStrings::Keyword, "5"))
						{
							if (m_matchParamEditor.GetKeywordPicker().DrawFormSelector(
									LS(CommonStrings::Form, "fs"),
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									a_entry.emplace_back(m_ooNewEntryIDKW, Data::NodeOverrideConditionType::Keyword);

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

					if (LCG_BM(UIWidgetCommonStrings::BipedSlot, "6"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								LS(CommonStrings::Biped, "bp"),
								m_ooNewBiped))
						{
							if (m_ooNewBiped != Biped::BIPED_OBJECT::kNone)
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

					if (LCG_BM(UIWidgetCommonStrings::EquipmentDisplay, "7"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								LS(CommonStrings::Type, "ss"),
								m_ooNewSlot))
						{
							if (m_ooNewSlot != Data::ObjectSlot::kMax)
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

					if (LCG_BM(CommonStrings::Race, "8"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Race);

						if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
								LS(CommonStrings::Form, "fs"),
								m_ooNewEntryIDRace))
						{
							if (m_ooNewEntryIDRace)
							{
								a_entry.emplace_back(
									m_ooNewEntryIDRace,
									Data::NodeOverrideConditionType::Race);

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

					if (ImGui::MenuItem(LS(CommonStrings::Furniture, "9")))
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

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Clear, "A")))
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

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "B")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideConditionList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "C"),
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

			ImGui::PopID();

			return result;
		}

		template <class T>
		template <class Tp>
		void UINodeOverrideEditorWidget<T>::DrawOverrideConditionTable(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const Tp& a_params,
			const bool a_exists)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.f, 5.f });

			constexpr int NUM_COLUMNS = 5;

			float width;

			if constexpr (std::is_same_v<Tp, SingleNodeOverrideParentUpdateParams>)
			{
				width = -1.0f;
			}
			else
			{
				width = ImGui::GetFontSize() * 29.0f;
			}

			if (ImGui::BeginTable(
					"offset_match_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ width, 0.f }))
			{
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4.0f);
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

					//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawOverrideConditionContextMenu(
						a_handle,
						a_data,
						*it,
						a_params,
						a_exists);

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
						case Data::NodeOverrideConditionType::Race:

							it = a_entry.emplace(
								it,
								result.form,
								result.matchType);

							break;
						case Data::NodeOverrideConditionType::BipedSlot:

							it = a_entry.emplace(
								it,
								result.biped);

							break;
						case Data::NodeOverrideConditionType::EquipmentSlot:

							it = a_entry.emplace(
								it,
								result.slot);

							break;

						case Data::NodeOverrideConditionType::Furniture:

							it = a_entry.emplace(
								it,
								result.matchType);

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

						m_matchParamEditor.Reset();

						UpdateMatchParamAllowedTypes(e.fbf.type);

						const char* tdesc;
						const char* vdesc;

						switch (e.fbf.type)
						{
						case Data::NodeOverrideConditionType::Node:

							m_matchParamEditor.SetNext<ConditionParamItem::CMENode>(
								e.node,
								a_params.name);

							tdesc = LS(CommonStrings::Node);
							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::CMENode);

							break;
						case Data::NodeOverrideConditionType::Form:

							m_matchParamEditor.SetNext<ConditionParamItem::Form>(
								e.form.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);

							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Form);
							tdesc = LS(CommonStrings::Form);

							break;
						case Data::NodeOverrideConditionType::Keyword:

							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);

							tdesc = LS(CommonStrings::Keyword);
							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Keyword);

							break;
						case Data::NodeOverrideConditionType::BipedSlot:

							m_matchParamEditor.SetNext<ConditionParamItem::BipedSlot>(
								e.bipedSlot);
							m_matchParamEditor.SetNext<ConditionParamItem::Form>(
								e.form.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);

							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::BipedSlot);
							tdesc = LS(CommonStrings::Biped);

							break;
						case Data::NodeOverrideConditionType::EquipmentSlot:

							m_matchParamEditor.SetNext<ConditionParamItem::EquipmentSlot>(
								e.equipmentSlot);
							m_matchParamEditor.SetNext<ConditionParamItem::Form>(
								e.form.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);

							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlot);
							tdesc = LS(CommonStrings::Display);

							break;
						case Data::NodeOverrideConditionType::Race:

							m_matchParamEditor.SetNext<ConditionParamItem::Form>(
								e.form.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);

							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Form);
							tdesc = LS(CommonStrings::Race);

							break;
						case Data::NodeOverrideConditionType::Furniture:

							m_matchParamEditor.SetNext<ConditionParamItem::Extra>(
								e);
							m_matchParamEditor.SetNext<ConditionParamItem::Form>(
								e.form.get_id());
							m_matchParamEditor.SetNext<ConditionParamItem::Keyword>(
								e.keyword.get_id());

							vdesc = m_matchParamEditor.GetItemDesc(ConditionParamItem::Furniture);
							tdesc = LS(CommonStrings::Furniture);

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
							HandleValueUpdate(
								a_handle,
								a_data,
								a_params,
								a_exists);
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

					//ImGui::PopStyleVar();

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::DrawOverrideOffsetAdjust(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideOffset_t& a_entry,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
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
		template <class Tp>
		NodeOverrideCommonResult UINodeOverrideEditorWidget<T>::DrawOverrideConditionContextMenu(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			Data::configNodeOverrideCondition_t& a_entry,
			const Tp& a_params,
			const bool a_exists)
		{
			NodeOverrideCommonResult result;

			ImGui::PushID("oo_offset_match_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				m_ooNewEntryID = {};
				m_ooNewEntryIDKW = {};
				m_ooNewEntryIDRace = {};
				m_ooNewBiped = Biped::BIPED_OBJECT::kNone;
				m_ooNewSlot = Data::ObjectSlot::kMax;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if constexpr (!std::is_same_v<Tp, SingleNodeOverrideParentUpdateParams>)
					{
						if (LCG_BM(CommonStrings::Node, "2"))
						{
							stl::fixed_string c;

							if (UICMNodeSelectorWidget::DrawCMNodeSelector(
									"##node_sel",
									c,
									OverrideNodeInfo::GetCMENodeData(),
									std::addressof(a_params.name)))
							{
								result.action = NodeOverrideCommonAction::Insert;
								result.str = std::move(c);
								result.matchType = Data::NodeOverrideConditionType::Node;

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}
					}

					if (LCG_BM(CommonStrings::Item, "3"))
					{
						if (LCG_BM(CommonStrings::Insert, "4"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
									LS(CommonStrings::Form, "fs"),
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									result.action = NodeOverrideCommonAction::Insert;
									result.form = m_ooNewEntryID;
									result.matchType = Data::NodeOverrideConditionType::Form;
								}

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						if (LCG_BM(CommonStrings::Keyword, "5"))
						{
							if (m_matchParamEditor.GetKeywordPicker().DrawFormSelector(
									LS(CommonStrings::Form, "fs"),
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									result.action = NodeOverrideCommonAction::Insert;
									result.form = m_ooNewEntryIDKW;
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
							if (m_ooNewBiped != Biped::BIPED_OBJECT::kNone)
							{
								result.action = NodeOverrideCommonAction::Insert;
								result.biped = m_ooNewBiped;
								result.matchType = Data::NodeOverrideConditionType::BipedSlot;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (LCG_BM(UIWidgetCommonStrings::EquipmentDisplay, "7"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								LS(CommonStrings::Type, "ss"),
								m_ooNewSlot))
						{
							if (m_ooNewSlot != Data::ObjectSlot::kMax)
							{
								result.action = NodeOverrideCommonAction::Insert;
								result.slot = m_ooNewSlot;
								result.matchType = Data::NodeOverrideConditionType::EquipmentSlot;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					auto race_str = LS(CommonStrings::Race);

					if (LCG_BM(CommonStrings::Race, "8"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Race);

						if (m_matchParamEditor.GetFormPicker().DrawFormSelector(
								LS(CommonStrings::Race, "fs"),
								m_ooNewEntryIDRace))
						{
							if (m_ooNewEntryIDRace)
							{
								result.action = NodeOverrideCommonAction::Insert;
								result.form = m_ooNewEntryIDRace;
								result.matchType = Data::NodeOverrideConditionType::Race;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(LS(CommonStrings::Furniture, "9")))
					{
						result.action = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Furniture;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "A")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UINodeOverrideEditorWidget<T>::HandleValueUpdate(
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const SingleNodeOverrideUpdateParams& a_params,
			const bool a_exists)
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
			T a_handle,
			entryNodeOverrideData_t& a_data,
			const SingleNodeOverrideParentUpdateParams& a_params,
			const bool a_exists)
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
					LS(UINodeOverrideEditorStrings::MatchSkin, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchSkin));

				break;
			case Data::NodeOverrideConditionType::Form:
			case Data::NodeOverrideConditionType::Keyword:

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

				break;

			case Data::NodeOverrideConditionType::Node:

				result |= ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::IgnoreScabbards, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kNodeIgnoreScabbards));

				break;

			case Data::NodeOverrideConditionType::Furniture:

				result |= ImGui::CheckboxFlagsT(
					LS(CommonStrings::LayingDown, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kLayingDown));

				break;
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		bool UINodeOverrideEditorWidget<T>::DrawConditionItemExtra(
			ConditionParamItem a_item,
			void* a_p1,
			const void* a_p2,
			void* a_p3)
		{
			auto match = static_cast<Data::configNodeOverrideCondition_t*>(a_p3);

			bool result = false;

			ImGui::PushID("match_item_extra");

			switch (match->fbf.type)
			{
			case Data::NodeOverrideConditionType::BipedSlot:
			case Data::NodeOverrideConditionType::EquipmentSlot:
			case Data::NodeOverrideConditionType::Furniture:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Data::NodeOverrideConditionType::Form:
			case Data::NodeOverrideConditionType::Race:

				if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch1));

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
			case Data::NodeOverrideConditionType::Form:
			case Data::NodeOverrideConditionType::BipedSlot:
			case Data::NodeOverrideConditionType::EquipmentSlot:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.form_common);
				break;
			case Data::NodeOverrideConditionType::Furniture:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.furniture);
				break;
			case Data::NodeOverrideConditionType::Race:
				m_matchParamEditor.GetFormPicker().SetAllowedTypes(m_type_filters.race);
				break;
			}
		}

	}
}