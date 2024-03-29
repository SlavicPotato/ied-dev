#include "pch.h"

#include "UIAboutModal.h"

#include "UICommon.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

#include "UIAboutModalStrings.h"

#include "gitinfo.h"
#include "plugin.h"
#include "version.h"

#define LINK_URI "https://"

namespace IED
{
	namespace UI
	{
		UIAboutModal::UIAboutModal(
			Controller& a_controller) :
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
						ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
						ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::Version));

						ImGui::TableSetColumnIndex(1);
						ImGui::Text(
							"%s [%.8x-%s]",
							PLUGIN_VERSION_VERSTRING,
							MK_GIT_COMMIT(GIT_CUR_COMMIT),
							GIT_BRANCH);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::Author));

						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%s", PLUGIN_AUTHOR);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("URL");

						ImGui::TableSetColumnIndex(1);
						UICommon::DrawURL(PLUGIN_URL, LINK_URI PLUGIN_URL);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("GIT");

						ImGui::TableSetColumnIndex(1);
						UICommon::DrawURL(PLUGIN_URL_DEV, LINK_URI PLUGIN_URL_DEV);

						ImGui::EndTable();
					}

					ImGui::Separator();

					if (ImGui::BeginTable(
							"2",
							2,
							ImGuiTableFlags_NoSavedSettings |
								ImGuiTableFlags_SizingStretchProp,
							{ -1.0f, 0.0f }))
					{
						ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.25f);
						ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.75f);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", UIL::LS(CommonStrings::Dependencies));

						ImGui::TableSetColumnIndex(1);

#if defined(IED_MIMALLOC_IN_USE)

						UICommon::DrawURL(
							"mimalloc %u.%u",
							DEP_URL_MIMALLOC,
							MI_MALLOC_VERSION / 100,
							MI_MALLOC_VERSION % 100);

#endif

						UICommon::DrawURL(
							"boost %u.%u.%u",
							DEP_URL_BOOST,
							BOOST_VERSION / 100000,
							BOOST_VERSION / 100 % 1000,
							BOOST_VERSION % 100);

						UICommon::DrawURL(
							"jsoncpp %u.%u.%u",
							DEP_URL_JSONCPP,
							JSONCPP_VERSION_MAJOR,
							JSONCPP_VERSION_MINOR,
							JSONCPP_VERSION_PATCH);

						UICommon::DrawURL(
							"ImGui %s (%u)",
							DEP_URL_IMGUI,
							IMGUI_VERSION,
							IMGUI_VERSION_NUM);

#if defined(IED_ENABLE_I3DI)

						ImGui::Text(
							"assimp %u.%u.%u-%u",
							aiGetVersionMajor(),
							aiGetVersionMinor(),
							aiGetVersionPatch(),
							aiGetVersionRevision());

#endif

						ImGui::EndTable();
					}

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					return ImGui::Button(
						UIL::LS(CommonStrings::Close, "C"),
						{ -1.0f, 0.0f });
				});
		}
	}
}