#include "pch.h"

#include "UIImportWidget.h"

#include "IED/UI/UIDialogImportExportStrings.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIImportWidget::UIImportWidget(Controller& a_controller) :
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIImportWidget::QueueImportPopup(
			const fs::path& a_fullpath,
			const stl::fixed_string& a_key)
		{
			auto data = std::make_shared<Data::configStore_t>();

			Serialization::ParserState state;

			if (!m_controller.LoadConfigStore(a_fullpath, *data, state))
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
						 a_key.c_str())
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
					})
					.set_text_wrap_size(23.f);
			}
		}

		
		void UIImportWidget::DoImport(
			Data::configStore_t&& a_data,
			stl::flag<ImportFlags> a_flags)
		{
			OnDataImport(m_controller.ImportData(std::move(a_data), a_flags));
		}

	}
}
