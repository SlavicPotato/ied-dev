#include "pch.h"

#include "UIAboutModal.h"

#include "UICommon.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

#include "UIAboutModalStrings.h"

#include "gitparams.h"
#include "plugin.h"
#include "version.h"

#define LINK_URI "https://"

namespace IED
{
	namespace UI
	{
		UIAboutModal::UIAboutModal(
			Controller& a_controller) :
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIAboutModal::QueueAboutPopup()
		{
			auto& queue = m_controller.UIGetPopupQueue();

			queue.push(
					 UIPopupType::Custom,
					 PLUGIN_NAME_FULL)
				.draw([this] {

					if (ImGui::BeginTable(
							"1",
							2,
							ImGuiTableFlags_NoSavedSettings |
								ImGuiTableFlags_SizingStretchProp,
							{ -1.0f, 0.0f }))
					{
						ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.2f);
						ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.8f);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", LS(CommonStrings::Version));

						ImGui::TableSetColumnIndex(1);
						ImGui::Text(
							"%s-%.8x [%s]",
							PLUGIN_VERSION_VERSTRING,
							MK_GIT_COMMIT(GIT_CUR_COMMIT),
							MK_STRING(GIT_BRANCH));

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", LS(CommonStrings::Author));

						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%s", PLUGIN_AUTHOR);
						
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", "URL");

						ImGui::TableSetColumnIndex(1);
						UICommon::DrawURL(PLUGIN_URL, LINK_URI PLUGIN_URL);
						
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", "GIT");

						ImGui::TableSetColumnIndex(1);
						UICommon::DrawURL(PLUGIN_URL_DEV, LINK_URI PLUGIN_URL_DEV);

						ImGui::EndTable();
					}

					ImGui::Spacing();
					ImGui::Separator();

					return ImGui::Button(
						LS(CommonStrings::Close, "2"),
						{ -1.0f, 0.0f });
				});
		}
	}
}