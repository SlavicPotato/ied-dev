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
			UIFileSelector(a_controller, PATHS::EXPORTS),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_rFileCheck(
				"^[a-zA-Z0-9_\\- \\'\\\"\\,\\.]+$",
				std::regex_constants::ECMAScript)
		{}

		void UIDialogImportExport::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::Begin(
					LS<UIDialogImportExportStrings, 3>(
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
					if (ImGui::Button(LS(CommonStrings::Delete, "1")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Confirm,
								 LS(CommonStrings::Confirm),
								 "%s [%s]",
								 LS(UIDialogImportExportStrings::DeleteConfirm),
								 selected->m_key.c_str())
							.call([this, item = *selected](const auto&) {
								if (!DeleteItem(item))
								{
									auto& queue = m_controller.UIGetPopupQueue();

									queue.push(
										UIPopupType::Message,
										LS(CommonStrings::Error),
										"%s\n\n%s",
										LS(UIDialogImportExportStrings::DeleteError),
										GetLastException().what());
								}
							});
					}

					ImGui::SameLine();
					if (ImGui::Button(LS(CommonStrings::Rename, "2")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Input,
								 LS(CommonStrings::Rename),
								 "%s",
								 LS(UIDialogImportExportStrings::RenamePrompt))
							.call([this, item = *selected](const auto& a_p) {
								std::string file(a_p.GetInput());

								if (file.empty())
								{
									return;
								}

								if (!CheckFileName(file))
								{
									auto& queue = m_controller.UIGetPopupQueue();

									queue.push(
										UIPopupType::Message,
										LS(CommonStrings::Error),
										LS(UIWidgetCommonStrings::IllegalFilename));
								}
								else
								{
									try
									{
										fs::path name(file);
										name += ".json";

										if (!RenameItem(item, name))
										{
											auto& queue = m_controller.UIGetPopupQueue();

											queue.push(
												UIPopupType::Message,
												LS(CommonStrings::Error),
												"%s\n\n%s",
												LS(UIDialogImportExportStrings::RenameError),
												GetLastException().what());
										}
									}
									catch (std::exception& e)
									{
										auto& queue = m_controller.UIGetPopupQueue();

										queue.push(
											UIPopupType::Message,
											LS(CommonStrings::Error),
											"%s",
											e.what());
									}
								}
							});
					}

					ImGui::Separator();

					auto& conf = m_controller.GetConfigStore().settings;

					conf.mark_if(ImGui::CheckboxFlagsT(
						LS(UIDialogImportExportStrings::SkipTempRefs, "3"),
						stl::underlying(std::addressof(conf.data.ui.importExport.importFlags.value)),
						stl::underlying(ImportFlags::kEraseTemporary)));

					DrawTip(UITip::SkipTempRefs);
				}

				ImGui::PopTextWrapPos();

				ImGui::Separator();

				if (selected)
				{
					if (ImGui::Button(LS(CommonStrings::Import, "4"), { 120.f, 0.f }))
					{
						auto data = std::make_shared<Data::configStore_t>();

						Serialization::ParserState state;

						if (!m_controller.LoadConfigStore(selected->m_fullpath, *data, state))
						{
							auto& queue = m_controller.UIGetPopupQueue();

							queue.push(
								UIPopupType::Message,
								LS(CommonStrings::Error),
								"%s",
								m_controller.JSGetLastException().what());
						}
						else
						{
							auto& queue = m_controller.UIGetPopupQueue();

							queue.push(
									 UIPopupType::Confirm,
									 LS(CommonStrings::Confirm),
									 "%s [%s]",
									 LS(UIDialogImportExportStrings::ImportConfirm),
									 selected->m_key.c_str())
								.draw([this, state = std::move(state)] {
									if (state.has_errors())
									{
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.0f);
										ImGui::TextColored(
											UICommon::g_colorWarning,
											"%s",
											LS(UIDialogImportExportStrings::ImportHasErrorsWarning));
										ImGui::PopTextWrapPos();

										ImGui::Separator();
									}

									auto& conf = m_controller.GetConfigStore().settings;

									conf.mark_if(DrawExportFilters(conf.data.ui.importExport.exportFlags));

									ImGui::Separator();
									ImGui::Spacing();

									ImGui::PushID("mode_sel");

									if (ImGui::RadioButton(
											LS(CommonStrings::Overwrite, "1"),
											!conf.data.ui.importExport.importFlags.test(ImportFlags::kMerge)))
									{
										conf.data.ui.importExport.importFlags.clear(ImportFlags::kMerge);
										conf.mark_dirty();
									}

									ImGui::SameLine();

									if (ImGui::RadioButton(
											LS(CommonStrings::Merge, "2"),
											conf.data.ui.importExport.importFlags.test(ImportFlags::kMerge)))
									{
										conf.data.ui.importExport.importFlags.set(ImportFlags::kMerge);
										conf.mark_dirty();
									}

									DrawTip(UITip::ImportMode);

									ImGui::PopID();

									ImGui::Spacing();

									return conf.data.ui.importExport.exportFlags.test_any(Data::ConfigStoreSerializationFlags::kAll);
								})
								.call([this, data = std::move(data)](const auto&) mutable {
									auto& conf = m_controller.GetConfigStore().settings;
									DoImport(std::move(*data), conf.data.ui.importExport.importFlags);
								});
						}
					}

					ImGui::SameLine();
				}

				if (ImGui::Button(LS(CommonStrings::Export, "5"), { 120.f, 0.f }))
				{
					ImGui::OpenPopup("__export_ctx");
				}

				DrawExportContextMenu();

				ImGui::SameLine();
				if (ImGui::Button(LS(CommonStrings::Close, "6"), { 120.f, 0.f }))
				{
					SetOpenState(false);
				}
			}

			ImGui::End();
		}

		void UIDialogImportExport::DrawExportContextMenu()
		{
			if (ImGui::BeginPopup("__export_ctx"))
			{
				if (ImGui::MenuItem(LS(CommonStrings::New, "1")))
				{
					auto& queue = m_controller.UIGetPopupQueue();

					queue.push(
							 UIPopupType::Input,
							 LS(UIDialogImportExportStrings::ExportToFile),
							 "%s",
							 LS(UIDialogImportExportStrings::ExportConfirm))
						.draw([this] {
							auto& conf = m_controller.GetConfigStore().settings;

							conf.mark_if(DrawExportFilters(conf.data.ui.importExport.exportFlags));

							return conf.data.ui.importExport.exportFlags.test_any(Data::ConfigStoreSerializationFlags::kAll);
						})
						.call([this](const auto& a_p) {
							std::string file(a_p.GetInput());

							if (file.empty())
							{
								return;
							}

							if (!CheckFileName(file))
							{
								auto& queue = m_controller.UIGetPopupQueue();

								queue.push(
									UIPopupType::Message,
									LS(CommonStrings::Error),
									"%s",
									LS(UIWidgetCommonStrings::IllegalFilename));
							}
							else
							{
								fs::path path = PATHS::EXPORTS;
								path /= file;
								path += ".json";

								DoExport(path);
							}
						});
				}

				auto& selected = GetSelected();
				if (selected)
				{
					ImGui::Separator();

					if (ImGui::MenuItem(LS(UIDialogImportExportStrings::OverwriteSelected, "2")))
					{
						auto& queue = m_controller.UIGetPopupQueue();

						queue.push(
								 UIPopupType::Confirm,
								 LS(UIDialogImportExportStrings::ExportToFile),
								 "%s [%s]",
								 LS(UIDialogImportExportStrings::OverwriteConfirm),
								 selected->m_key.c_str())
							.draw([this] {
								auto& conf = m_controller.GetConfigStore().settings;

								conf.mark_if(DrawExportFilters(conf.data.ui.importExport.exportFlags));

								return conf.data.ui.importExport.exportFlags.test_any(Data::ConfigStoreSerializationFlags::kAll);
							})
							.call([this, path = selected->m_fullpath](auto&) {
								DoExport(path);
							});
					}
				}

				ImGui::EndPopup();
			}
		}

		void UIDialogImportExport::DoImport(
			Data::configStore_t&& a_data,
			stl::flag<ImportFlags> a_flags)
		{
			auto& conf = m_controller.GetConfigStore().settings.data;

			if (m_controller.ImportData(std::move(a_data), a_flags))
			{
				m_controller.UIReset();
				SetOpenState(false);
			}
			else
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					LS(CommonStrings::Error),
					"%s\n\n%s",
					LS(UIDialogImportExportStrings::ImportError),
					m_controller.JSGetLastException().what());
			}
		}

		void UIDialogImportExport::DoExport(const fs::path& a_path)
		{
			try
			{
				auto& conf = m_controller.GetConfigStore().settings;

				if (!m_controller.ExportData(a_path, conf.data.ui.importExport.exportFlags))
				{
					auto& queue = m_controller.UIGetPopupQueue();

					queue.push(
						UIPopupType::Message,
						LS(CommonStrings::Error),
						"%s\n\n%s",
						LS(UIDialogImportExportStrings::ExportError),
						m_controller.JSGetLastException().what());
				}
				else
				{
					if (DoUpdate(false))
					{
						auto file = a_path.filename().stem().string();
						if (HasFile(file))
						{
							SelectItem(file);
						}
					}
				}
			}
			catch (const std::exception& e)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					LS(CommonStrings::Error),
					"%s",
					e.what());
			}
		}

		void UIDialogImportExport::OnOpen()
		{
			DoUpdate(true);
		}

		bool UIDialogImportExport::DoUpdate(bool a_select)
		{
			if (!UpdateFileList(a_select))
			{
				auto& queue = m_controller.UIGetPopupQueue();
				queue.push(
					UIPopupType::Message,
					LS(CommonStrings::Error),
					"%s\n\n%s",
					LS(UIDialogImportExportStrings::FileListError),
					GetLastException().what());

				return false;
			}

			return true;
		}

		bool UIDialogImportExport::CheckFileName(const std::string& a_path) const
		{
			try
			{
				return std::regex_match(a_path, m_rFileCheck);
			}
			catch (const std::exception&)
			{
				return false;
			}
		}
	}  // namespace UI
}  // namespace IED