#include "pch.h"

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)

#	include "UIOutfitFormManager.h"

#	include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"
#	include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#	include "IED/UI/UICommon.h"
#	include "IED/UI/UIFormBrowserCommonFilters.h"

#	include "UIOutfitFormManagerStrings.h"

#	include "IED/OM/PersistentOutfitFormManager.h"

#	include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitFormManager::UIOutfitFormManager(
				Controller& a_controller) :
				UIProfileSelectorWidget<
					profileSelectorParamsOutfitForm_t,
					OutfitFormListProfile>(
					UIProfileSelectorFlags::kEnableApply),
				UIFormLookupInterface(a_controller),
				m_fp(a_controller, FormInfoFlags::kNone, true),
				m_filter(true),
				m_controller(a_controller)
			{
				m_fp.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::OutfitItems));
				m_fp.SetFormBrowserEnabled(true);
			}

			UIOutfitFormManager::~UIOutfitFormManager()
			{
				GlobalProfileManager::GetSingleton<OutfitFormListProfile>().RemoveSink(this);
			}

			void UIOutfitFormManager::Initialize()
			{
				InitializeProfileBase();
			}

			void UIOutfitFormManager::Draw()
			{
				SetWindowDimensions(0.0f, 800.0f, 600.0f, true);

				if (ImGui::Begin(
						UIL::LS<UIOutfitFormManagerStrings, 3>(
							UIOutfitFormManagerStrings::OutfitFormManager,
							WINDOW_ID),
						GetOpenState(),
						ImGuiWindowFlags_MenuBar))
				{
					DrawMenuBar();
					DrawFilterTree();
					ImGui::Separator();
					DrawProfileTree();
					ImGui::Separator();
					DrawEntryPanel();
				}

				ImGui::End();
			}

			void UIOutfitFormManager::OnClose()
			{
				m_data.reset();
			}

			void UIOutfitFormManager::UpdateData()
			{
				if (!m_data)
				{
					m_data = stl::make_smart_for_overwrite<CachedOutfitData>();
				}

				ITaskPool::AddTask([data = m_data] {
					data->Update();
				});
			}

			void UIOutfitFormManager::DrawEntryPanel()
			{
				if (ImGui::BeginChild("entry_panel", { -1.0f, 0.0f }))
				{
					UpdateData();

					ImGui::PushID("entries");
					DrawEntryList();
					ImGui::PopID();

					if (m_flags.consume(UIOutfitFormManagerFlags::kRefreshData))
					{
						UpdateData();
					}
				}

				ImGui::EndChild();
			}

			void UIOutfitFormManager::DrawEntryList()
			{
				const stl::lock_guard lock(m_data->lock);

				for (auto& e : m_data->data)
				{
					if (!m_filter.Test(e.first))
					{
						continue;
					}

					ImGui::PushID(e.first.c_str());

					const auto contextResult = DrawEntryContextMenu(e);

					switch (contextResult)
					{
					case UIOutfitFormManagerContextAction::Add:
						ImGui::SetNextItemOpen(true);
						break;
					}

					if (ImGui::TreeNodeEx(
							"tree",
							ImGuiTreeNodeFlags_SpanAvailWidth,
							"[%.8X] - %s",
							e.second.formid.get(),
							e.first.c_str()))
					{
						ImGui::Spacing();
						ImGui::Indent();

						DrawEntry(e);

						ImGui::Unindent();
						ImGui::Spacing();

						ImGui::TreePop();
					}

					ImGui::PopID();
				}
			}

			auto UIOutfitFormManager::DrawEntryContextMenu(const value_type& a_value)
				-> UIOutfitFormManagerContextAction
			{
				UIOutfitFormManagerContextAction result{
					UIOutfitFormManagerContextAction::None
				};

				ImGui::PushID("context_area");

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

				if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
				{
					m_newEntryID = {};
					ClearDescriptionPopupBuffer();
				}

				ImGui::PopStyleVar();

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::BeginPopup("context_menu"))
				{
					if (UIL::LCG_BM(CommonStrings::Add, "1"))
					{
						if (UIL::LCG_BM(UIWidgetCommonStrings::AddOne, "1"))
						{
							if (m_fp.DrawFormSelector(m_newEntryID))
							{
								if (m_newEntryID)
								{
									QueueAddOutfitItem(a_value.first, m_newEntryID);

									result = UIOutfitFormManagerContextAction::Add;

									ImGui::CloseCurrentPopup();
								}
							}

							ImGui::EndMenu();
						}

						if (UIL::LCG_BM(UIWidgetCommonStrings::AddMultiple, "2"))
						{
							if (m_fp.DrawFormSelectorMulti())
							{
								auto& entries = m_fp.GetSelectedEntries();
								if (!entries.empty())
								{
									stl::vector<Data::configFormZeroMissing_t> list;
									list.reserve(entries.size());

									auto& vec = entries.getvec();

									std::transform(
										vec.begin(),
										vec.end(),
										std::back_inserter(list),
										[](auto& a_v) {
											return a_v->first;
										});

									QueueAddOutfitItemList(a_value.first, std::move(list));

									result = UIOutfitFormManagerContextAction::Add;

									ImGui::CloseCurrentPopup();
								}
							}

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					/*if (UIL::LCG_BM(CommonStrings::Name, "2"))
					{
						if (DrawDescriptionPopup())
						{
							QueueRenameOutfit(a_value.first, GetDescriptionPopupBuffer());

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}*/

					ImGui::Separator();

					if (UIL::LCG_MI(CommonStrings::Delete, "3"))
					{
						QueueRemoveOutfitForm(a_value.first);
					}

					if (UIL::LCG_MI(CommonStrings::Clear, "4"))
					{
						QueueClearOutfitList(a_value.first);
					}

					ImGui::Separator();

					if (UIL::LCG_MI(UIWidgetCommonStrings::CopyIDToClipboard, "5"))
					{
						char buf[9];
						stl::snprintf(buf, "%.8X", a_value.second.formid.get());
						ImGui::SetClipboardText(buf);
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();

				return result;
			}

			auto UIOutfitFormManager::DrawOutfitItemEntryContextMenu()
				-> UIOutfitFormManagerContextAction
			{
				UIOutfitFormManagerContextAction result{
					UIOutfitFormManagerContextAction::None
				};

				ImGui::PushID("context_area");

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

				UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

				ImGui::PopStyleVar();

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::BeginPopup("context_menu"))
				{
					if (UIL::LCG_MI(IED::CommonStrings::Delete, "1"))
					{
						result = UIOutfitFormManagerContextAction::Delete;
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();

				return result;
			}

			void UIOutfitFormManager::DrawEntry(
				const value_type& a_value)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.f, 5.f });

				constexpr int NUM_COLUMNS = 3;

				if (ImGui::BeginTable(
						"form_list_table",
						NUM_COLUMNS,
						ImGuiTableFlags_Borders |
							ImGuiTableFlags_Resizable |
							ImGuiTableFlags_NoSavedSettings |
							ImGuiTableFlags_SizingStretchProp,
						{ -1.0f, 0.f }))
				{
					const auto w = (ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x);

					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
					ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
					ImGui::TableSetupColumn(UIL::LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

					for (int column = 0; column < NUM_COLUMNS; column++)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::TableHeader(ImGui::TableGetColumnName(column));
					}

					DrawOutfitList(a_value);

					ImGui::EndTable();
				}

				ImGui::PopStyleVar();
			}

			void UIOutfitFormManager::DrawOutfitList(
				const value_type& a_value)
			{
				const auto& list = a_value.second.list;

				int i = 0;

				for (auto it = list.begin(); it != list.end(); ++it)
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					const auto contextResult = DrawOutfitItemEntryContextMenu();

					switch (contextResult)
					{
					case UIOutfitFormManagerContextAction::Delete:
						QueueRemoveOutfitItem(
							a_value.first,
							static_cast<std::uint32_t>(std::distance(list.begin(), it)));
						break;
					}

					char buf[16];
					stl::snprintf(buf, "%.8X##form", it->get());

					ImGui::TableSetColumnIndex(1);

					auto& fb = m_controller.UIGetFormBrowser();

					if (ImGui::Selectable(
							buf,
							false,
							ImGuiSelectableFlags_DontClosePopups))
					{
						m_updateItemID = *it;

						if (!ImGui::GetIO().KeyShift)
						{
							fb.Open(false);
							if (auto& filt = m_fp.GetAllowedTypes())
							{
								fb.SetTabFilter(*filt);
							}
							else
							{
								fb.ClearTabFilter();
							}
						}
						else
						{
							ImGui::OpenPopup("fe_context_menu");
						}
					}

					if (fb.DrawImpl().result)
					{
						if (auto& f = fb.GetSelectedEntry(); f && f->formid)
						{
							QueueUpdateOutfitItem(
								a_value.first,
								static_cast<std::uint32_t>(std::distance(list.begin(), it)),
								f->formid);
						}
					}

					if (ImGui::BeginPopup("fe_context_menu"))
					{
						if (m_fp.DrawFormSelector(m_updateItemID))
						{
							if (m_updateItemID)
							{
								QueueUpdateOutfitItem(
									a_value.first,
									static_cast<std::uint32_t>(std::distance(list.begin(), it)),
									m_updateItemID);
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}

					ImGui::TableSetColumnIndex(2);
					DrawFormInfoText(*it);

					ImGui::PopID();

					++i;
				}
			}

			void UIOutfitFormManager::DrawFilterTree()
			{
				if (ImGui::TreeNodeEx(
						"filter_tree",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						UIL::LS(CommonStrings::Filter)))
				{
					ImGui::Spacing();
					ImGui::Indent();

					m_filter.Draw();

					ImGui::Unindent();
					ImGui::Spacing();

					ImGui::TreePop();
				}
			}

			void UIOutfitFormManager::DrawProfileTree()
			{
				if (ImGui::TreeNodeEx(
						"tree_prof",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						UIL::LS(CommonStrings::Profile)))
				{
					ImGui::Spacing();

					ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

					profileSelectorParamsOutfitForm_t params{};
					DrawProfileSelector(params);

					ImGui::PopItemWidth();

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}

			void UIOutfitFormManager::DrawFormInfoText(Game::FormID a_form)
			{
				if (auto formInfo = this->LookupForm(a_form))
				{
					if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(formInfo->form.type))
					{
						ImGui::Text(
							"[%s] %s",
							typeDesc,
							formInfo->form.name.c_str());
					}
					else
					{
						ImGui::Text(
							"[%hhu] %s",
							formInfo->form.type,
							formInfo->form.name.c_str());
					}
				}
			}

			void UIOutfitFormManager::QueueCreateOutfitForm(const std::string& a_name)
			{
				ITaskPool::AddPriorityTask([a_name, &ctrl = m_controller] {
					if (!IED::OM::PersistentOutfitFormManager::GetSingleton().CreateForm(a_name))
					{
						ctrl.QueueToast(
							UIL::L(UIOutfitFormManagerStrings::AddFailed),
							UICommon::g_colorError);
					}
				});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			/*void UIOutfitFormManager::QueueRenameOutfit(
				const std::string& a_id,
				const std::string& a_name)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     a_name,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().RenameForm(a_id, a_name.c_str()))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::RenameFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}*/

			void UIOutfitFormManager::QueueAddOutfitItem(
				const std::string& a_id,
				Game::FormID       a_itemid)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     a_itemid,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().AddOutfitItem(a_id, a_itemid))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::AddItemFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueAddOutfitItemList(
				const std::string&                           a_id,
				stl::vector<Data::configFormZeroMissing_t>&& a_list)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     list  = std::move(a_list),
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().AddOutfitItemList(a_id, list))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::AddItemFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueRemoveOutfitForm(
				const std::string& a_id)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     &ctrl = m_controller] {
						Game::FormID removed;

						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().RemoveForm(a_id, removed))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::RemoveFailed),
								UICommon::g_colorError);
						}
						else
						{
							std::uint64_t numcleared = 0;

							const stl::lock_guard lock(ctrl.GetLock());

							auto& conf = ctrl.GetActiveConfig().outfit;

							const auto f = [&](Data::OM::configOutfit_t& a_e) {
								if (a_e.outfit == removed)
								{
									a_e.outfit = {};
									numcleared++;
								}
							};

							conf.visit([&](auto& a_e) {
								f(a_e);
								a_e.visit_overrides(f);
							});

							if (numcleared)
							{
								ctrl.UIReset();
								ctrl.RequestLFEvaluateAll();
								ctrl.QueueToast(
									"Cleared %llu outfit forms",
									UICommon::g_colorWarning,
									numcleared);
							}
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueRemoveOutfitItem(
				const std::string& a_id,
				std::uint32_t      a_index)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     a_index,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().RemoveOutfitItem(a_id, a_index))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::RemoveItemFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueClearOutfitList(
				const std::string& a_id)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().ClearOutfitList(a_id))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::ClearFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueUpdateOutfitItem(
				const std::string& a_id,
				std::uint32_t      a_index,
				Game::FormID       a_newItem)
			{
				ITaskPool::AddPriorityTask(
					[a_id,
				     a_index,
				     a_newItem,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().UpdateOutfitItem(
								a_id,
								a_index,
								a_newItem))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::ClearFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::QueueApplyFormsFromList(
				const OutfitFormListProfile::base_type& a_list)
			{
				ITaskPool::AddPriorityTask(
					[a_list,
				     &ctrl = m_controller] {
						if (!IED::OM::PersistentOutfitFormManager::GetSingleton().ApplyFormsFromList(
								a_list))
						{
							ctrl.QueueToast(
								UIL::L(UIOutfitFormManagerStrings::ApplyFailed),
								UICommon::g_colorError);
						}
					});

				m_flags.set(UIOutfitFormManagerFlags::kRefreshData);
			}

			void UIOutfitFormManager::DrawMenuBar()
			{
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu(UIL::LS(CommonStrings::File, "1")))
					{
						DrawFileMenu();

						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "2")))
					{
						DrawActionsMenu();

						ImGui::EndMenu();
					}

					ImGui::EndMenuBar();
				}
			}

			void UIOutfitFormManager::DrawFileMenu()
			{
				if (ImGui::MenuItem(UIL::LS(CommonStrings::Exit, "1")))
				{
					SetOpenState(false);
				}
			}

			void UIOutfitFormManager::DrawActionsMenu()
			{
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Add, "1")))
				{
					if (DrawDescriptionPopup())
					{
						QueueCreateOutfitForm(GetDescriptionPopupBuffer());

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Serialization, "2")))
				{
					auto& sm = IED::OM::PersistentOutfitFormManager::GetSingleton();

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Read, "1"), nullptr, sm.GetReadEnabled()))
					{
						sm.ToggleRead();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Write, "2"), nullptr, sm.GetWriteEnabled()))
					{
						sm.ToggleWrite();
					}

					ImGui::EndMenu();
				}
			}

			UIPopupQueue& UIOutfitFormManager::GetPopupQueue_ProfileBase() const
			{
				return m_controller.UIGetPopupQueue();
			}

			OutfitFormListProfile::base_type UIOutfitFormManager::GetData(
				const profileSelectorParamsOutfitForm_t& a_data)
			{
				return IED::OM::PersistentOutfitFormManager::GetSingleton().MakeFormList();
			}

			void UIOutfitFormManager::ApplyProfile(
				const profileSelectorParamsOutfitForm_t& a_params,
				const OutfitFormListProfile&             a_profile)
			{
				QueueApplyFormsFromList(a_profile.Data());
			}

			void UIOutfitFormManager::CachedOutfitData::Update()
			{
				auto result = MakeData();

				const stl::lock_guard l(lock);

				data = std::move(result);
			}

			auto UIOutfitFormManager::CachedOutfitData::MakeData()
				-> container_type
			{
				container_type result;

				const auto& sm = IED::OM::PersistentOutfitFormManager::GetSingleton();

				const auto lock1 = sm.GetScopedLock();

				const auto& forms = sm.GetForms();

				result.reserve(forms.size());

				for (auto& e : forms)
				{
					const auto r = result.try_emplace(e.first, e.second->get()->formID);

					const auto& list = e.second->get()->outfitItems;

					r.first->second.list.reserve(list.size());

					for (auto& f : list)
					{
						r.first->second.list.emplace_back(f ? f->formID : Game::FormID{});
					}
				}

				return result;
			}
		}
	}
}

#endif