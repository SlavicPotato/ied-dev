#include "pch.h"

#include "UIStats.h"

#include "IED/Controller/Controller.h"

#include "IED/SkeletonCache.h"

#include "Drivers/UI.h"

#include "UIStatsStrings.h"

#include "UIFormInfoCache.h"
#include "UIMain.h"

#include "I3DI/I3DIMain.h"

namespace IED
{
	namespace UI
	{
		UIStats::UIStats(
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller) :
			UIMiscTextInterface(a_controller),
			UILocalizationInterface(a_controller),
			UITipsInterface(a_controller),
			m_owner(a_owner),
			m_controller(a_controller)
		{
		}

		void UIStats::Draw()
		{
			SetWindowDimensions(0.0f, 600.0f, 400.0f, true);

			if (ImGui::Begin(
					LS<CommonStrings, 3>(
						CommonStrings::Stats,
						WINDOW_ID),
					GetOpenState()))
			{
				auto task = m_owner.As<IUIRenderTaskMain>();
				assert(task);

				auto i3di = task->GetContext().GetChild<I3DIMain>();

				ImGui::Columns(2, nullptr, false);

				ImGui::TextUnformatted("Actor processor:");
				ImGui::TextUnformatted("Effect processor:");
				ImGui::TextUnformatted("UI:");
				//ImGui::TextUnformatted("FC:");
				ImGui::TextUnformatted("Skeleton cache:");
				ImGui::TextUnformatted("Biped cache:");

				const auto odbLevel = m_controller.GetODBLevel();

				if (odbLevel != ObjectDatabaseLevel::kDisabled)
				{
					ImGui::TextUnformatted("Model cache:");
				}

				if (m_controller.PhysicsProcessingEnabled())
				{
					ImGui::TextUnformatted("Physics objects:");
				}

				ImGui::TextUnformatted("CC:");
				ImGui::TextUnformatted("EV:");

				if (i3di)
				{
					if (i3di->GetCommonData())
					{
						ImGui::TextUnformatted("I3DI (P):");
						ImGui::TextUnformatted("I3DI (R):");
					}
				}

				ImGui::NextColumn();

				ImGui::Text("%lld \xC2\xB5s", m_controller.NodeProcessorGetTime());
				ImGui::Text("%lld \xC2\xB5s", m_controller.EffectControllerGetTime());
				ImGui::Text("%lld \xC2\xB5s", Drivers::UI::GetPerf());
				//ImGui::Text("%zu", m_controller.UIGetFormLookupCache().size());

				const auto& sc = SkeletonCache::GetSingleton();
				ImGui::Text("%zu [%zu]", sc.GetSize(), sc.GetTotalEntries());

				ImGui::Text(
					"%zu/%u",
					m_controller.GetEquippedItemCacheSize(),
					m_controller.GetEquippedItemCacheSizeMax());

				if (odbLevel != ObjectDatabaseLevel::kDisabled)
				{
					ImGui::Text(
						"%zu/%zu/%u",
						m_controller.GetODBUnusedObjectCount(),
						m_controller.GetODBObjectCount(),
						stl::underlying(odbLevel));

					DrawTip(UITip::CacheInfo);
				}

				if (m_controller.PhysicsProcessingEnabled())
				{
					ImGui::Text("%zu", m_controller.GetNumSimComponents());
				}

				ImGui::Text("%llu", m_controller.GetCounterValue());
				ImGui::Text("%llu", m_controller.GetEvalCounter());

				if (i3di)
				{
					if (auto& i3diCommonData = i3di->GetCommonData())
					{
						ImGui::Text("%lld \xC2\xB5s", i3di->GetLastPrepTime());
						ImGui::Text("%lld \xC2\xB5s", i3diCommonData->objectController.GetLastRunTime());
					}
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
			constexpr int NUM_COLUMNS = 9;

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
					{ -1.0f, 0.0f }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn(LS(CommonStrings::ID), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Handle), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Name), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Base), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Race), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Equipment), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Custom), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(CommonStrings::Age), ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn(LS(UIStatsStrings::CellAttached), ImGuiTableColumnFlags_None);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				const auto& objects = m_controller.GetObjects();
				const auto& ai      = m_controller.GetActorInfo();

				const auto ss = ImGui::TableGetSortSpecs();

				const auto scl = ss ? get_sort_comp_lambda(ss) : get_sort_comp_lambda_default();

				stl::vector<std::unique_ptr<sorted_list_entry_t>> list;
				list.reserve(objects.size());

				for (auto& e : objects)
				{
					auto v = std::make_unique<sorted_list_entry_t>(
						e,
						e.second.GetNumOccupiedSlots(),
						e.second.GetNumOccupiedCustom(),
						e.second.GetAge() / 60000000);

					if (auto it = ai.find(e.first); it != ai.end())
					{
						v->name = it->second.name;
						v->race = it->second.race;
					}
					else
					{
						m_controller.QueueUpdateActorInfo(e.first);
					}

					auto it = std::upper_bound(list.begin(), list.end(), v, scl);

					list.emplace(it, std::move(v));
				}

				for (const auto& e : list)
				{
					ImGui::TableNextRow();

					ImGui::PushID(e->obj.first.get());

					ImGui::TableSetColumnIndex(0);
					TextCopyable("%.8X", e->obj.first.get());

					ImGui::TableSetColumnIndex(1);
					TextCopyable("%.8X", e->obj.second.GetHandle().get());

					if (!e->name.empty())
					{
						ImGui::TableSetColumnIndex(2);
						TextCopyable("%s", e->name.c_str());
					}

					ImGui::TableSetColumnIndex(3);
					TextCopyable("%.8X", e->obj.second.GetNPCFormID().get());

					if (e->race)
					{
						ImGui::TableSetColumnIndex(4);
						TextCopyable("%.8X", e->race.get());
					}

					ImGui::TableSetColumnIndex(5);
					ImGui::Text("%zu", e->nslot);

					ImGui::TableSetColumnIndex(6);
					ImGui::Text("%zu", e->ncust);

					ImGui::TableSetColumnIndex(7);
					ImGui::Text("%lld", e->age);

					ImGui::TableSetColumnIndex(8);
					ImGui::TextUnformatted(
						e->obj.second.IsCellAttached() ?
							"true" :
							"false");

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
				return a_rhs->obj.first < a_lhs->obj.first;
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
							return a_rhs->obj.first > a_lhs->obj.first;
						};
					}
				case 1:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.GetHandle() < a_lhs->obj.second.GetHandle();
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.GetHandle() > a_lhs->obj.second.GetHandle();
						};
					}
				case 2:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->name < a_lhs->name;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->name > a_lhs->name;
						};
					}
				case 3:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.GetNPCFormID() < a_lhs->obj.second.GetNPCFormID();
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.GetNPCFormID() > a_lhs->obj.second.GetNPCFormID();
						};
					}
				case 4:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->race < a_lhs->race;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->race > a_lhs->race;
						};
					}
				case 5:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->nslot < a_lhs->nslot;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->nslot > a_lhs->nslot;
						};
					}
				case 6:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->ncust < a_lhs->ncust;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->ncust > a_lhs->ncust;
						};
					}
				case 7:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->age < a_lhs->age;
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->age > a_lhs->age;
						};
					}
				case 8:
					if (sort_spec.SortDirection == ImGuiSortDirection_Ascending)
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.IsCellAttached() <
							       a_lhs->obj.second.IsCellAttached();
						};
					}
					else
					{
						return [](auto& a_rhs, auto& a_lhs) {
							return a_rhs->obj.second.IsCellAttached() >
							       a_lhs->obj.second.IsCellAttached();
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