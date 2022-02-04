#include "pch.h"

#include "UIStats.h"

#include "IED/Controller/Controller.h"

#include "Drivers/UI.h"

#include "UIStatsStrings.h"

#include "UIFormInfoCache.h"

namespace IED
{
	namespace UI
	{
		UIStats::UIStats(
			Controller& a_controller) :
			UILocalizationInterface(a_controller),
			UITipsInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIStats::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			SetWindowDimensions(0.0f, 600.0f, 400.0f, true);

			if (ImGui::Begin(
					LS<CommonStrings, 3>(
						CommonStrings::Stats,
						WINDOW_ID),
					GetOpenState()))
			{
				ImGui::Columns(3, nullptr, false);

				ImGui::TextUnformatted("Nodeproc:");
				ImGui::TextUnformatted("UI:");
				ImGui::TextUnformatted("FC:");

				auto odbLevel = m_controller.GetODBLevel();

				if (odbLevel != ObjectDatabaseLevel::kDisabled)
				{
					ImGui::TextUnformatted("Cache:");
				}

				ImGui::NextColumn();

				ImGui::Text("%lld \xC2\xB5s", m_controller.NodeProcessorGetTime());
				ImGui::Text("%lld \xC2\xB5s", Drivers::UI::GetPerf());
				ImGui::Text("%zu", m_controller.UIGetFormLookupCache().size());

				if (odbLevel != ObjectDatabaseLevel::kDisabled)
				{
					ImGui::Text(
						"%zu/%zu/%u",
						m_controller.GetODBUnusedObjectCount(),
						m_controller.GetODBObjectCount(),
						stl::underlying(odbLevel));

					DrawTip(UITip::CacheInfo);
				}

				ImGui::Columns();

				ImGui::Separator();

				if (TreeEx(
						"actor_tree",
						true,
						"%s [%zu]",
						LS(CommonStrings::Actors),
						m_controller.GetObjects().size()))
				{
					DrawActorTable();

					ImGui::TreePop();
				}
			}

			ImGui::End();
		}

		UIData::UICollapsibleStates& UIStats::GetCollapsibleStatesData()
		{
			return m_controller.GetConfigStore().settings.data.ui.statsColStates;
		}

		void UIStats::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UIStats::DrawActorTable()
		{
			constexpr int NUM_COLUMNS = 7;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5, 5 });

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_RowBg |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_Hideable |
						ImGuiTableFlags_Sortable |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0 }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn(LS(CommonStrings::ID), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Handle), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Name), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Race), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Equipment), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Custom), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Age), ImGuiTableColumnFlags_None);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto& objects = m_controller.GetObjects();
				auto& ai = m_controller.GetActorInfo();

				sort_comp_func_t scl;

				if (auto ss = ImGui::TableGetSortSpecs())
				{
					scl = get_sort_comp_lambda(ss);
				}
				else
				{
					scl = get_sort_comp_lambda_default();
				}

				std::list<sorted_list_entry_t> list;

				for (auto& e : objects)
				{
					sorted_list_entry_t v{ e };

					if (auto it = ai.find(e.first); it != ai.cend())
					{
						v.name = it->second.name;
						v.race = it->second.race;
					}
					else
					{
						m_controller.QueueUpdateActorInfo(e.first);
					}

					v.nslot = e.second.GetNumOccupiedSlots();
					v.ncust = e.second.GetNumOccupiedCustom();
					v.age = e.second.GetAge() / 60000000;

					auto it = list.begin();
					while (it != list.end())
					{
						if (scl(v, *it))
						{
							break;
						}

						++it;
					}

					list.emplace(it, std::move(v));
				}

				for (auto& e : list)
				{
					ImGui::TableNextRow();

					ImGui::PushID(e.obj.first.get());

					ImGui::TableSetColumnIndex(0);

					ImGui::Text("%.8X", e.obj.first.get());

					ImGui::TableSetColumnIndex(1);

					ImGui::Text("%.8X", e.obj.second.GetHandle().get());

					if (!e.name.empty())
					{
						ImGui::TableSetColumnIndex(2);
						ImGui::TextUnformatted(e.name.c_str());
					}

					if (e.race)
					{
						ImGui::TableSetColumnIndex(3);
						ImGui::Text("%.8X", e.race.get());
					}

					ImGui::TableSetColumnIndex(4);

					ImGui::Text("%zu", e.nslot);

					ImGui::TableSetColumnIndex(5);

					ImGui::Text("%zu", e.ncust);

					ImGui::TableSetColumnIndex(6);

					ImGui::Text("%lld", e.age);

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		auto UIStats::get_sort_comp_lambda_default()
			-> sort_comp_func_t
		{
			return [](auto& a_rhs, auto& a_lhs) {
				return a_rhs.obj.first < a_lhs.obj.first;
			};
		}

		auto UIStats::get_sort_comp_lambda(const ImGuiTableSortSpecs* a_specs)
			-> sort_comp_func_t
		{
			for (int i = 0; i < a_specs->SpecsCount; i++)
			{
				auto& sort_spec = a_specs->Specs[i];

				switch (sort_spec.ColumnIndex)
				{
				case 0:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return get_sort_comp_lambda_default();
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.obj.first > a_lhs.obj.first;
						};
					}
				case 1:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.obj.second.GetHandle() < a_lhs.obj.second.GetHandle();
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.obj.second.GetHandle() > a_lhs.obj.second.GetHandle();
						};
					}
				case 2:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.name < a_lhs.name;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.name > a_lhs.name;
						};
					}
				case 3:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.race < a_lhs.race;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.race > a_lhs.race;
						};
					}
				case 4:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.nslot < a_lhs.nslot;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.nslot > a_lhs.nslot;
						};
					}
				case 5:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.ncust < a_lhs.ncust;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.ncust > a_lhs.ncust;
						};
					}
				case 6:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.age < a_lhs.age;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs.age > a_lhs.age;
						};
					}
				default:
					break;
				}
			}

			return get_sort_comp_lambda_default();
		}

	}
}