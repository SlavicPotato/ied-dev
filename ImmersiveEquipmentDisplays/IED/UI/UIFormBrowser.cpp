#include "pch.h"

#include "UIFormBrowser.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIFormBrowser::UIFormBrowser(Controller& a_controller) :
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_formIDFilter(true),
			m_formNameFilter(true),
			m_tabItems{
				{ { UIFormBrowserStrings::Weapons, TESObjectWEAP::kTypeID },
			      { UIFormBrowserStrings::Armor, IFormDatabase::EXTRA_TYPE_ARMOR },
			      { UIFormBrowserStrings::Shields, TESObjectARMO::kTypeID },
			      { UIFormBrowserStrings::Ammo, TESAmmo::kTypeID },
			      { UIFormBrowserStrings::Torches, TESObjectLIGH::kTypeID },
			      { UIFormBrowserStrings::Misc, TESObjectMISC::kTypeID },
			      { UIFormBrowserStrings::Ingredients, IngredientItem::kTypeID },
			      { UIFormBrowserStrings::PotionsFood, AlchemyItem::kTypeID },
			      { UIFormBrowserStrings::Keys, TESKey::kTypeID },
			      { UIFormBrowserStrings::Books, TESObjectBOOK::kTypeID },
			      { UIFormBrowserStrings::SoulGems, TESSoulGem::kTypeID },
			      { UIFormBrowserStrings::Scrolls, ScrollItem::kTypeID },
			      { UIFormBrowserStrings::Spells, SpellItem::kTypeID },
			      { UIFormBrowserStrings::Static, TESObjectSTAT::kTypeID },
			      { UIFormBrowserStrings::MovableStatic, BGSMovableStatic::kTypeID },
			      { UIFormBrowserStrings::Flora, TESFlora::kTypeID },
			      { UIFormBrowserStrings::Furniture, TESFurniture::kTypeID },
			      { UIFormBrowserStrings::Keywords, BGSKeyword::kTypeID },
			      { UIFormBrowserStrings::NPCS, TESNPC::kTypeID },
			      { UIFormBrowserStrings::Races, TESRace::kTypeID },
			      { UIFormBrowserStrings::ArtObjects, BGSArtObject::kTypeID },
			      { UIFormBrowserStrings::AnimObject, TESObjectANIO::kTypeID },
			      { UIFormBrowserStrings::Trees, TESObjectTREE::kTypeID },
			      { UIFormBrowserStrings::Grass, TESGrass::kTypeID },
			      { UIFormBrowserStrings::Activators, TESObjectACTI::kTypeID },
			      { UIFormBrowserStrings::TalkingActivators, BGSTalkingActivator::kTypeID },
			      { UIFormBrowserStrings::Sounds, BGSSoundDescriptorForm::kTypeID },
			      { UIFormBrowserStrings::Quests, TESQuest::kTypeID },
			      { UIFormBrowserStrings::Locations, BGSLocation::kTypeID },
			      { UIFormBrowserStrings::Worldspaces, TESWorldSpace::kTypeID }

			    }
			}

		{
			m_formIDFilter.SetFlags(
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_CharsHexadecimal);

			m_formNameFilter.SetFlags(ImGuiInputTextFlags_EnterReturnsTrue);
		}

		auto UIFormBrowser::Draw()
			-> FormBrowserDrawResult
		{
			FormBrowserDrawResult result{ false, false };

			SetWindowDimensions(0.f, 1200.f, 700.f, true);

			if (ImGui::BeginPopupModal(
					LS(UIWidgetCommonStrings::FormBrowser, POPUP_ID),
					GetOpenState(),
					ImGuiWindowFlags_NoScrollbar |
						ImGuiWindowFlags_NoScrollWithMouse))
			{
				const auto offsetY =
					ImGui::GetFontSize() + (ImGui::GetStyle().ItemInnerSpacing.y * 2.f) + 3.f;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

				if (ImGui::BeginChild(
						"main",
						{ -1.0f, -offsetY },
						false,
						ImGuiWindowFlags_NoScrollbar |
							ImGuiWindowFlags_NoScrollWithMouse))
				{
					if (!m_data)
					{
						QueueGetDatabase();

						auto text = LS(UIWidgetCommonStrings::QueryingDatabase);

						const auto w = ImGui::GetWindowSize().x -
						               ImGui::CalcTextSize(text).x;

						ImGui::SetCursorPosX(w * 0.5f);
						ImGui::TextUnformatted(text);
					}
					else
					{
						result.result = DrawTabBar();
					}
				}

				ImGui::EndChild();

				ImGui::PopStyleVar();

				if (m_multiSelectMode)
				{
					if (ImGui::Button(
							LS(CommonStrings::OK, "ctl_1"),
							{ -1.f, 0.f }))
					{
						result.result = !m_selectedEntries.empty();
						SetOpenState(false);
					}
				}
				else
				{
					if (ImGui::Button(
							LS(CommonStrings::Close, "ctl_1"),
							{ -1.f, 0.f }))
					{
						SetOpenState(false);
					}
				}

				if (!IsWindowOpen())
				{
					result.closed = true;
				}

				ImGui::EndPopup();
			}

			return result;
		}

		bool UIFormBrowser::Open(bool a_multisel)
		{
			ClearTabFilter();

			m_hlForm = {};
			m_multiSelectMode = a_multisel;
			m_selectedEntries.clear();

			SetOpenState(true);

			ImGui::OpenPopup(LS(UIWidgetCommonStrings::FormBrowser, POPUP_ID));

			return true;
		}

		bool UIFormBrowser::IsBrowserOpen() const
		{
			return ImGui::IsPopupOpen(LS(UIWidgetCommonStrings::FormBrowser, POPUP_ID));
		}

		void UIFormBrowser::SetTabFilter(
			const tab_filter_type& a_filter)
		{
			if (a_filter.empty())
			{
				ClearTabFilter();
			}
			else
			{
				for (auto& f : m_tabItems)
				{
					f.enabled = a_filter.contains(f.type);
				}
			}
		}

		void UIFormBrowser::SetTabFilter(
			std::initializer_list<tab_filter_type::value_type> a_init)
		{
			if (a_init.size() == 0)
			{
				ClearTabFilter();
			}
			else
			{
				for (auto& f : m_tabItems)
				{
					f.enabled = std::find(
									a_init.begin(),
									a_init.end(),
									f.type) != a_init.end();
				}
			}
		}

		void UIFormBrowser::ClearTabFilter()
		{
			for (auto& f : m_tabItems)
			{
				f.enabled = true;
			}
		}

		void UIFormBrowser::OnClose()
		{
			if (!IsWindowOpen())
			{
				Reset();
			}
		}

		bool UIFormBrowser::DrawTabBar()
		{
			if (m_multiSelectMode)
			{
				DrawContextMenu();
			}

			bool result = false;

			if (ImGui::BeginTabBar("form_browser_tab_bar"))
			{
				for (std::uint32_t i = 0; i < std::size(m_tabItems); i++)
				{
					auto& e = m_tabItems[i];

					if (!e.enabled)
					{
						continue;
					}

					ImGui::PushID(i);

					if (ImGui::BeginTabItem(LS(e.label, "_")))
					{
						result |= DrawTabPanel(e.type);
						ImGui::EndTabItem();
					}

					ImGui::PopID();
				}

				ImGui::EndTabBar();
			}

			return result;
		}

		bool UIFormBrowser::DrawTabPanel(std::uint32_t a_type)
		{
			auto it = m_data->find(a_type);
			if (it == m_data->end())
			{
				return false;
			}

			bool result = false;

			bool filterUpdate = false;

			if (a_type != m_currentType)
			{
				m_currentType = a_type;

				filterUpdate = true;
			}

			constexpr int COLUMNS_COUNT = 2;

			constexpr auto TABLE_FLAGS =
				ImGuiTableFlags_Borders |
				ImGuiTableFlags_ScrollY |
				ImGuiTableFlags_Hideable;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 2, 2 });

			if (ImGui::BeginTable(
					"form_browser_table",
					COLUMNS_COUNT,
					TABLE_FLAGS,
					{ -1.0f, -1.0f }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn(LS(CommonStrings::FormID));
				ImGui::TableSetupColumn(LS(CommonStrings::Name));

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (int column = 0; column < COLUMNS_COUNT; column++)
				{
					ImGui::TableSetColumnIndex(column);
					auto column_name = ImGui::TableGetColumnName(column);
					ImGui::PushID(column);
					ImGui::TableHeader(column_name);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });

					if (column == 0)
					{
						ImGui::SameLine(70.f);
						ImGui::PushItemWidth(75.0f);

						filterUpdate |= m_formIDFilter.Draw();

						ImGui::PopItemWidth();
					}
					else
					{
						ImGui::SameLine(50.f);
						ImGui::PushItemWidth(165.0f);

						filterUpdate |= m_formNameFilter.Draw();

						ImGui::PopItemWidth();
					}

					ImGui::PopStyleVar();

					ImGui::PopID();
				}

				if (m_filteredData)
				{
					result = DrawTable(*m_filteredData);
				}
				else
				{
					result = DrawTable(it->second);
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();

			if (filterUpdate || m_nextDoFilterUpdate)
			{
				m_nextDoFilterUpdate = false;

				if (m_formIDFilter.Has() ||
				    m_formNameFilter.Has())
				{
					m_filteredData->clear();
					m_filteredData->reserve(it->second.size());

					Game::FormID formID;

					if (m_formIDFilter.Has())
					{
						char buf[16];
						stl::snprintf(buf, "0x%s", m_formIDFilter.GetBuffer());

						char* end = nullptr;

						auto v = std::strtoul(buf, &end, 0);

						if (end > buf)
						{
							formID = v;
						}
					}

					for (auto& e : it->second)
					{
						if ((!formID || e.formid == formID) &&
						    m_formNameFilter.Test(e.name))
						{
							m_filteredData->emplace_back(e);
						}
					}

					m_filteredData.mark(true);
				}
				else
				{
					m_filteredData.reset();
				}
			}

			return result;
		}

		template <class T>
		bool UIFormBrowser::DrawTable(const T& a_data)
		{
			constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_SpanAllColumns;

			bool result = false;

			auto size = static_cast<int>(
				std::min(a_data.size(), std::size_t(std::numeric_limits<int>::max() - 1)));

			ImGuiListClipper clipper;
			clipper.Begin(size);
			while (clipper.Step())
			{
				for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					if (i < 0 || i >= size)
					{
						break;
					}

					auto& e = a_data[i];

					char buf[16 + std::numeric_limits<int>::digits10 + 3];
					stl::snprintf(buf, "%.8X##_%d", e.formid.get(), i);

					ImGui::TableNextRow();

					if (m_multiSelectMode)
					{
						if (m_selectedEntries.contains(e.formid))
						{
							ImGui::TableSetBgColor(
								ImGuiTableBgTarget_RowBg1,
								IM_COL32(50, 50, 50, 255));
						}
					}
					else
					{
						if (m_hlForm && m_hlForm == e.formid)
						{
							ImGui::TableSetBgColor(
								ImGuiTableBgTarget_RowBg1,
								IM_COL32(50, 50, 50, 255));
						}
					}

					ImGui::TableSetColumnIndex(0);

					if (ImGui::Selectable(buf, false, SELECTABLE_FLAGS))
					{
						if (m_multiSelectMode)
						{
							if (!m_selectedEntries.emplace(e.formid, e).second)
							{
								m_selectedEntries.erase(e.formid);
							}
						}
						else
						{
							m_selectedEntry = e;
							result = true;
							SetOpenState(false);
						}
					}

					ImGui::TableSetColumnIndex(1);

					ImGui::TextUnformatted(e.name.c_str());
				}
			}

			return result;
		}

		void UIFormBrowser::Reset()
		{
			m_data.reset();
			m_filteredData.reset();
			m_nextDoFilterUpdate = true;
			m_dbQueryInProgress = false;
		}

		bool UIFormBrowser::HasType(std::uint32_t a_type) const
		{
			auto it = std::find_if(
				m_tabItems.begin(),
				m_tabItems.end(),
				[&](auto& a_v) {
					return a_v.enabled && a_v.type == a_type;
				});

			return it != m_tabItems.end();
		}

		void UIFormBrowser::DrawContextMenu()
		{
			ImGui::PushID("fb_context_area");

			ImGui::PushStyleVar(
				ImGuiStyleVar_WindowPadding,
				{ 8, 8 });

			DrawPopupToggleButton("open", "context_menu");

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						LS(UIWidgetCommonStrings::ClearSelection, "1"),
						nullptr,
						false,
						!m_selectedEntries.empty()))
				{
					m_selectedEntries.clear();
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();
		}

		void UIFormBrowser::QueueGetDatabase()
		{
			if (m_dbQueryInProgress)
			{
				return;
			}

			m_dbQueryInProgress = true;
			m_controller.QueueGetFormDatabase(
				[this](IFormDatabase::result_type a_result) {
					if (m_dbQueryInProgress)
					{
						m_data = a_result;
						m_dbQueryInProgress = false;
					}
				});
		}
	}
}