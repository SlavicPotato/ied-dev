#include "pch.h"

#include "UIActorInfo.h"

#include "UIActorInfoStrings.h"

#include "IED/Controller/Controller.h"

namespace IED
{

	namespace UI
	{
		UIActorInfo::UIActorInfo(Controller& a_controller) :
			UIActorList<ActorInfoData>(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_invFilter(true),
			m_controller(a_controller)
		{
		}

		void UIActorInfo::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			SetWindowDimensions(0.0f, 1200.0f, 800.0f, true);

			if (ImGui::Begin(
					LS<UIWidgetCommonStrings, 3>(
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

			stl::scoped_lock lock(data.data->lock);

			auto handle = m_listCurrent->handle;

			if (!data.data->initialized)
			{
				ImGui::TextUnformatted(LS(UIActorInfoStrings::DataUnavailable));

				return;
			}

			UpdateActorInfoData(handle, data);

			if (!data.data->succeeded)
			{
				ImGui::TextColored(
					UICommon::g_colorWarning,
					"%s",
					LS(UIActorInfoStrings::CouldNotUpdate));

				ImGui::Separator();
				ImGui::Spacing();
			}

			assert(a_data.data->actor == a_handle);

			if (CollapsingHeader(
					"actor_tree",
					true,
					"%s",
					LS(CommonStrings::Actor)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawActorInfoContents(handle, data);

				ImGui::Unindent();
				ImGui::Spacing();
			}

			if (CollapsingHeader(
					"inv_tree",
					false,
					"%s",
					LS(CommonStrings::Inventory)))
			{
				ImGui::Spacing();

				DrawInventoryTreeContents(handle, data);

				ImGui::Spacing();
			}
		}

		void UIActorInfo::DrawActorInfoContents(
			Game::FormID         a_handle,
			const ActorInfoData& a_data)
		{
			auto& data = a_data.data->entry;

			ImGui::Columns(2, nullptr, true);

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
				ImGui::Text("%s:", LS(CommonStrings::Form));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%.8X", a_data.data->actor.get());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Name));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%s", data.name.c_str());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Mod));

				ImGui::TableSetColumnIndex(1);

				std::uint32_t modIndex;
				if (a_handle.GetPluginPartialIndex(modIndex))
				{
					auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

					auto itm = modList.find(modIndex);
					if (itm != modList.end())
					{
						ImGui::TextWrapped(
							"%s [%.2X]",
							itm->second.name.c_str(),
							itm->second.GetPartialIndex());
					}
					else
					{
						ImGui::TextWrapped("%s", LS(CommonStrings::Unknown));
					}
				}
				else
				{
					ImGui::TextWrapped("%s", LS(CommonStrings::Unknown));
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Base));

				ImGui::TableSetColumnIndex(1);
				if (data.npc)
				{
					ImGui::TextWrapped("%.8X", data.npc->form.get());

					if (data.npc->templ)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", LS(CommonStrings::Template));

						ImGui::TableSetColumnIndex(1);
						ImGui::TextWrapped(
							"%.8X",
							data.npc->templ.get());
					}
				}
				else
				{
					ImGui::TextWrapped("%s", "N/A");
				}

				auto& raceInfo = Data::IData::GetRaceList();

				auto race = data.GetRace();

				auto itr = raceInfo.find(race);
				if (itr != raceInfo.end())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Race));

					ImGui::TableSetColumnIndex(1);

					ImGui::TextWrapped(
						"%s [%.8X]",
						itr->second.edid.c_str(),
						race.get());
				}

				if (data.worldspace)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Worldspace));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", data.worldspace.get());
				}

				if (data.cell)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Cell));

					ImGui::TableSetColumnIndex(1);

					ImGui::TextWrapped("%.8X", data.cell.get());

					if (!data.cellName.empty())
					{
						ImGui::SameLine();
						ImGui::TextWrapped("[%s]", data.cellName.c_str());
					}

					if (!data.attached)
					{
						ImGui::SameLine();
						ImGui::TextWrapped("%s", "[Detached]");
					}
				}

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
					ImGui::Text("%s:", LS(CommonStrings::Sex));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%s",
						data.npc->female ?
							LS(CommonStrings::Female) :
                            LS(CommonStrings::Male));
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Weight));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%.0f", data.weight);

				if (data.skin)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Skin));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", data.skin.get());
				}

				if (data.idle)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Idle));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%.8X [%s]",
						data.idle.get(),
						data.idleName.c_str());
				}

				if (data.package)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Package));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", data.package.get());
				}

				if (data.furniture)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Furniture));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", data.furniture.get());
				}

				ImGui::EndTable();
			}

			ImGui::Columns();

			if (data.equipped.first || data.equipped.second)
			{
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				ImGui::Columns(2, nullptr, true);

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
						ImGui::Text("%s [R]:", LS(CommonStrings::Equipped));

						ImGui::TableSetColumnIndex(1);
						ImGui::TextWrapped(
							"%.8X [%hhu] [%s]",
							data.equipped.first.get(),
							data.equippedTypes.first,
							data.equippedNames.first.c_str());
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
						ImGui::Text("%s [L]:", LS(CommonStrings::Equipped));

						ImGui::TableSetColumnIndex(1);
						ImGui::TextWrapped(
							"%.8X [%hhu] [%s]",
							data.equipped.second.get(),
							data.equippedTypes.second,
							data.equippedNames.second.c_str());
					}

					ImGui::EndTable();
				}

				ImGui::Columns();
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			ImGui::Spacing();

			ImGui::TextWrapped(
				"%s: %f %f %f",
				LS(CommonStrings::Position),
				data.pos.x,
				data.pos.y,
				data.pos.z);

			ImGui::TextWrapped(
				"%s: %f %f %f",
				LS(CommonStrings::Rotation),
				data.rot.x,
				data.rot.y,
				data.rot.z);
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

			ImGui::Text("%zu base items", a_data.data->inventory.items.size());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::PopID();

			const auto offsetY = -ImGui::GetStyle().WindowPadding.y;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			if (ImGui::BeginChild(
					"child",
					{ -1.0f, offsetY },
					false,
					ImGuiWindowFlags_HorizontalScrollbar))
			{
				DrawInventoryEntries(a_handle, *a_data.data);
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

		void UIActorInfo::DrawInventoryFilterTree()
		{
			if (TreeEx(
					"flt",
					false,
					"%s",
					LS(CommonStrings::Filters)))
			{
				m_invFilter.Draw();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void UIActorInfo::DrawInventoryEntries(
			Game::FormID              a_handle,
			const ActorInfoAggregate& a_data)
		{
			auto& data = a_data.inventory;

			ImGui::PushID("list");

			for (auto& [i, e] : data.items)
			{
				char buffer[256];

				stl::snprintf(
					buffer,
					"[%.8X] [%hhu] [%s] [%lld]",
					i.get(),
					e.type,
					e.name.c_str(),
					e.count);

				if (!m_invFilter.Test(buffer))
				{
					continue;
				}

				bool disabled = e.count <= 0;

				if (disabled)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
				}

				ImGui::PushID(i.get());

				if (ImGui::TreeNodeEx(
						"",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						buffer))
				{
					ImGui::Spacing();

					DrawInventoryBaseTree(a_handle, e);

					if (!e.extraList.empty())
					{
						DrawInventoryExtraListsTree(a_handle, e);
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

		void UIActorInfo::DrawInventoryBaseTree(
			Game::FormID                      a_handle,
			const actorInventoryInfo_t::Base& a_data)
		{
			if (ImGui::TreeNodeEx(
					"base_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(CommonStrings::Base)))
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
				ImGui::Text("%s:", LS(CommonStrings::Form));

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.8X", a_data.formid.get());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Name));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%s", a_data.name.c_str());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Playable));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%s",
					a_data.flags.test(InventoryInfoBaseFlags::kNonPlayable) ?
						LS(CommonStrings::No) :
                        LS(CommonStrings::Yes));

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Count));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%s: %u, %s: %d, %s: %lld",
					LS(CommonStrings::Base),
					a_data.baseCount,
					LS(CommonStrings::Delta),
					a_data.deltaCount,
					LS(CommonStrings::Total),
					a_data.count);

				if (a_data.enchantment)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(UIWidgetCommonStrings::Enchantment));

					ImGui::TableSetColumnIndex(1);

					if (a_data.enchantment->name)
					{
						ImGui::TextWrapped(
							"%.8X [%s]",
							a_data.enchantment->formid,
							a_data.enchantment->name->c_str());
					}
					else
					{
						ImGui::TextWrapped("%.8X", a_data.enchantment->formid);
					}
				}

				ImGui::EndTable();
			}
		}

		void UIActorInfo::DrawInventoryExtraListsTree(
			Game::FormID                      a_handle,
			const actorInventoryInfo_t::Base& a_data)
		{
			if (ImGui::TreeNodeEx(
					"extra_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(CommonStrings::Extra)))
			{
				ImGui::Spacing();

				DrawInventoryExtraLists(a_handle, a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		void UIActorInfo::DrawInventoryExtraLists(
			Game::FormID                      a_handle,
			const actorInventoryInfo_t::Base& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });
				
			constexpr int NUM_COLUMNS = 8;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupColumn(LS(CommonStrings::Name));
				ImGui::TableSetupColumn(LS(CommonStrings::Equipped));
				ImGui::TableSetupColumn(LS(CommonStrings::Favorited));
				ImGui::TableSetupColumn(LS(UIWidgetCommonStrings::PreventEquip));
				ImGui::TableSetupColumn(LS(UIWidgetCommonStrings::Enchantment));
				ImGui::TableSetupColumn(LS(CommonStrings::Health));
				ImGui::TableSetupColumn(LS(CommonStrings::UID));
				ImGui::TableSetupColumn(LS(CommonStrings::Owner));

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
					ImGui::TextUnformatted(e.GetName(a_data).c_str());

					ImGui::TableSetColumnIndex(1);
					if (e.flags.test_any(InventoryInfoExtraFlags::kEquipped))
					{
						ImGui::Text("%s [R]", LS(CommonStrings::Yes));
					}
					else if (e.flags.test_any(InventoryInfoExtraFlags::kEquippedLeft))
					{
						ImGui::Text("%s [L]", LS(CommonStrings::Yes));
					}
					else
					{
						ImGui::Text("%s", LS(CommonStrings::No));
					}
					
					ImGui::TableSetColumnIndex(2);
					ImGui::Text(
						"%s",
						e.flags.test(InventoryInfoExtraFlags::kFavorite) ?
							LS(CommonStrings::Yes) :
                            LS(CommonStrings::No));

					ImGui::TableSetColumnIndex(3);
					ImGui::Text(
						"%s",
						e.flags.test(InventoryInfoExtraFlags::kCannotWear) ?
							LS(CommonStrings::Yes) :
                            LS(CommonStrings::No));

					ImGui::TableSetColumnIndex(4);
					if (auto& enchantment = e.GetEnchantment(a_data))
					{
						if (!e.enchantment)
						{
							ImGui::TextUnformatted("[B]");
							ImGui::SameLine();
						}

						if (enchantment->name)
						{
							ImGui::Text(
								"%.8X [%s]",
								enchantment->formid,
								enchantment->name->c_str());
						}
						else
						{
							ImGui::Text("%.8X", enchantment->formid);
						}
					}
					else
					{
						ImGui::Text("%s", LS(CommonStrings::No));
					}

					ImGui::TableSetColumnIndex(5);
					if (e.flags.test(InventoryInfoExtraFlags::kHasHealth))
					{
						ImGui::Text("%f", e.health);
					}
					else
					{
						ImGui::Text("%s", LS(CommonStrings::No));
					}

					ImGui::TableSetColumnIndex(6);
					if (e.uid)
					{
						ImGui::Text("%hu", *e.uid);
					}
					else
					{
						ImGui::Text("%s", LS(CommonStrings::No));
					}
					
					ImGui::TableSetColumnIndex(7);
					if (e.owner)
					{
						ImGui::Text("%.8X", e.owner);
					}
					else
					{
						ImGui::Text("%s", LS(CommonStrings::None));
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
				stl::scoped_lock lock(m_controller.GetLock());

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
