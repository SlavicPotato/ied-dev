#include "pch.h"

#include "UIDialogImportExport.h"

#include "PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

#include "UIDialogImportExportStrings.h"
#include "Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		using namespace UICommon;

		UIDialogImportExport::UIDialogImportExport(
			Controller& a_controller) :
			UIFileSelector(PATHS::EXPORTS, ".json"),
			UIImportWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UIDialogImportExport::Draw()
		{
			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::Begin(
					UIL::LS<UIDialogImportExportStrings, 3>(
						UIDialogImportExportStrings::ImportExport,
						WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				DrawFileSelector();

				auto& selected = GetSelected();

				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

				if (selected)
				{
					if (ImGui::Button(UIL::LS(CommonStrings::Delete, "1")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Confirm,
								 UIL::LS(CommonStrings::Confirm),
								 "%s [%s]",
								 UIL::LS(UIDialogImportExportStrings::DeleteConfirm),
								 selected->m_key.c_str())
							.call([this, item = *selected](const auto&) {
								if (!DeleteItem(item))
								{
									auto& queue = m_controller.UIGetPopupQueue();

									queue.push(
										UIPopupType::Message,
										UIL::LS(CommonStrings::Error),
										"%s\n\n%s",
										UIL::LS(UIDialogImportExportStrings::DeleteError),
										GetLastException().what());
								}
							});
					}

					ImGui::SameLine();
					if (ImGui::Button(UIL::LS(CommonStrings::Rename, "2")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Input,
								 UIL::LS(CommonStrings::Rename),
								 "%s",
								 UIL::LS(UIDialogImportExportStrings::RenamePrompt))
							.fmt_input("%s", selected->m_key.c_str())
							.call([this, item = *selected](const auto& a_p) {
								std::string file(a_p.GetInput());

								if (file.empty())
								{
									return;
								}

								try
								{
									fs::path name(str_conv::str_to_wstr(file));
									name += ".json";

									if (!RenameItem(item, name))
									{
										auto& queue = m_controller.UIGetPopupQueue();

										queue.push(
											UIPopupType::Message,
											UIL::LS(CommonStrings::Error),
											"%s\n\n%s",
											UIL::LS(UIDialogImportExportStrings::RenameError),
											GetLastException().what());
									}
								}
								catch (std::exception& e)
								{
									auto& queue = m_controller.UIGetPopupQueue();

									queue.push(
										UIPopupType::Message,
										UIL::LS(CommonStrings::Error),
										"%s: %s",
										__FUNCTION__,
										e.what());
								}
							});
					}

					ImGui::SameLine();
					if (ImGui::Button(UIL::LS(CommonStrings::Refresh, "3")))
					{
						UpdateFileList();
					}
				}

				ImGui::PopTextWrapPos();

				ImGui::Separator();
				ImGui::Spacing();

				if (selected)
				{
					if (ImGui::Button(UIL::LS(CommonStrings::Import, "4"), { 120.f, 0.f }))
					{
						QueueImportPopup(selected->m_fullpath, selected->m_key);
					}

					ImGui::SameLine();
				}

				if (ImGui::Button(UIL::LS(CommonStrings::Export, "5"), { 120.f, 0.f }))
				{
					ImGui::OpenPopup("__export_ctx");
				}

				DrawExportContextMenu();

				ImGui::SameLine();
				if (ImGui::Button(UIL::LS(CommonStrings::Close, "6"), { 120.f, 0.f }))
				{
					SetOpenState(false);
				}
			}

			ImGui::End();
		}

		void UIDialogImportExport::OnDataImport(bool a_success)
		{
			if (a_success)
			{
				m_controller.UIReset();
			}
			else
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					UIL::LS(UIDialogImportExportStrings::ImportError),
					m_controller.JSGetLastException().what());
			}
		}

		void UIDialogImportExport::DrawExportContextMenu()
		{
			if (ImGui::BeginPopup("__export_ctx"))
			{
				if (ImGui::MenuItem(UIL::LS(CommonStrings::New, "1")))
				{
					auto& queue = m_controller.UIGetPopupQueue();

					queue.push(
							 UIPopupType::Input,
							 UIL::LS(UIDialogImportExportStrings::ExportToFile),
							 "%s",
							 UIL::LS(UIDialogImportExportStrings::ExportConfirm))
						.draw([this] {
							auto& conf = m_controller.GetSettings();

							conf.mark_if(DrawExportFilters(conf.data.ui.importExport.serializationFlags));

							return conf.data.ui.importExport.serializationFlags.test_any(Data::ConfigStoreSerializationFlags::kAll);
						})
						.call([this](const auto& a_p) {
							std::string file(a_p.GetInput());

							if (file.empty())
							{
								return;
							}

							try
							{
								fs::path path(PATHS::EXPORTS);
								auto     fn = fs::path(str_conv::str_to_wstr(file)).filename();
								if (!fn.has_filename())
								{
									throw std::exception("bad filename");
								}
								path /= fn;
								path += ".json";

								DoExport(path);
							}
							catch (const std::exception& e)
							{
								auto& queue = m_controller.UIGetPopupQueue();

								queue.push(
									UIPopupType::Message,
									UIL::LS(CommonStrings::Error),
									"%s: %s",
									__FUNCTION__,
									e.what());
							}
						})
						.set_text_wrap_size(23.f);
				}

				auto& selected = GetSelected();
				if (selected)
				{
					ImGui::Separator();

					if (ImGui::MenuItem(UIL::LS(UIDialogImportExportStrings::OverwriteSelected, "2")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Confirm,
								 UIL::LS(UIDialogImportExportStrings::ExportToFile),
								 "%s [%s]",
								 UIL::LS(UIDialogImportExportStrings::OverwriteConfirm),
								 selected->m_key.c_str())
							.draw([this] {
								auto& conf = m_controller.GetSettings();

								conf.mark_if(DrawExportFilters(conf.data.ui.importExport.serializationFlags));

								return conf.data.ui.importExport.serializationFlags.test_any(Data::ConfigStoreSerializationFlags::kAll);
							})
							.call([this, path = selected->m_fullpath](auto&) {
								DoExport(path);
							})
							.set_text_wrap_size(23.f);
					}
				}

				ImGui::EndPopup();
			}
		}

		void UIDialogImportExport::DoExport(const fs::path& a_path)
		{
			try
			{
				const auto& settings = m_controller.GetSettings();

				if (!m_controller.ExportData(
						a_path,
						ExportFlags::kNone,
						settings.data.ui.importExport.serializationFlags))
				{
					auto& queue = m_controller.UIGetPopupQueue();

					queue.push(
						UIPopupType::Message,
						UIL::LS(CommonStrings::Error),
						"%s\n\n%s",
						UIL::LS(UIDialogImportExportStrings::ExportError),
						m_controller.JSGetLastException().what());
				}
				else
				{
					if (DoUpdate())
					{
						SelectItem(str_conv::wstr_to_str(a_path.filename().stem().wstring()));
					}
				}
			}
			catch (const std::exception& e)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s: %s",
					__FUNCTION__,
					e.what());
			}
		}

		void UIDialogImportExport::OnOpen()
		{
			DoUpdate();
		}

		bool UIDialogImportExport::DoUpdate()
		{
			if (!UpdateFileList())
			{
				auto& queue = m_controller.UIGetPopupQueue();
				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					UIL::LS(UIDialogImportExportStrings::FileListError),
					GetLastException().what());

				return false;
			}

			return true;
		}

	}
}