#include "pch.h"

#include "UIFormPickerWidget.h"

#include "IED/UI/UICommon.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		UIFormPickerWidget::UIFormPickerWidget(
			Controller&   a_controller,
			FormInfoFlags a_requiredFlags,
			bool          a_restrictTypes,
			bool          a_forceBase) :
			UIFormSelectorWidget(
				a_controller,
				a_requiredFlags,
				a_restrictTypes,
				true,
				a_forceBase),
			UIPopupInterface(a_controller),
			UIFormLookupInterface(a_controller),
			m_controller(a_controller)
		{
		}

		bool UIFormPickerWidget::DrawFormPicker(
			const char*               a_strid,
			Localization::StringID    a_label,
			Data::configCachedForm_t& a_form,
			const char*               a_tipText)
		{
			auto tmp = a_form.get_id();

			bool result = DrawFormPicker(a_strid, a_label, tmp, a_tipText);
			if (result)
			{
				a_form = tmp;
			}

			return result;
		}

		bool UIFormPickerWidget::DrawFormPicker(
			const char*            a_strid,
			Localization::StringID a_label,
			Game::FormID&          a_form,
			const char*            a_tipText)
		{
			ImGui::PushID("form_picker_widget");
			ImGui::PushID(a_strid);

			bool result  = false;
			bool hasForm = a_form != 0;

			m_currentInfo = nullptr;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 4.0f });

			result = DrawContextMenu(a_form);

			ImGui::PopStyleVar();

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 6.0f, 4.0f });

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginTable(
					"table",
					hasForm ? 2 : 1,
					ImGuiTableFlags_Borders |
						(hasForm ? ImGuiTableFlags_SizingFixedFit : ImGuiTableFlags_SizingStretchProp) |
						ImGuiTableFlags_NoSavedSettings,
					{ ImGui::CalcItemWidth(), 0 }))
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);

				char        buf[24];
				const char* label;

				if (hasForm)
				{
					stl::snprintf(buf, "%.8X##formsel", a_form.get());
					label = buf;
				}
				else
				{
					label = UIL::LS(CommonStrings::Select, "formsel");
				}

				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.5f });

				if (ImGui::Selectable(
						label,
						false,
						ImGuiSelectableFlags_DontClosePopups))
				{
					if (ImGui::GetIO().KeyShift || !IsFormBrowserEnabled())
					{
						ImGui::OpenPopup("form_sel_popup");
					}
					else
					{
						auto& formBrowser = m_controller.UIGetFormBrowser();

						if (formBrowser.Open(false))
						{
							if (m_onOpenFunc)
							{
								m_onOpenFunc(*this, formBrowser);
							}

							if (auto& allowed = GetAllowedTypes())
							{
								formBrowser.SetTabFilter(*allowed);
							}
							else
							{
								formBrowser.ClearTabFilter();
							}

							formBrowser.SetHighlightForm(a_form);
						}
					}
				}

				ImGui::PopStyleVar();

				if (ImGui::BeginPopup("form_sel_popup"))
				{
					result |= DrawFormSelector(a_form, a_tipText);

					if (result)
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				result |= DrawFormBrowserPopup(a_form).result;

				if (hasForm)
				{
					m_currentInfo = LookupForm(a_form);

					if (m_currentInfo)
					{
						ImGui::TableSetColumnIndex(1);

						auto desc = UIFormTypeSelectorWidget::form_type_to_desc(m_currentInfo->form.type);

						if (!m_currentInfo->form.name.empty())
						{
							ImGui::Text(
								"%s [%s]",
								m_currentInfo->form.name.c_str(),
								desc);
						}
						else if (desc)
						{
							ImGui::TextUnformatted(desc);
						}
						else
						{
							ImGui::Text("%hhu", m_currentInfo->form.type);
						}

						UICommon::ToolTip(100.0f, [&] {
							stl::fixed_string modName;

							std::uint32_t modIndex;

							if (a_form.GetPluginPartialIndex(modIndex))
							{
								auto& imap = Data::IData::GetPluginInfo().GetIndexMap();

								if (auto it = imap.find(modIndex); it != imap.end())
								{
									modName = it->second.name;
								}
							}

							if (!m_currentInfo->form.name.empty())
							{
								if (!modName.empty())
								{
									ImGui::Text(
										"[%.8X | %s] %s [%s]",
										a_form.get(),
										modName.c_str(),
										m_currentInfo->form.name.c_str(),
										desc);
								}
								else
								{
									ImGui::Text(
										"[%.8X] %s [%s]",
										a_form.get(),
										m_currentInfo->form.name.c_str(),
										desc);
								}
							}
							else
							{
								if (!modName.empty())
								{
									ImGui::Text(
										"[%.8X | %s] [%s]",
										a_form.get(),
										modName.c_str(),
										desc);
								}
								else
								{
									ImGui::Text(
										"%.8X [%s]",
										a_form.get(),
										desc);
								}
							}
						});
					}
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();

			ImGui::AlignTextToFramePadding();
			ImGui::SameLine();
			ImGui::TextUnformatted(UIL::LS(a_label));

			if (a_tipText)
			{
				UITipsInterface::DrawTip(a_tipText);
			}

			ImGui::PopID();
			ImGui::PopID();

			return result;
		}

		bool UIFormPickerWidget::DrawContextMenu(Game::FormID& a_form)
		{
			UIPopupToggleButtonWidget::DrawPopupToggleButton("context_button", "context_menu");

			bool result = false;

			if (ImGui::BeginPopup("context_menu"))
			{
				if (IsFormBrowserEnabled())
				{
					auto& formBrowser = m_controller.UIGetFormBrowser();

					ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

					if (UIL::LCG_MI(CommonStrings::Browse, "1"))
					{
						if (formBrowser.Open(false))
						{
							if (m_onOpenFunc)
							{
								m_onOpenFunc(*this, formBrowser);
							}

							if (auto& allowed = GetAllowedTypes())
							{
								formBrowser.SetTabFilter(*allowed);
							}
							else
							{
								formBrowser.ClearTabFilter();
							}

							formBrowser.SetHighlightForm(a_form);
						}
					}

					ImGui::PopItemFlag();

					auto fb_result = DrawFormBrowserPopup(a_form);

					if (fb_result)
					{
						result = true;
						ImGui::CloseCurrentPopup();
					}
					else if (fb_result.closed)
					{
						ImGui::CloseCurrentPopup();
					}
				}

				if (m_allowClear && a_form != 0)
				{
					if (UIL::LCG_MI(CommonStrings::Clear, "2"))
					{
						a_form = 0;
						result = true;
					}
				}

				ImGui::EndPopup();
			}

			return result;
		}

		UIFormBrowser::FormBrowserDrawResult UIFormPickerWidget::DrawFormBrowserPopup(Game::FormID& a_form)
		{
			auto& formBrowser = m_controller.UIGetFormBrowser();

			auto fb_result = formBrowser.DrawImpl();

			if (fb_result)
			{
				auto& entry = formBrowser.GetSelectedEntry();

				if (entry)
				{
					if (IsEntryValid(*entry))
					{
						a_form = entry->formid;
					}
					else
					{
						QueueNotification(
							"Error",
							"%.8X - form not valid for this input",
							entry->formid.get());
					}
				}
			}

			return fb_result;
		}
	}
}