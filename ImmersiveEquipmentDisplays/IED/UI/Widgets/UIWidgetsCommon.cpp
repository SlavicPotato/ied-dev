#include "pch.h"

#include "UIWidgetsCommon.h"

#include "../UICommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		void DrawConfigClassInUse(Data::ConfigClass a_class)
		{
			switch (a_class)
			{
			case Data::ConfigClass::Race:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorPurple);
				break;
			case Data::ConfigClass::NPC:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightOrange);
				break;
			case Data::ConfigClass::Actor:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLimeGreen);
				break;
			default:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
				break;
			}

			ImGui::TextUnformatted(Data::GetConfigClassName(a_class));

			ImGui::PopStyleColor();
		};

	}
}