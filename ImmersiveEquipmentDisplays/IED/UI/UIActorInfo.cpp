#include "pch.h"

#include "UIActorInfo.h"

#include "UIActorInfoStrings.h"

#include "IED/UI/Widgets/UIVariableTypeSelectorWidget.h"

#include "IED/Controller/Controller.h"

namespace IED
{

	namespace UI
	{
		UIActorInfo::UIActorInfo(Controller& a_controller) :
			UIFormInfoTooltipWidget(a_controller),
			UIMiscTextInterface(a_controller),
			UIActorList<ActorInfoData>(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			m_invFilter(true),
			m_controller(a_controller)
		{
		}

		void UIActorInfo::Draw()
		{
			SetWindowDimensions(0.0f, 1200.0f, 800.0f, true);

			if (ImGui::Begin(
					UIL::LS<UIWidgetCommonStrings, 3>(
						UIWidgetCommonStrings::ActorInfo,
						WINDOW_ID),
					GetOpenState()))
			{
				ImGui::Spacing();

				ListDraw();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				DrawActorInfo();
			}

			ImGui::End();
		}

		void UIActorInfo::OnOpen()
		{
			Reset();
		}

		void UIActorInfo::OnClose()
		{
			Reset();
		}

		void UIActorInfo::Reset()
		{
			ListReset();
		}

		void UIActorInfo::DrawActorInfo()
		{
			if (!m_listCurrent)
			{
				return;
			}

			auto& data = m_listCurrent->data;
			if (!data.data)
			{
				return;
			}

			const boost::lock_guard lock(data.data->lock);

			const auto handle = m_listCurrent->handle;

			if (!data.data->initialized)
			{
				ImGui::TextUnformatted(
					UIL::LS(UIActorInfoStrings::DataUnavailable));

				return;
			}

			UpdateActorInfoData(handle, data);

			if (!data.data->succeeded)
			{
				ImGui::TextColored(
					UICommon::g_colorWarning,
					"%s",
					UIL::LS(UIActorInfoStrings::CouldNotUpdate));

				ImGui::Separator();
				ImGui::Spacing();
			}

			assert(data.data->actor == handle);

			if (CollapsingHeader(
					"actor_tree",
					true,
					"%s",
					UIL::LS(CommonStrings::Actor)))
			{
				ImGui::Spacing();
				ImGui::Indent();
				ImGui::PushID("actor");

				DrawActorInfoContents(handle, data);

				ImGui::PopID();
				ImGui::Unindent();
				ImGui::Spacing();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			DrawTabBar(handle, data);
		}

		void UIActorInfo::DrawTabBar(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			if (ImGui::BeginTabBar(
					"tab_bar",
					ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
			{
				DrawInventoryTabItem(a_handle, a_data);
				DrawFactionsTabItem(a_handle, a_data);
				DrawEffectsTabItem(a_handle, a_data);
				DrawVariablesTabItem(a_handle);
				DrawActorStateTabItem(a_handle, a_data);

				ImGui::EndTabBar();
			}
		}

		void UIActorInfo::DrawInventoryTabItem(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			stl::snprintf(
				m_buffer,
				"%s (%zu)",
				UIL::LS(CommonStrings::Inventory),
				a_data.data->inventory.items.size());

			if (ImGui::BeginTabItem(
					UIL::LMKID<3>(m_buffer, "1")))
			{
				ImGui::Spacing();
				ImGui::PushID("1");

				DrawInventoryTreeContents(a_handle, a_data);

				ImGui::PopID();
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
		}

		void UIActorInfo::DrawFactionsTabItem(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			stl::snprintf(
				m_buffer,
				"%s (%zu)",
				UIL::LS(CommonStrings::Factions),
				a_data.data->factions.data.size());

			if (ImGui::BeginTabItem(
					UIL::LMKID<3>(m_buffer, "2")))
			{
				ImGui::Spacing();
				ImGui::PushID("2");

				DrawFactionTreeContents(a_handle, a_data);

				ImGui::PopID();
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
		}

		void UIActorInfo::DrawEffectsTabItem(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			stl::snprintf(
				m_buffer,
				"%s (%zu)",
				UIL::LS(UIActorInfoStrings::ActiveEffects),
				a_data.data->effects.data.size());

			if (ImGui::BeginTabItem(
					UIL::LMKID<3>(m_buffer, "3")))
			{
				ImGui::Spacing();
				ImGui::PushID("3");

				DrawEffectTreeContents(a_handle, a_data);

				ImGui::PopID();
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
		}

		void UIActorInfo::DrawVariablesTabItem(
			Game::FormID a_handle)
		{
			const auto& objects = m_controller.GetObjects();

			auto it = objects.find(a_handle);
			if (it != objects.end())
			{
				const auto& data = it->second.GetVariables();

				stl::snprintf(
					m_buffer,
					"%s (%zu)",
					UIL::LS(CommonStrings::Variables),
					data.size());

				if (ImGui::BeginTabItem(
						UIL::LMKID<3>(m_buffer, "4")))
				{
					ImGui::Spacing();
					ImGui::PushID("4");

					DrawVariableTreeContents(a_handle, data);

					ImGui::PopID();
					ImGui::Spacing();

					ImGui::EndTabItem();
				}
			}
		}

		void UIActorInfo::DrawActorStateTabItem(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			if (ImGui::BeginTabItem(
					UIL::LS(UIActorInfoStrings::ActorState, "5")))
			{
				ImGui::Spacing();
				ImGui::PushID("5");

				DrawActorStateContents(a_handle, a_data);

				ImGui::PopID();
				ImGui::Spacing();

				ImGui::EndTabItem();
			}
		}

		template <class T>
		bool UIActorInfo::DrawActorInfoLineFormStringPair(
			T                                           a_label,
			const std::pair<Game::FormID, std::string>& a_data)
		{
			if (a_data.first)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(a_label));

				ImGui::TableSetColumnIndex(1);

				DrawFormWithInfoWrapped(a_data.first);

				if (!a_data.second.empty())
				{
					ImGui::SameLine();
					TextWrappedCopyable("[%s]", a_data.second.c_str());
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class T>
		bool UIActorInfo::DrawActorInfoLineForm(
			T            a_label,
			Game::FormID a_data)
		{
			if (a_data)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(a_label));

				ImGui::TableSetColumnIndex(1);
				DrawFormWithInfoWrapped(a_data);

				return true;
			}
			else
			{
				return false;
			}
		}

		void UIActorInfo::DrawActorInfoContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			auto& data = a_data.data->entry;

			DrawActorInfoContentsFirst(a_handle, a_data);

			if (data.equipped.first || data.equipped.second)
			{
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				DrawActorInfoContentsSecond(a_handle, a_data);
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			DrawActorInfoContentsThird(a_handle, a_data);
		}

		void UIActorInfo::DrawActorInfoContentsFirst(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			auto& data = a_data.data->entry;

			ImGui::Columns(2, "col", true);

			if (ImGui::BeginTable(
					"ltable_1",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Form));

				ImGui::TableSetColumnIndex(1);
				DrawFormWithInfoWrapped(a_data.data->actor);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Base));

				ImGui::TableSetColumnIndex(1);
				if (data.npc)
				{
					DrawFormWithInfoWrapped(data.npc->form);

					if (data.npc->templ)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::Template));

						ImGui::TableSetColumnIndex(1);
						DrawFormWithInfoWrapped(data.npc->templ);
					}

					if (data.npc->nontemp != data.npc->form &&
					    data.npc->nontemp != data.npc->templ)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::NonTemp));

						ImGui::TableSetColumnIndex(1);
						DrawFormWithInfoWrapped(data.npc->nontemp);
					}
				}
				else
				{
					ImGui::Text("%s", "N/A");
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Name));

				ImGui::TableSetColumnIndex(1);
				TextWrappedCopyable("%s", data.name.c_str());

				std::uint32_t modIndex;
				if (a_handle.GetPluginPartialIndex(modIndex))
				{
					auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

					auto itm = modList.find(modIndex);
					if (itm != modList.end())
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::Mod));

						ImGui::TableSetColumnIndex(1);

						TextWrappedCopyable(
							"[%X] %s",
							itm->second.GetPartialIndex(),
							itm->second.name.c_str());
					}
				}

				auto& raceInfo = Data::IData::GetRaceList();

				const auto race = data.GetRace();

				auto itr = raceInfo.find(race);
				if (itr != raceInfo.end())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(CommonStrings::Race));

					ImGui::TableSetColumnIndex(1);

					DrawFormWithInfoWrapped(race);
					ImGui::SameLine();
					TextWrappedCopyable("[%s]", itr->second.edid.c_str());
				}

				DrawActorInfoLineForm(CommonStrings::Worldspace, data.worldspace);

				if (DrawActorInfoLineFormStringPair(CommonStrings::Cell, data.cell))
				{
					if (!data.attached)
					{
						ImGui::SameLine();
						ImGui::TextWrapped("%s", "[Detached]");
					}
				}

				DrawActorInfoLineFormStringPair(CommonStrings::Location, data.location);

				ImGui::EndTable();
			}

			ImGui::NextColumn();

			if (ImGui::BeginTable(
					"rtable_1",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				if (data.npc)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(CommonStrings::Sex));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%s",
						data.npc->female ?
							UIL::LS(CommonStrings::Female) :
							UIL::LS(CommonStrings::Male));
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Flags));

				ImGui::TableSetColumnIndex(1);
				TextCopyable("%.8X", data.flags);

				UICommon::ToolTip(100.0f, [&] {
					if (data.flags & 1u << 4)
					{
						ImGui::Text(UIL::LS(UIWidgetCommonStrings::CollisionDisabled));
					}

					if (data.flags & 1u << 5)
					{
						ImGui::Text(UIL::LS(CommonStrings::Deleted));
					}

					if (data.flags & 1u << 10)
					{
						ImGui::Text(UIL::LS(CommonStrings::Persistent));
					}

					if (data.flags & 1u << 11)
					{
						ImGui::Text(UIL::LS(CommonStrings::Disabled));
					}

					if (data.flags & 1u << 12)
					{
						ImGui::Text(UIL::LS(UIWidgetCommonStrings::Ignored));
					}

					if (data.flags & 1u << 16)
					{
						ImGui::Text(UIL::LS(UIWidgetCommonStrings::FullLOD));
					}
				});

				if (data.dead || data.hasLoadedState)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(CommonStrings::State));

					ImGui::TableSetColumnIndex(1);

					if (data.hasLoadedState)
					{
						ImGui::TextWrapped("%s", UIL::LS(CommonStrings::Loaded));
					}

					if (data.dead)
					{
						if (data.hasLoadedState)
						{
							ImGui::SameLine();
							ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
							ImGui::SameLine();
						}

						ImGui::TextWrapped("%s", UIL::LS(CommonStrings::Dead));
					}
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Weight));

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.0f", data.weight);

				if (data.inventoryWeight)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(UIActorInfoStrings::InventoryWeight));

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%.0f", data.inventoryWeight->first);
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
					ImGui::Text("%.0f", data.inventoryWeight->second);
				}

				DrawActorInfoLineFormStringPair(CommonStrings::Skin, data.skin);

				if (DrawActorInfoLineForm(CommonStrings::Outfit, data.outfit.first))
				{
					if (data.outfit.second)
					{
						ImGui::SameLine();
						ImGui::TextWrapped("[%s]", UIL::LS(CommonStrings::Default));
					}
				}

				DrawActorInfoLineFormStringPair(CommonStrings::Idle, data.idle);
				DrawActorInfoLineFormStringPair(CommonStrings::Furniture, data.furniture);
				DrawActorInfoLineForm(CommonStrings::Package, data.package);

				ImGui::EndTable();
			}

			ImGui::Columns();
		}

		void UIActorInfo::DrawActorInfoContentsSecond(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			auto& data = a_data.data->entry;

			ImGui::Columns(2, "col", true);

			if (ImGui::BeginTable(
					"ltable_2",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				if (data.equipped.first)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s [R]:", UIL::LS(CommonStrings::Equipped));

					ImGui::TableSetColumnIndex(1);

					DrawFormWithInfoWrapped(data.equipped.first);
					ImGui::SameLine();

					if (auto typeDest = UIFormTypeSelectorWidget::form_type_to_desc(data.equippedTypes.first))
					{
						ImGui::TextWrapped(
							"[%s|%hhu] [%s]",
							typeDest,
							data.equippedTypes.first,
							data.equippedNames.first.c_str());
					}
					else
					{
						ImGui::TextWrapped(
							"[%hhu] [%s]",
							data.equippedTypes.first,
							data.equippedNames.first.c_str());
					}
				}

				ImGui::EndTable();
			}

			ImGui::NextColumn();

			if (ImGui::BeginTable(
					"rtable_2",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				if (data.equipped.second)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s [L]:", UIL::LS(CommonStrings::Equipped));

					ImGui::TableSetColumnIndex(1);

					DrawFormWithInfoWrapped(data.equipped.second);
					ImGui::SameLine();

					if (auto typeDest = UIFormTypeSelectorWidget::form_type_to_desc(data.equippedTypes.second))
					{
						ImGui::TextWrapped(
							"[%s|%hhu] [%s]",
							typeDest,
							data.equippedTypes.second,
							data.equippedNames.second.c_str());
					}
					else
					{
						ImGui::TextWrapped(
							"[%hhu] [%s]",
							data.equippedTypes.second,
							data.equippedNames.second.c_str());
					}
				}

				ImGui::EndTable();
			}

			ImGui::Columns();
		}

		void UIActorInfo::DrawActorInfoContentsThird(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			auto& data = a_data.data->entry;

			ImGui::Columns(2, "col", true);

			if (ImGui::BeginTable(
					"ltable_3",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Position));

				ImGui::TableSetColumnIndex(1);
				TextWrappedCopyable(
					"%f %f %f",
					data.pos.x,
					data.pos.y,
					data.pos.z);

				ImGui::EndTable();
			}

			ImGui::NextColumn();

			if (ImGui::BeginTable(
					"rtable_3",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Rotation));

				ImGui::TableSetColumnIndex(1);
				TextWrappedCopyable(
					"%f %f %f",
					data.rot.x,
					data.rot.y,
					data.rot.z);

				ImGui::EndTable();
			}

			ImGui::Columns();
		}

		void UIActorInfo::DrawFactionTreeContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			const auto offsetY = ImGui::GetStyle().ItemInnerSpacing.y;

			if (ImGui::BeginChild(
					"child",
					{ -1.0f, -offsetY },
					false))
			{
				DrawFactionEntries(a_handle, *a_data.data);
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawInventoryTreeContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			ImGui::PushID("header");

			DrawInventoryFilterTree();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::PopID();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			const auto offsetY = ImGui::GetStyle().ItemInnerSpacing.y;

			if (ImGui::BeginChild(
					"child",
					{ -1.0f, -offsetY },
					false,
					ImGuiWindowFlags_HorizontalScrollbar))
			{
				DrawInventoryEntries(a_handle, *a_data.data);
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawEffectTreeContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			const auto offsetY = ImGui::GetStyle().ItemInnerSpacing.y;

			if (ImGui::BeginChild(
					"child",
					{ -1.0f, -offsetY },
					false))
			{
				DrawEffectEntries(a_handle, *a_data.data);
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawVariableTreeContents(
			Game::FormID                    a_handle,
			const conditionalVariableMap_t& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			const auto offsetY = ImGui::GetStyle().ItemInnerSpacing.y;

			if (ImGui::BeginChild(
					"child",
					{ -1.0f, -offsetY },
					false))
			{
				DrawVariableEntries(a_handle, a_data);
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

		static constexpr const char* state_val_to_str(ActorState::SIT_SLEEP_STATE a_val)
		{
			switch (a_val)
			{
			case ActorState::SIT_SLEEP_STATE::kNormal:
				return UIL::LS(UIActorInfoStrings::Normal);
			case ActorState::SIT_SLEEP_STATE::kWantToSit:
				return UIL::LS(UIActorInfoStrings::WantToSit);
			case ActorState::SIT_SLEEP_STATE::kWaitingForSitAnim:
				return UIL::LS(UIActorInfoStrings::WaitingForSitAnim);
			case ActorState::SIT_SLEEP_STATE::kIsSitting:
				return UIL::LS(UIActorInfoStrings::IsSitting);
			case ActorState::SIT_SLEEP_STATE::kWantToStand:
				return UIL::LS(UIActorInfoStrings::WantToStand);
			case ActorState::SIT_SLEEP_STATE::kWantToSleep:
				return UIL::LS(UIActorInfoStrings::WantToSleep);
			case ActorState::SIT_SLEEP_STATE::kWaitingForSleepAnim:
				return UIL::LS(UIActorInfoStrings::WaitingForSleepAnim);
			case ActorState::SIT_SLEEP_STATE::kIsSleeping:
				return UIL::LS(UIActorInfoStrings::IsSleeping);
			case ActorState::SIT_SLEEP_STATE::kWantToWake:
				return UIL::LS(UIActorInfoStrings::WantToWake);
			default:
				return nullptr;
			}
		}

		static constexpr const char* state_val_to_str(ActorState::FLY_STATE a_val)
		{
			switch (a_val)
			{
			case ActorState::FLY_STATE::kNone:
				return UIL::LS(CommonStrings::None);
			case ActorState::FLY_STATE::kTakeOff:
				return UIL::LS(UIActorInfoStrings::TakeOff);
			case ActorState::FLY_STATE::kCruising:
				return UIL::LS(UIActorInfoStrings::Cruising);
			case ActorState::FLY_STATE::kHovering:
				return UIL::LS(UIActorInfoStrings::Hovering);
			case ActorState::FLY_STATE::kLanding:
				return UIL::LS(UIActorInfoStrings::Landing);
			case ActorState::FLY_STATE::kPerching:
				return UIL::LS(UIActorInfoStrings::Perching);
			case ActorState::FLY_STATE::kAction:
				return UIL::LS(UIActorInfoStrings::Action);
			default:
				return nullptr;
			}
		}

		static constexpr const char* state_val_to_str(ActorState::ACTOR_LIFE_STATE a_val)
		{
			switch (a_val)
			{
			case ActorState::ACTOR_LIFE_STATE::kAlive:
				return UIL::LS(UIActorInfoStrings::Alive);
			case ActorState::ACTOR_LIFE_STATE::kDying:
				return UIL::LS(UIActorInfoStrings::Dying);
			case ActorState::ACTOR_LIFE_STATE::kDead:
				return UIL::LS(CommonStrings::Dead);
			case ActorState::ACTOR_LIFE_STATE::kUnconcious:
				return UIL::LS(UIActorInfoStrings::Unconcious);
			case ActorState::ACTOR_LIFE_STATE::kReanimate:
				return UIL::LS(UIActorInfoStrings::Reanimate);
			case ActorState::ACTOR_LIFE_STATE::kRecycle:
				return UIL::LS(UIActorInfoStrings::Recycle);
			case ActorState::ACTOR_LIFE_STATE::kRestrained:
				return UIL::LS(UIActorInfoStrings::Restrained);
			case ActorState::ACTOR_LIFE_STATE::kEssentialDown:
				return UIL::LS(UIActorInfoStrings::EssentialDown);
			case ActorState::ACTOR_LIFE_STATE::kBleedout:
				return UIL::LS(UIActorInfoStrings::Bleedout);
			default:
				return nullptr;
			}
		}

		static constexpr const char* state_val_to_str(ActorState::KNOCK_STATE_ENUM a_val)
		{
			switch (a_val)
			{
			case ActorState::KNOCK_STATE_ENUM::kNormal:
				return UIL::LS(UIActorInfoStrings::Normal);
			case ActorState::KNOCK_STATE_ENUM::kExplode:
				return UIL::LS(UIActorInfoStrings::Explode);
			case ActorState::KNOCK_STATE_ENUM::kExplodeLeadIn:
				return UIL::LS(UIActorInfoStrings::ExplodeLeadIn);
			case ActorState::KNOCK_STATE_ENUM::kOut:
				return UIL::LS(UIActorInfoStrings::Out);
			case ActorState::KNOCK_STATE_ENUM::kOutLeadIn:
				return UIL::LS(UIActorInfoStrings::OutLeadIn);
			case ActorState::KNOCK_STATE_ENUM::kQueued:
				return UIL::LS(UIActorInfoStrings::Queued);
			case ActorState::KNOCK_STATE_ENUM::kGetUp:
				return UIL::LS(UIActorInfoStrings::GetUp);
			case ActorState::KNOCK_STATE_ENUM::kDown:
				return UIL::LS(UIActorInfoStrings::Down);
			case ActorState::KNOCK_STATE_ENUM::kWaitForTaskQueue:
				return UIL::LS(UIActorInfoStrings::WaitForTaskQueue);
			default:
				return nullptr;
			}
		}

		static constexpr const char* state_val_to_str(ActorState::ATTACK_STATE_ENUM a_val)
		{
			switch (a_val)
			{
			case ActorState::ATTACK_STATE_ENUM::kNone:
				return UIL::LS(CommonStrings::None);
			case ActorState::ATTACK_STATE_ENUM::kDraw:
				return UIL::LS(UIActorInfoStrings::Draw);
			case ActorState::ATTACK_STATE_ENUM::kSwing:
				return UIL::LS(UIActorInfoStrings::Swing);
			case ActorState::ATTACK_STATE_ENUM::kHit:
				return UIL::LS(UIActorInfoStrings::Hit);
			case ActorState::ATTACK_STATE_ENUM::kNextAttack:
				return UIL::LS(UIActorInfoStrings::NextAttack);
			case ActorState::ATTACK_STATE_ENUM::kFollowThrough:
				return UIL::LS(UIActorInfoStrings::FollowThrough);
			case ActorState::ATTACK_STATE_ENUM::kBash:
				return UIL::LS(UIActorInfoStrings::Bash);
			case ActorState::ATTACK_STATE_ENUM::kBowDraw:
				return UIL::LS(UIActorInfoStrings::BowDraw);
			case ActorState::ATTACK_STATE_ENUM::kBowAttached:
				return UIL::LS(UIActorInfoStrings::BowAttached);
			case ActorState::ATTACK_STATE_ENUM::kBowDrawn:
				return UIL::LS(UIActorInfoStrings::BowDrawn);
			case ActorState::ATTACK_STATE_ENUM::kBowReleasing:
				return UIL::LS(UIActorInfoStrings::BowReleasing);
			case ActorState::ATTACK_STATE_ENUM::kBowReleased:
				return UIL::LS(UIActorInfoStrings::BowReleased);
			case ActorState::ATTACK_STATE_ENUM::kBowNextAttack:
				return UIL::LS(UIActorInfoStrings::BowNextAttack);
			case ActorState::ATTACK_STATE_ENUM::kBowFollowThrough:
				return UIL::LS(UIActorInfoStrings::BowFollowThrough);
			case ActorState::ATTACK_STATE_ENUM::kFire:
				return UIL::LS(UIActorInfoStrings::Fire);
			case ActorState::ATTACK_STATE_ENUM::kFiring:
				return UIL::LS(UIActorInfoStrings::Firing);
			case ActorState::ATTACK_STATE_ENUM::kFired:
				return UIL::LS(UIActorInfoStrings::Fired);
			default:
				return nullptr;
			}
		}

		static constexpr const char* state_val_to_str(ActorState::WEAPON_STATE a_val)
		{
			switch (a_val)
			{
			case ActorState::WEAPON_STATE::kSheathed:
				return UIL::LS(UIActorInfoStrings::Sheathed);
			case ActorState::WEAPON_STATE::kWantToDraw:
				return UIL::LS(UIActorInfoStrings::WantToDraw);
			case ActorState::WEAPON_STATE::kDrawing:
				return UIL::LS(UIActorInfoStrings::Drawing);
			case ActorState::WEAPON_STATE::kDrawn:
				return UIL::LS(UIActorInfoStrings::Drawn);
			case ActorState::WEAPON_STATE::kWantToSheathe:
				return UIL::LS(UIActorInfoStrings::WantToSheathe);
			case ActorState::WEAPON_STATE::kSheathing:
				return UIL::LS(UIActorInfoStrings::Sheathing);
			default:
				return nullptr;
			}
		}

		static void draw_state_column(
			UIActorInfoStrings a_string,
			std::uint32_t      a_value,
			const char*        a_valstr)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextWrapped("%s", UIL::LS(a_string));
			ImGui::TableSetColumnIndex(1);
			ImGui::TextWrapped("%u [%s]", a_value, a_valstr);
		}

		static void draw_state_column(
			UIActorInfoStrings a_string,
			std::uint32_t      a_value)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextWrapped("%s", UIL::LS(a_string));
			ImGui::TableSetColumnIndex(1);
			ImGui::TextWrapped("%u", a_value);
		}

		template <class T>
		static void draw_state_column(UIActorInfoStrings a_string, T a_value)  //
			requires(std::is_enum_v<T>)
		{
			draw_state_column(
				a_string,
				static_cast<std::uint32_t>(a_value),
				state_val_to_str(a_value));
		}

		void UIActorInfo::DrawActorStateContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			const auto& state1 = a_data.data->entry.state1;
			const auto& state2 = a_data.data->entry.state2;

			ImGui::Columns(2, "col", true);

			if (ImGui::BeginTable(
					"lt",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextWrapped("%s", UIL::LS(UIActorInfoStrings::MovementState));
				ImGui::TableSetColumnIndex(1);
				ImGui::Text(
					"F:%u B:%u R:%u L:%u",
					state1.movingForward,
					state1.movingBack,
					state1.movingRight,
					state1.movingLeft);

				draw_state_column(UIActorInfoStrings::WalkingState, state1.walking);
				draw_state_column(UIActorInfoStrings::RunningState, state1.running);
				draw_state_column(UIActorInfoStrings::SprintingState, state1.sprinting);
				draw_state_column(UIActorInfoStrings::SneakingState, state1.sneaking);
				draw_state_column(UIActorInfoStrings::SwimmingState, state1.swimming);
				draw_state_column(UIActorInfoStrings::SitSleepState, state1.sitSleepState);
				draw_state_column(UIActorInfoStrings::FlyState, state1.flyState);
				draw_state_column(UIActorInfoStrings::LifeState, state1.lifeState);
				draw_state_column(UIActorInfoStrings::KnockState, state1.knockState);
				draw_state_column(UIActorInfoStrings::AttackState, state1.meleeAttackState);

				ImGui::EndTable();
			}

			ImGui::NextColumn();

			if (ImGui::BeginTable(
					"rt",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.35f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.65f);

				draw_state_column(UIActorInfoStrings::TalkingToPlayerState, state2.talkingToPlayer);
				draw_state_column(UIActorInfoStrings::ForceRunState, state2.forceRun);
				draw_state_column(UIActorInfoStrings::ForceSneakState, state2.forceSneak);
				draw_state_column(UIActorInfoStrings::HeadTrackingState, state2.headTracking);
				draw_state_column(UIActorInfoStrings::ReanimatingState, state2.reanimating);
				draw_state_column(UIActorInfoStrings::WeaponState, state2.weaponState);
				draw_state_column(UIActorInfoStrings::WantBlockingState, state2.wantBlocking);
				draw_state_column(UIActorInfoStrings::FlightBlockedState, state2.flightBlocked);
				draw_state_column(UIActorInfoStrings::RecoilState, state2.recoil);
				draw_state_column(UIActorInfoStrings::AllowFlyingState, state2.allowFlying);
				draw_state_column(UIActorInfoStrings::StaggeredState, state2.staggered);

				ImGui::EndTable();
			}

			ImGui::Columns();
		}

		void UIActorInfo::DrawInventoryFilterTree()
		{
			if (TreeEx(
					"flt",
					false,
					"%s",
					UIL::LS(CommonStrings::Filters)))
			{
				m_invFilter.Draw();

				ImGui::TreePop();
			}
		}

		void UIActorInfo::DrawFactionEntries(
			Game::FormID              a_handle,
			const ActorInfoAggregate& a_data)
		{
			auto& data = a_data.factions;

			constexpr int NUM_COLUMNS = 5;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, -1.0f }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);

				ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Name));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Rank));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Merchant));
				ImGui::TableSetupColumn(UIL::LS(UIActorInfoStrings::PlayerEnemy));

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				for (auto& e : data.data)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					DrawFormWithInfo(e.first);

					ImGui::TableSetColumnIndex(1);
					TextCopyable("%s", e.second.name.c_str());

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%hhd", e.second.rank);

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%s", e.second.isMerchant ? "Yes" : "No");

					ImGui::TableSetColumnIndex(4);
					ImGui::Text("%s", e.second.flags.test(FACTION_DATA::Flag::kPlayerIsEnemy) ? "Yes" : "No");
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawInventoryEntries(
			Game::FormID              a_handle,
			const ActorInfoAggregate& a_data)
		{
			auto& data = a_data.inventory;

			ImGui::PushID("list");

			for (auto& d : data.items.getvec())
			{
				const auto i = d->first;
				auto&      e = d->second;

				if (e.IsEquipped())
				{
					stl::snprintf(
						m_buffer,
						"[%.8X] [%hhu] [%s] [%d] [%s]",
						i.get(),
						e.type,
						e.name.c_str(),
						e.count,
						UIL::LS(CommonStrings::Equipped));
				}
				else
				{
					stl::snprintf(
						m_buffer,
						"[%.8X] [%hhu] [%s] [%d]",
						i.get(),
						e.type,
						e.name.c_str(),
						e.count);
				}

				if (!m_invFilter.Test(m_buffer))
				{
					continue;
				}

				const bool disabled = e.count <= 0;

				if (disabled)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
				}

				ImGui::PushID(i.get());

				if (ImGui::TreeNodeEx(
						"",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						m_buffer))
				{
					ImGui::Spacing();

					DrawInventoryBaseTree(a_handle, e);

					if (!e.extraList.empty())
					{
						DrawInventoryExtraListsTree(a_handle, a_data, e);
					}

					ImGui::Spacing();

					ImGui::TreePop();
				}

				if (disabled)
				{
					ImGui::PopStyleColor();
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		void UIActorInfo::DrawEffectFormCell(
			const actorActiveEffectInfo_t::formEntry_t& a_entry,
			bool                                        a_type)
		{
			if (!a_entry.id)
			{
				return;
			}

			if (!a_entry.name.empty())
			{
				if (a_type)
				{
					DrawFormWithInfo(a_entry.id);
					ImGui::SameLine();

					if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(a_entry.type))
					{
						ImGui::TextWrapped(
							"[%s] [%s]",
							typeDesc,
							a_entry.name.c_str());
					}
					else
					{
						ImGui::TextWrapped(
							"[%hhu] [%s]",
							a_entry.type,
							a_entry.name.c_str());
					}
				}
				else
				{
					DrawFormWithInfo(a_entry.id);
					ImGui::SameLine();
					ImGui::TextWrapped(
						"[%s]",
						a_entry.name.c_str());
				}
			}
			else
			{
				if (a_type)
				{
					DrawFormWithInfo(a_entry.id);
					ImGui::SameLine();

					if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(a_entry.type))
					{
						ImGui::TextWrapped("[%s]", typeDesc);
					}
					else
					{
						ImGui::TextWrapped("[%hhu]", a_entry.type);
					}
				}
				else
				{
					DrawFormWithInfo(a_entry.id);
				}
			}
		}

		void UIActorInfo::DrawEffectEntries(
			Game::FormID              a_handle,
			const ActorInfoAggregate& a_data)
		{
			auto& data = a_data.effects;

			constexpr int NUM_COLUMNS = 7;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, -1.0f }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);

				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Effect));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Flags));
				ImGui::TableSetupColumn(UIL::LS(UIActorInfoStrings::SpellOrEnchantment));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Source));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Elapsed));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Duration));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Magnitude));

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				for (auto& e : data.data)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);

					const bool inactive = e.flags.test_any(
						ActiveEffect::Flag::kInactive |
						ActiveEffect::Flag::kDispelled);

					if (inactive)
					{
						ImGui::PushStyleVar(
							ImGuiStyleVar_Alpha,
							ImGui::GetStyle().Alpha * 0.5f);
					}

					DrawEffectFormCell(e.effect, false);

					if (inactive)
					{
						ImGui::PopStyleVar();
					}

					ImGui::TableSetColumnIndex(1);
					TextCopyable("%.8X", e.flags.value);

					UICommon::ToolTip(100.0f, [&] {
						if (e.flags.test(ActiveEffect::Flag::kInactive))
						{
							ImGui::Text(UIL::LS(CommonStrings::Inactive));
						}

						if (e.flags.test(ActiveEffect::Flag::kDispelled))
						{
							ImGui::Text(UIL::LS(CommonStrings::Dispelled));
						}

						if (e.flags.test(ActiveEffect::Flag::kRecovers))
						{
							ImGui::Text(UIL::LS(CommonStrings::Recovers));
						}

						if (e.flags.test(ActiveEffect::Flag::kHasConditions))
						{
							ImGui::Text(UIL::LS(CommonStrings::Conditions));
						}
					});

					ImGui::TableSetColumnIndex(2);
					DrawEffectFormCell(e.spell, true);

					ImGui::TableSetColumnIndex(3);
					DrawEffectFormCell(e.source, false);

					ImGui::TableSetColumnIndex(4);
					ImGui::Text("%.1f", e.elapsed);

					if (e.duration != 0.0f)
					{
						ImGui::TableSetColumnIndex(5);
						ImGui::Text("%.1f", e.duration);
					}

					ImGui::TableSetColumnIndex(6);
					ImGui::Text("%.1f", e.magnitude);
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawVariableEntries(
			Game::FormID                    a_handle,
			const conditionalVariableMap_t& a_data)
		{
			constexpr int NUM_COLUMNS = 3;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, -1.0f }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);

				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Name));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Type));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Value));

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				for (auto& [i, e] : a_data)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					TextWrappedCopyable("%s", i.c_str());

					ImGui::TableSetColumnIndex(1);

					if (const auto typeDesc =
					        UIVariableTypeSelectorWidget::variable_type_to_desc(e.type))
					{
						ImGui::TextWrapped("%s", typeDesc);
					}
					else
					{
						ImGui::TextWrapped("%u", e.type);
					}

					ImGui::TableSetColumnIndex(2);

					switch (e.type)
					{
					case ConditionalVariableType::kInt32:
						TextCopyable("%d", e.i32);
						break;
					case ConditionalVariableType::kFloat:
						TextCopyable("%.3f", e.f32);
						break;
					case ConditionalVariableType::kForm:
						DrawFormWithInfo(e.form.get_id());
						break;
					}
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawInventoryBaseTree(
			Game::FormID                      a_handle,
			const actorInventoryInfo_t::Base& a_data)
		{
			if (ImGui::TreeNodeEx(
					"base_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(CommonStrings::Base)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawInventoryBase(a_handle, a_data);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		void UIActorInfo::DrawInventoryBase(
			Game::FormID                      a_handle,
			const actorInventoryInfo_t::Base& a_data)
		{
			if (ImGui::BeginTable(
					"table",
					2,
					ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.15f);
				ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.85f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Form));

				ImGui::TableSetColumnIndex(1);
				DrawFormWithInfoWrapped(a_data.formid);

				if (!a_data.name.empty())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(CommonStrings::Name));

					ImGui::TableSetColumnIndex(1);
					TextWrappedCopyable("%s", a_data.name.c_str());
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Playable));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%s",
					a_data.flags.test(InventoryInfoBaseFlags::kNonPlayable) ?
						UIL::LS(CommonStrings::No) :
						UIL::LS(CommonStrings::Yes));

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Count));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%s: %u, %s: %d, %s: %d",
					UIL::LS(CommonStrings::Base),
					a_data.baseCount,
					UIL::LS(CommonStrings::Delta),
					a_data.countDelta,
					UIL::LS(CommonStrings::Total),
					a_data.count);

				if (a_data.enchantment)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(UIWidgetCommonStrings::Enchantment));

					ImGui::TableSetColumnIndex(1);

					DrawFormWithInfoWrapped(a_data.enchantment->formid);

					if (a_data.enchantment->name)
					{
						ImGui::SameLine();
						TextWrappedCopyable(
							"[%s]",
							a_data.enchantment->name->c_str());
					}
				}

				ImGui::EndTable();
			}
		}

		void UIActorInfo::DrawInventoryExtraListsTree(
			Game::FormID                      a_handle,
			const ActorInfoAggregate&         a_info,
			const actorInventoryInfo_t::Base& a_data)
		{
			if (ImGui::TreeNodeEx(
					"extra_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(CommonStrings::Extra)))
			{
				ImGui::Spacing();

				DrawInventoryExtraLists(a_handle, a_info, a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		void UIActorInfo::DrawInventoryExtraLists(
			Game::FormID                      a_handle,
			const ActorInfoAggregate&         a_info,
			const actorInventoryInfo_t::Base& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			constexpr int NUM_COLUMNS = 10;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Name));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Equipped));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Favorited));
				ImGui::TableSetupColumn(UIL::LS(UIWidgetCommonStrings::PreventEquip));
				ImGui::TableSetupColumn(UIL::LS(UIWidgetCommonStrings::ForceEquip));
				ImGui::TableSetupColumn(UIL::LS(UIWidgetCommonStrings::Enchantment));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Health));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::UID));
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Owner));
				ImGui::TableSetupColumn(UIL::LS(UIWidgetCommonStrings::OriginalREFR));

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				std::uint32_t i = 0;

				for (auto& e : a_data.extraList)
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					TextCopyable("%s", e.GetName(a_data).c_str());

					ImGui::TableSetColumnIndex(1);

					if (e.flags.test_any(InventoryInfoExtraFlags::kEquippedMask))
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::Yes));

						if (e.flags.test(InventoryInfoExtraFlags::kEquipped) &&
						    a_info.entry.equipped.first &&
						    a_data.formid == a_info.entry.equipped.first)
						{
							ImGui::SameLine();
							ImGui::TextUnformatted("[R]");
						}

						if (e.flags.test(InventoryInfoExtraFlags::kEquippedLeft))
						{
							ImGui::SameLine();
							ImGui::TextUnformatted("[L]");
						}
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::No));
					}

					ImGui::TableSetColumnIndex(2);
					ImGui::Text(
						"%s",
						e.flags.test(InventoryInfoExtraFlags::kFavorite) ?
							UIL::LS(CommonStrings::Yes) :
							UIL::LS(CommonStrings::No));

					ImGui::TableSetColumnIndex(3);
					ImGui::Text(
						"%s",
						e.flags.test(InventoryInfoExtraFlags::kCannotWear) ?
							UIL::LS(CommonStrings::Yes) :
							UIL::LS(CommonStrings::No));

					ImGui::TableSetColumnIndex(4);
					ImGui::Text(
						"%s",
						e.flags.test(InventoryInfoExtraFlags::kShouldWear) ?
							UIL::LS(CommonStrings::Yes) :
							UIL::LS(CommonStrings::No));

					ImGui::TableSetColumnIndex(5);
					if (auto& enchantment = e.GetEnchantment(a_data))
					{
						if (!e.enchantment)
						{
							ImGui::TextUnformatted("[B]");
							ImGui::SameLine();
						}

						DrawFormWithInfoWrapped(enchantment->formid);

						if (enchantment->name)
						{
							ImGui::SameLine();
							ImGui::Text(
								"[%s]",
								enchantment->name->c_str());
						}
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::No));
					}

					ImGui::TableSetColumnIndex(6);
					if (e.flags.test(InventoryInfoExtraFlags::kHasHealth))
					{
						ImGui::Text("%f", e.health);
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::No));
					}

					ImGui::TableSetColumnIndex(7);
					if (e.uid)
					{
						TextCopyable("%hu", *e.uid);
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::None));
					}

					ImGui::TableSetColumnIndex(8);
					if (e.owner)
					{
						DrawFormWithInfoWrapped(e.owner);
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::None));
					}

					ImGui::TableSetColumnIndex(9);
					if (e.originalRefr)
					{
						DrawFormWithInfoWrapped(e.originalRefr);
					}
					else
					{
						ImGui::Text("%s", UIL::LS(CommonStrings::None));
					}

					ImGui::PopID();

					i++;
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UIActorInfo::UpdateActorInfoData(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			if (IPerfCounter::delta_us(
					a_data.data->lastUpdate,
					IPerfCounter::Query()) > 100000)
			{
				QueueInfoUpdate(
					a_handle,
					a_data);
			}
		}

		ActorInfoData UIActorInfo::GetData(Game::FormID a_handle)
		{
			ActorInfoData result;

			QueueInfoUpdate(a_handle, result);

			return result;
		}

		void UIActorInfo::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UIActorInfo::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelActorSettings& UIActorInfo::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.actorInfo.actorSettings;
		}

		UIData::UICollapsibleStates& UIActorInfo::GetCollapsibleStatesData()
		{
			return m_controller.GetConfigStore().settings.data.ui.actorInfo.colStates;
		}

		void UIActorInfo::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UIActorInfo::QueueInfoUpdate(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			if (!a_data.data)
			{
				return;
			}

			ITaskPool::AddTask([this, handle = a_handle, data = a_data.data] {
				const boost::lock_guard lock(m_controller.GetLock());
				const boost::lock_guard datalock(data->lock);

				data->lastUpdate  = IPerfCounter::Query();
				data->actor       = handle;
				data->initialized = true;

				if (auto actor = handle.As<Actor>())
				{
					m_controller.FillActorInfoEntry(actor, data->entry, true);

					if (auto npc = actor->GetActorBase())
					{
						data->inventory.Update(*npc, GetEntryDataList(actor));
					}
					else
					{
						data->inventory.items.clear();
					}

					data->factions.Update(actor);
					data->effects.Update(actor);

					data->succeeded = true;
				}
				else
				{
					data->succeeded = false;
				}
			});
		}
	}
}
