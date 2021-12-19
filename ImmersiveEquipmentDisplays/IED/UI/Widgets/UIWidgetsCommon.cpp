#include "pch.h"

#include "UIWidgetsCommon.h"

#include "../UICommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		sexInfo_t GetOppositeSex2(Data::ConfigSex a_sex)
		{
			if (a_sex == Data::ConfigSex::Female)
			{
				return { Data::ConfigSex::Male, "male" };
			}
			else
			{
				return { Data::ConfigSex::Female, "female" };
			}
		}

		void DrawConfigClassInUse(Data::ConfigClass a_class)
		{
			switch (a_class)
			{
			case Data::ConfigClass::Race:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightOrange);
				break;
			case Data::ConfigClass::NPC:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorPurple);
				break;
			case Data::ConfigClass::Actor:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLimeGreen);
				break;
			default:
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
				break;
			}

			ImGui::Text("%s", Data::GetConfigClassName(a_class));

			ImGui::PopStyleColor();
		};

	}  // namespace UI
}  // namespace IED