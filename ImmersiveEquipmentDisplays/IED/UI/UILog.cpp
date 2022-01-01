#include "pch.h"

#include "UICommon.h"
#include "UILog.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UILog::UILog(Controller& a_controller) :
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_filter(true)
		{
		}

		void UILog::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			SetWindowDimensions(0.0f, 800.0f, 800.0f, true);

			if (ImGui::Begin(
					LS<CommonStrings, 3>(
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
			auto& settings = m_controller.GetConfigStore().settings;

			auto& data = ISKSE::GetBacklog();

			ImGui::PushID("header");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -25.0f);

			m_filter.Draw();

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * -10.0f);

			constexpr int step = 1;
			constexpr int stepFast = 100;
			std::uint32_t limit = data.GetLimit();

			if (ImGui::InputScalar(
					LS(CommonStrings::Limit, "ctl_1"),
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

			DrawLevelCheckbox(LS(CommonStrings::Fatal, "ctl_2"), LogLevel::FatalError);
			ImGui::SameLine();
			DrawLevelCheckbox(LS(CommonStrings::Error, "ctl_3"), LogLevel::Error);
			ImGui::SameLine();
			DrawLevelCheckbox(LS(CommonStrings::Warning, "ctl_4"), LogLevel::Warning);
			ImGui::SameLine();
			DrawLevelCheckbox(LS(CommonStrings::Message, "ctl_5"), LogLevel::Message);
			ImGui::SameLine();
			DrawLevelCheckbox(LS(CommonStrings::Debug, "ctl_6"), LogLevel::Debug);

			ImGui::PopID();
		}

		void UILog::DrawLogText()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			auto& data = ISKSE::GetBacklog();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			if (ImGui::BeginChild("log_text", { -1.0f, 0.0f }))
			{
				{
					IScopedLock lock(data.GetLock());

					if (auto curSize = data.Size(); curSize != m_lastSize)
					{
						m_doScrollBottom = true;
						m_lastSize = curSize;
					}

					for (auto& e : data)
					{
						auto level = e.level();

						if (!settings.data.ui.logLevels[stl::underlying(level)])
						{
							continue;
						}

						if (!m_filter.Test(e))
						{
							continue;
						}

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

		void UILog::DrawLevelCheckbox(const char* a_label, LogLevel a_level)
		{
			auto& settings = m_controller.GetConfigStore().settings;

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