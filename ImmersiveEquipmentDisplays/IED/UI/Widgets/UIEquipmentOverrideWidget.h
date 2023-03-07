#pragma once

#include "UIDescriptionPopup.h"
#include "UIEquipmentOverrideConditionsWidget.h"
#include "UIEquipmentOverrideResult.h"
#include "UIPopupToggleButtonWidget.h"

#include "UIWidgetsCommon.h"

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UILocalizationInterface.h"

#include "UIBaseConfigWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		template <
			class _List,
			class _Param>
		class UIEquipmentOverrideWidget :
			public virtual UIEquipmentOverrideConditionsWidget,
			public virtual UIDescriptionPopupWidget,
			public UICollapsibles
		{
		public:
			using override_type = typename _List::value_type;
			using value_type    = typename override_type::base_value_type;

			void DrawEquipmentOverrides(
				value_type&   a_baseConfig,
				_List&        a_list,
				const _Param& a_params);

		private:
			UIEquipmentOverrideAction DrawEquipmentOverrideTreeContextMenu(
				value_type&   a_baseConfig,
				_List&        a_list,
				const _Param& a_params);

			void DrawEquipmentOverrideList(
				value_type&   a_baseConfig,
				_List&        a_list,
				const _Param& a_params);

			UIEquipmentOverrideResult DrawEquipmentOverrideContextMenu(
				override_type& a_data,
				const _Param&  a_params);

			virtual void DrawExtraEquipmentOverrideOptions(
				value_type&    a_baseConfig,
				const _Param&  a_params,
				override_type& a_override);

			virtual void DrawEquipmentOverrideValues(
				const _Param& a_params,
				value_type&   a_data) = 0;

			constexpr virtual bool EOStoreCC() const;

			virtual void OnEquipmentOverrideChange(
				const _Param& a_params) = 0;
		};

		template <
			class _List,
			class _Param>
		void UIEquipmentOverrideWidget<_List, _Param>::DrawEquipmentOverrides(
			value_type&   a_baseConfig,
			_List&        a_list,
			const _Param& a_params)
		{
			const auto storecc = EOStoreCC();

			ImGui::PushID("config_equipment_overrides");

			const auto contextResult = DrawEquipmentOverrideTreeContextMenu(
				a_baseConfig,
				a_list,
				a_params);

			const bool empty = a_list.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (contextResult == UIEquipmentOverrideAction::Insert)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			bool result;

			if (storecc)
			{
				result = TreeEx(
					"tree",
					true,
					"%s",
					UIL::LS(UIBaseConfigString::EquipmentOverrides));
			}
			else
			{
				result = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UIBaseConfigString::EquipmentOverrides));
			}

			if (result)
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawEquipmentOverrideList(
						a_baseConfig,
						a_list,
						a_params);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
		}

		template <
			class _List,
			class _Param>
		UIEquipmentOverrideAction UIEquipmentOverrideWidget<_List, _Param>::DrawEquipmentOverrideTreeContextMenu(
			value_type&   a_baseConfig,
			_List&        a_list,
			const _Param& a_params)
		{
			UIEquipmentOverrideAction result{
				UIEquipmentOverrideAction::None
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
				if (UIL::LCG_BM(CommonStrings::New, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::Item, "0"))
					{
						if (DrawDescriptionPopup())
						{
							a_list.emplace_back(
								a_baseConfig,
								GetDescriptionPopupBuffer());

							OnEquipmentOverrideChange(a_params);

							ClearDescriptionPopupBuffer();

							result = UIEquipmentOverrideAction::Insert;
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Group, "1"))
					{
						if (DrawDescriptionPopup())
						{
							auto& e = a_list.emplace_back(
								a_baseConfig,
								GetDescriptionPopupBuffer());

							e.overrideFlags.set(Data::ConfigOverrideFlags::kIsGroup);

							OnEquipmentOverrideChange(a_params);

							ClearDescriptionPopupBuffer();

							result = UIEquipmentOverrideAction::Insert;
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set<_List>(a_list);
				}

				{
					auto clipData = UIClipboard::Get<override_type>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "3"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_list.emplace_back(*clipData);

							OnEquipmentOverrideChange(a_params);

							result = UIEquipmentOverrideAction::Paste;
						}
					}
				}

				{
					auto clipData = UIClipboard::Get<_List>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_list = *clipData;

							OnEquipmentOverrideChange(a_params);

							result = UIEquipmentOverrideAction::Paste;
						}
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <
			class _List,
			class _Param>
		void UIEquipmentOverrideWidget<_List, _Param>::DrawEquipmentOverrideList(
			value_type&   a_baseConfig,
			_List&        a_list,
			const _Param& a_params)
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
				ImGui::PushID(i);

				const auto result = DrawEquipmentOverrideContextMenu(*it, a_params);

				switch (result.action)
				{
				case UIEquipmentOverrideAction::Delete:

					it = a_list.erase(it);
					OnEquipmentOverrideChange(a_params);

					break;
				case UIEquipmentOverrideAction::Insert:

					it = a_list.emplace(it, a_baseConfig, result.desc);
					OnEquipmentOverrideChange(a_params);
					ImGui::SetNextItemOpen(true);

					break;
				case UIEquipmentOverrideAction::InsertGroup:

					it = a_list.emplace(it, a_baseConfig, result.desc);
					it->overrideFlags.set(Data::ConfigOverrideFlags::kIsGroup);
					OnEquipmentOverrideChange(a_params);
					ImGui::SetNextItemOpen(true);

					break;
				case UIEquipmentOverrideAction::Swap:

					if (IterSwap(a_list, it, result.dir))
					{
						OnEquipmentOverrideChange(a_params);
					}

					break;
				case UIEquipmentOverrideAction::Reset:

					static_cast<value_type&>(*it) = a_baseConfig;
					OnEquipmentOverrideChange(a_params);

					break;
				case UIEquipmentOverrideAction::Rename:

					it->description = result.desc;
					OnEquipmentOverrideChange(a_params);

					break;
				case UIEquipmentOverrideAction::Paste:

					if (auto clipData = UIClipboard::Get<override_type>())
					{
						it = a_list.emplace(it, *clipData);
						OnEquipmentOverrideChange(a_params);
					}
					[[fallthrough]];

				case UIEquipmentOverrideAction::PasteOver:

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

						DrawExtraEquipmentOverrideOptions(a_baseConfig, a_params, e);

						DrawEquipmentOverrideConditionTree(
							e.conditions,
							[&] {
								OnEquipmentOverrideChange(a_params);
							});

						ImGui::Spacing();

						if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
						{
							ImGui::PushID("eo_group");
							ImGui::PushID("header");

							if (ImGui::CheckboxFlagsT(
									UIL::LS(CommonStrings::Continue, "0"),
									stl::underlying(std::addressof(e.overrideFlags.value)),
									stl::underlying(Data::ConfigOverrideFlags::kContinue)))
							{
								OnEquipmentOverrideChange(a_params);
							}
							UITipsInterface::DrawTip(UITip::EquipmentOverrideGroupContinue);

							ImGui::PopID();

							ImGui::Spacing();

							ImGui::PushID("item");

							DrawEquipmentOverrides(
								a_baseConfig,
								e.group,
								a_params);

							ImGui::PopID();

							ImGui::PopID();
						}
						else
						{
							ImGui::PushID("eo_bc_values");

							DrawEquipmentOverrideValues(a_params, e);

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

		template <
			class _List,
			class _Param>
		UIEquipmentOverrideResult UIEquipmentOverrideWidget<_List, _Param>::DrawEquipmentOverrideContextMenu(
			override_type& a_data,
			const _Param&  a_params)
		{
			UIEquipmentOverrideResult result;

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
								result.action = UIEquipmentOverrideAction::Insert;
								result.desc   = GetDescriptionPopupBuffer();

								ClearDescriptionPopupBuffer();
							}

							ImGui::EndMenu();
						}

						if (UIL::LCG_BM(CommonStrings::Group, "1"))
						{
							if (DrawDescriptionPopup())
							{
								result.action = UIEquipmentOverrideAction::InsertGroup;
								result.desc   = GetDescriptionPopupBuffer();

								ClearDescriptionPopupBuffer();
							}

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					const auto clipData = UIClipboard::Get<override_type>();

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

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Reset, "6")))
				{
					result.action = UIEquipmentOverrideAction::Reset;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "7")))
				{
					UIClipboard::Set(a_data);
				}

				const bool hasClipData = UIClipboard::Get<override_type>() ||
				                         UIClipboard::Get<value_type>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "8"),
						nullptr,
						false,
						hasClipData))
				{
					bool update = false;

					if (auto cd1 = UIClipboard::Get<override_type>())
					{
						a_data = *cd1;

						update = true;
					}
					else if (auto cd2 = UIClipboard::Get<value_type>())
					{
						static_cast<value_type&>(a_data) = *cd2;

						update = true;
					}

					if (update)
					{
						OnEquipmentOverrideChange(a_params);

						result.action = UIEquipmentOverrideAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <
			class _List,
			class _Param>
		void UIEquipmentOverrideWidget<_List, _Param>::DrawExtraEquipmentOverrideOptions(
			value_type&    a_baseConfig,
			const _Param&  a_params,
			override_type& a_override)
		{
		}

		template <
			class _List,
			class _Param>
		constexpr bool UIEquipmentOverrideWidget<
			_List,
			_Param>::EOStoreCC() const
		{
			return false;
		}

	}
}