#include "pch.h"

#include "UINodeMonitorSelectorWidget.h"

#include "IED/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		UINodeMonitorSelectorWidget::UINodeMonitorSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UINodeMonitorSelectorWidget::DrawNodeMonitorSelector(
			std::uint32_t& a_uid) const
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::ID, "nmon_sel"),
					get_nodemon_desc(a_uid),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : NodeOverrideData::GetNodeMonitorEntries())
				{
					ImGui::PushID(e.first);

					bool selected = (e.first == a_uid);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LMKID<3>(get_nodemon_desc(e.second), "0"),
							selected))
					{
						a_uid  = e.first;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UINodeMonitorSelectorWidget::get_nodemon_desc(
			std::uint32_t a_uid) const
		{
			auto& data = NodeOverrideData::GetNodeMonitorEntries();

			if (data.empty())
			{
				return LS(CommonStrings::NoData);
			}

			auto it = data.find(a_uid);
			if (it != data.end())
			{
				return get_nodemon_desc(it->second);
			}
			else
			{
				stl::snprintf(
					m_buf,
					"(%s) [%.8X]",
					LS(CommonStrings::Unknown),
					a_uid);

				return m_buf;
			}

		}

		const char* UINodeMonitorSelectorWidget::get_nodemon_desc(
			const Data::configNodeMonitorEntryBS_t& a_entry) const
		{
			stl::snprintf(
				m_buf,
				"[%.8X] %s",
				a_entry.data.uid,
				a_entry.data.description.c_str());

			return m_buf;
		}
	}
}