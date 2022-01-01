#include "pch.h"

#include "UIAboutModal.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

#include "UIAboutModalStrings.h"

#include "gitparams.h"
#include "plugin.h"
#include "version.h"

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
					//ImGui::Spacing();

					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 60.0f);

					ImGui::Text(
						"%s: %s-%.8x [%s]",
						LS(CommonStrings::Version),
						PLUGIN_VERSION_VERSTRING,
						MK_GIT_COMMIT(GIT_CUR_COMMIT),
						MK_STRING(GIT_BRANCH));

					ImGui::Spacing();

					ImGui::Text(
						"%s: %s",
						LS(CommonStrings::Author),
						PLUGIN_AUTHOR);

					ImGui::Spacing();

					ImGui::Text("URL: %s", PLUGIN_URL);

					ImGui::Spacing();

					ImGui::Text("GIT: %s", PLUGIN_URL_DEV);

					//ImGui::PopTextWrapPos();

					ImGui::Spacing();
					ImGui::Separator();

					return ImGui::Button(
						LS(CommonStrings::Close, "1"),
						{ -1.0f, 0.0f });
				});
		}
	}
}