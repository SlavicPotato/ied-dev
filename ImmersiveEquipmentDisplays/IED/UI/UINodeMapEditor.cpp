#include "pch.h"

#include "UINodeMapEditor.h"

#include "PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"
#include "IED/NodeMap.h"

#include "Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		UINodeMapEditor::UINodeMapEditor(Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		void UINodeMapEditor::Draw()
		{
			SetWindowDimensions(0.0f, 800.0f, 600.0f, true);

			if (ImGui::Begin(
					UIL::LS<CommonStrings, 3>(CommonStrings::Nodes, WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();
				DrawNodeTable();
			}

			ImGui::End();
		}

		void UINodeMapEditor::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (UIL::LCG_BM(CommonStrings::File, "1"))
				{
					if (UIL::LCG_MI(CommonStrings::New, "1"))
					{
						QueueNewNodePopup();
					}

					ImGui::Separator();

					if (UIL::LCG_MI(CommonStrings::Close, "2"))
					{
						SetOpenState(false);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

		void UINodeMapEditor::DrawNodeTable()
		{
			auto& map  = Data::NodeMap::GetSingleton();
			auto& data = map.GetData();

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5, 5 });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_RowBg |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0 }))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Node), ImGuiTableColumnFlags_None, 0.25f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Description), ImGuiTableColumnFlags_None, 0.65f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Actions), ImGuiTableColumnFlags_None, 0.1f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				for (auto& e : data.getvec())
				{
					if (!e->second.flags.test(Data::NodeDescriptorFlags::kUserNode))
					{
						continue;
					}

					ImGui::PushID(e);

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);

					ImGui::TextUnformatted(e->first.c_str());

					ImGui::TableSetColumnIndex(1);

					if (DrawColumn(1, e->first, e->second.name.c_str()))
					{
						auto buffer = GetBuffer();
						if (std::strlen(buffer))
						{
							e->second.name = buffer;
							OnChange();
						}
					}

					ImGui::TableSetColumnIndex(2);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

					if (ImGui::Selectable(UIL::LS(CommonStrings::Delete, "ctl_1")))
					{
						QueueDeleteNodePopup(e->first);
					}

					ImGui::PopStyleVar();

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UINodeMapEditor::QueueNewNodePopup()
		{
			auto& queue = m_controller.UIGetPopupQueue();

			queue.push(
					 UIPopupType::Input,
					 UIL::LS(CommonStrings::New),
					 "%s",
					 UIL::LS(UIWidgetCommonStrings::NewNodePrompt))
				.call([this](const auto& a_p) {
					auto& name = a_p.GetInput();

					if (!Data::NodeMap::ValidateNodeName(name))
					{
						m_controller.UIGetPopupQueue().push(
							UIPopupType::Message,
							UIL::LS(CommonStrings::Error),
							"%s",
							UIL::LS(UIWidgetCommonStrings::InvalidNode));

						return;
					}

					auto& map = Data::NodeMap::GetSingleton();

					auto r = map.Add(
						name,
						name,
						Data::NodeDescriptorFlags::kUserNode);

					if (!r.second)
					{
						m_controller.UIGetPopupQueue().push(
							UIPopupType::Message,
							UIL::LS(CommonStrings::Error),
							"%s [%s]",
							UIL::LS(UIWidgetCommonStrings::NodeExistsError),
							name.c_str());
					}
					else
					{
						OnChange();
					}
				});
		}

		void UINodeMapEditor::QueueDeleteNodePopup(
			const stl::fixed_string& a_node)
		{
			auto& queue = m_controller.UIGetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s [%s]",
					 UIL::LS(UIWidgetCommonStrings::DeleteNodePrompt),
					 a_node.c_str())
				.call([this, a_node](const auto&) {
					auto& map = Data::NodeMap::GetSingleton();

					if (!map.RemoveUserNode(a_node))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
							UIPopupType::Message,
							UIL::LS(CommonStrings::Error),
							"%s [%s]",
							UIL::LS(UIWidgetCommonStrings::NodeEraseError),
							a_node.c_str());
					}
					else
					{
						OnChange();
					}
				});
		}

		void UINodeMapEditor::OnChange()
		{
			auto& map = Data::NodeMap::GetSingleton();
			map.SetDirty();

			ITaskPool::AddTask([&controller = m_controller] {
				const stl::lock_guard lock(controller.GetLock());

				auto& map = Data::NodeMap::GetSingleton();
				if (!map.SaveUserNodes(PATHS::NODEMAP))
				{
					auto& queue = controller.UIGetPopupQueue();
					queue.push(
						UIPopupType::Message,
						UIL::LS(CommonStrings::Error),
						"%s [%s]\n\n%s",
						UIL::LS(UIWidgetCommonStrings::DataSaveError),
						PATHS::NODEMAP,
						map.GetLastException().what());
				}
			});
		}
	}
}