#include "pch.h"

#include "UICommon.h"
#include "UILog.h"

#include "IED/Controller/Controller.h"

#include "UILogStrings.h"

namespace IED
{
	namespace UI
	{
		UILog::UILog(Controller& a_controller) :
			m_controller(a_controller),
			m_filter(true)
		{
		}

		void UILog::Draw()
		{
			SetWindowDimensions(0.0f, 800.0f, 800.0f, true);

			if (ImGui::Begin(
					UIL::LS<CommonStrings, 3>(
						CommonStrings::Log,
						WINDOW_ID),
					GetOpenState()))
			{
				DrawHeader();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				DrawLogText();
			}

			ImGui::End();
		}

		void UILog::Initialize()
		{
		}

		void UILog::DrawHeader()
		{
			auto& settings = m_controller.GetSettings();

			auto& data = ISKSE::GetBacklog();

			ImGui::PushID("header");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -25.0f);

			m_filter.Draw();

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * -10.0f);

			ImGui::PushID("ctls");

			const int step     = 1;
			const int stepFast = 100;

			auto limit = static_cast<std::uint32_t>(data.GetLimit());

			if (ImGui::InputScalar(
					UIL::LS(CommonStrings::Limit, "1"),
					ImGuiDataType_U32,
					std::addressof(limit),
					std::addressof(step),
					std::addressof(stepFast),
					"%u",
					ImGuiInputTextFlags_EnterReturnsTrue))
			{
				limit = std::clamp<std::uint32_t>(limit, 1, 2000);

				data.SetLimit(limit);
				settings.data.ui.logLimit = limit;
				settings.mark_dirty();
			}

			ImGui::PopItemWidth();
			ImGui::PopItemWidth();

			ImGui::Spacing();

			DrawLevelCheckbox(UIL::LS(CommonStrings::Fatal, "2"), LogLevel::FatalError);
			ImGui::SameLine();
			DrawLevelCheckbox(UIL::LS(CommonStrings::Error, "3"), LogLevel::Error);
			ImGui::SameLine();
			DrawLevelCheckbox(UIL::LS(CommonStrings::Warning, "4"), LogLevel::Warning);
			ImGui::SameLine();
			DrawLevelCheckbox(UIL::LS(CommonStrings::Message, "5"), LogLevel::Message);
			ImGui::SameLine();
			DrawLevelCheckbox(UIL::LS(CommonStrings::Debug, "6"), LogLevel::Debug);
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

#if defined(SKMP_TIMESTAMP_LOGS)

			settings.mark_if(ImGui::Checkbox(
				UILI::LS(UILogStrings::ShowTimestamps, "7"),
				std::addressof(settings.data.ui.logShowTimestamps)));
#endif

			ImGui::PopID();

			ImGui::PopID();
		}

		void UILog::DrawLogText()
		{
			auto& settings = m_controller.GetSettings();

			const auto& data = ISKSE::GetBacklog();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			if (ImGui::BeginChild("log_text", { -1.0f, 0.0f }))
			{
				{
					const stl::lock_guard lock(data.GetLock());

					if (auto curSize = data.Size(); curSize != m_lastSize)
					{
						m_doScrollBottom = true;
						m_lastSize       = curSize;
					}

					for (auto& e : data)
					{
						auto level = e.level();

						if (!settings.data.ui.logLevels[stl::underlying(level)])
						{
							continue;
						}

						if (!m_filter.Test(e.string()))
						{
							continue;
						}

#if defined(SKMP_TIMESTAMP_LOGS)
						if (settings.data.ui.logShowTimestamps)
						{
							DrawTimeStampLine(e);
						}
#endif

						bool popcol = true;

						switch (level)
						{
						case LogLevel::FatalError:
							ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorFatalError);
							break;
						case LogLevel::Error:
							ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorError);
							break;
						case LogLevel::Warning:
							ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
							break;
						case LogLevel::Debug:
							ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
							break;
						default:
							popcol = false;
							break;
						}

						ImGui::TextWrapped("%s", e.data());

						if (popcol)
						{
							ImGui::PopStyleColor();
						}
					}
				}

				AutoScroll();
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

#if defined(SKMP_TIMESTAMP_LOGS)

		void UILog::DrawTimeStampLine(const BackLog::Entry& a_entry)
		{
			try
			{
				auto lt = std::chrono::floor<std::chrono::days>(a_entry.ts());

				std::chrono::year_month_day ymd{ lt };
				std::chrono::hh_mm_ss       hms{ a_entry.ts() - lt };

				ImGui::Text(
					"[%d-%.2u-%.2u %.2d:%.2d:%.2lld.%lld]",
					static_cast<int>(ymd.year()),
					static_cast<std::uint32_t>(ymd.month()),
					static_cast<std::uint32_t>(ymd.day()),
					static_cast<int>(hms.hours().count()),
					static_cast<int>(hms.minutes().count()),
					static_cast<long long>(hms.seconds().count()),
					static_cast<long long>(hms.subseconds().count()));

				ImGui::SameLine();
			}
			catch (...)
			{
			}
		}

#endif

		void UILog::DrawLevelCheckbox(const char* a_label, LogLevel a_level)
		{
			auto& settings = m_controller.GetSettings();

			settings.mark_if(ImGui::Checkbox(
				a_label,
				std::addressof(settings.data.ui.logLevels[stl::underlying(a_level)])));
		}

		void UILog::AutoScroll()
		{
			if (m_initialScroll > 0)
			{
				m_initialScroll--;

				if (!m_initialScroll)
				{
					ImGui::SetScrollHereY(0.0f);
				}
			}
			else
			{
				if (m_doScrollBottom)
				{
					if (ImGui::GetScrollY() > ImGui::GetScrollMaxY() - 25.0f)
					{
						ImGui::SetScrollHereY(0.0f);
					}

					m_doScrollBottom = false;
				}
			}

			ImGui::Dummy({ 0.0f, 0.0f });
		}

	}
}