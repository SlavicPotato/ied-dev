#include "pch.h"

#include "UIGeneralInfo.h"

#include "UIActorInfoStrings.h"
#include "UIGeneralInfoStrings.h"
#include "Widgets/UIDayOfWeekSelectorWidget.h"
#include "Widgets/UITimeOfDaySelectorWidgetStrings.h"
#include "Widgets/UIWidgetCommonStrings.h"

#include "UICommon.h"

#include "UILocalizationInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIGeneralInfo::UIGeneralInfo(
			Controller& a_controller) :
			UIFormInfoTooltipWidget(a_controller),
			UIMiscTextInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIGeneralInfo::Draw()
		{
			SetWindowDimensions(0.0f, 500.0f, 600.0f, true);

			if (ImGui::Begin(
					UIL::LS<UIWidgetCommonStrings, 3>(
						UIWidgetCommonStrings::GeneralInfo,
						WINDOW_ID),
					GetOpenState()))
			{
				DrawGeneralInfo();
			}

			ImGui::End();
		}

		void UIGeneralInfo::OnOpen()
		{
			auto tmp = stl::make_smart_for_overwrite<Data>();
			QueueInfoUpdate(tmp);

			m_data = std::move(tmp);
		}

		void UIGeneralInfo::OnClose()
		{
			Reset();
		}

		void UIGeneralInfo::DrawGeneralInfo()
		{
			if (!m_data)
			{
				return;
			}

			const stl::lock_guard lock(m_data->lock);

			if (!m_data->initialized)
			{
				ImGui::TextUnformatted(
					UIL::LS(UIActorInfoStrings::DataUnavailable));

				return;
			}

			UpdateData(m_data);

			DrawContents(*m_data);
		}

		void UIGeneralInfo::UpdateData(
			const stl::smart_ptr<Data>& a_data)
		{
			if (IPerfCounter::delta_us(
					a_data->lastUpdate,
					IPerfCounter::Query()) > 100000)
			{
				QueueInfoUpdate(a_data);
			}
		}

		void UIGeneralInfo::QueueInfoUpdate(
			const stl::smart_ptr<Data>& a_data)
		{
			ITaskPool::AddTask([data = a_data] {
				const stl::lock_guard datalock(data->lock);

				data->general.Update();

				data->lastUpdate  = IPerfCounter::Query();
				data->initialized = true;
			});
		}

		void UIGeneralInfo::DrawContents(const Data& a_data)
		{
			DrawGeneralTree(a_data);
			DrawCalendarTree(a_data);
			DrawSkyTree(a_data);
		}

		template <
			class T,
			class... Args>
		void draw_row(
			T           a_strid,
			const char* a_format,
			Args... a_args)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s:", UIL::LS(a_strid));

			ImGui::TableSetColumnIndex(1);
			ImGui::Text(a_format, a_args...);
		}

		void UIGeneralInfo::DrawGeneralTree(const Data& a_data)
		{
			auto& data = a_data.general;

			if (ImGui::TreeNodeEx(
					"tree_gen",
					ImGuiTreeNodeFlags_CollapsingHeader |
						ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UIGeneralInfoStrings::Timing)))
			{
				ImGui::Spacing();

				if (ImGui::BeginTable(
						"ct_gen",
						2,
						ImGuiTableFlags_NoSavedSettings |
							ImGuiTableFlags_SizingStretchProp,
						{ -1.0f, 0.0f }))
				{
					ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.5f);
					ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.5f);

					draw_row(UIGeneralInfoStrings::FrameTimer, "%f", data.frameTimer);
					draw_row(UIGeneralInfoStrings::FrameTimerSlow, "%f", data.frameTimerSlow);
					draw_row(UIGeneralInfoStrings::GlobalTimeMultiplier, "%f", data.globalTimeMultiplier);
					draw_row(UIGeneralInfoStrings::WorldTimeMultiplier, "%f", data.worldTimeMultiplier);
					draw_row(UIGeneralInfoStrings::PlayerTimeMultiplier, "%f", data.playerTimeMultiplier);

					ImGui::EndTable();
				}

				ImGui::Spacing();
			}
		}

		void UIGeneralInfo::DrawCalendarTree(const Data& a_data)
		{
			auto& data = a_data.general;

			if (ImGui::TreeNodeEx(
					"tree_cal",
					ImGuiTreeNodeFlags_CollapsingHeader |
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UIGeneralInfoStrings::Calendar)))
			{
				ImGui::Spacing();

				if (ImGui::BeginTable(
						"ct_cal",
						2,
						ImGuiTableFlags_NoSavedSettings |
							ImGuiTableFlags_SizingStretchProp,
						{ -1.0f, 0.0f }))
				{
					ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.5f);
					ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.5f);

					draw_row(CommonStrings::Day, "%.4f", data.day);
					draw_row(UIGeneralInfoStrings::DayOfWeek, "%d [%s]", data.dayOfWeek, UIDayOfWeekSelectorWidget::day_of_week_to_desc(data.dayOfWeek));
					draw_row(CommonStrings::Month, "%u", data.month);
					draw_row(CommonStrings::Year, "%u", data.year);
					draw_row(UIGeneralInfoStrings::DaysPassed, "%.4f", data.daysPassed);
					draw_row(UIGeneralInfoStrings::Timescale, "%f", data.timescale);

					ImGui::EndTable();
				}

				ImGui::Spacing();
			}
		}

		static auto get_tod_desc(Data::TimeOfDay a_tod)
		{
			switch (a_tod)
			{
			case Data::TimeOfDay::kDay:
				return UIL::LS(UITimeOfDaySelectorWidgetStrings::Day);
			case Data::TimeOfDay::kNight:
				return UIL::LS(UITimeOfDaySelectorWidgetStrings::Night);
			case Data::TimeOfDay::kSunrise:
				return UIL::LS(UITimeOfDaySelectorWidgetStrings::Sunrise);
			case Data::TimeOfDay::kSunset:
				return UIL::LS(UITimeOfDaySelectorWidgetStrings::Sunset);
			default:
				return UIL::LS(CommonStrings::None);
			}
		}

		void UIGeneralInfo::DrawSkyTree(const Data& a_data)
		{
			auto& data = a_data.general;

			if (ImGui::TreeNodeEx(
					"tree_clmt",
					ImGuiTreeNodeFlags_CollapsingHeader |
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UIGeneralInfoStrings::ClimateAndLighting)))
			{
				ImGui::Spacing();

				if (ImGui::BeginTable(
						"ct_clmt",
						2,
						ImGuiTableFlags_NoSavedSettings |
							ImGuiTableFlags_SizingStretchProp,
						{ -1.0f, 0.0f }))
				{
					ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_None, 0.5f);
					ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_None, 0.5f);

					draw_row(UIGeneralInfoStrings::TimeOfDay, "%s", get_tod_desc(data.timeOfDay.first));

					ImGui::SameLine();
					ImGui::TextUnformatted(data.timeOfDay.second ? "[AH]" : "[BH]");

					draw_form_row(UIGeneralInfoStrings::CurrentWeather, data.currentWeather);
					draw_form_row(UIGeneralInfoStrings::LastWeather, data.lastWeather);

					draw_row(UIGeneralInfoStrings::WeatherTransition, "%f", data.weatherTrans);

					draw_form_row(UIGeneralInfoStrings::Room, data.room);
					draw_form_row(UIGeneralInfoStrings::RoomLightingTemplate, data.roomLightingTemplate, true);

					draw_row(UIGeneralInfoStrings::SunAngle, "%.3f deg", data.sunAngle * 180.0f / std::numbers::pi_v<float>);
					draw_row(UIGeneralInfoStrings::ExteriorAmbientLightLevel, "%.4f / %.4f", data.exteriorAmbientLightLevel, data.torchLightLevel);

					if (data.exteriorAmbientLightLevel < data.torchLightLevel)
					{
						ImGui::SameLine();
						ImGui::TextUnformatted("[D]");
					}

					draw_row(UIGeneralInfoStrings::GameHour, "%.2f", data.gameHour);

					ImGui::EndTable();
				}

				ImGui::Spacing();
			}
		}

		template <class T>
		void UIGeneralInfo::draw_form_row(
			T            a_strid,
			Game::FormID a_formid,
			bool         a_testForm)
		{
			if (a_testForm && !a_formid)
			{
				return;
			}

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s:", UIL::LS(a_strid));

			ImGui::TableSetColumnIndex(1);
			DrawFormWithInfoWrapped(a_formid);
		}
	}
}