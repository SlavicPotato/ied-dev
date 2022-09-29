#pragma once

#include "UIProfileBase.h"

#include "IED/UI/Controls/UIAlignment.h"
#include "IED/UI/Widgets/Filters/UIGenericFilter.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/UIContext.h"

namespace IED
{
	namespace UI
	{
		template <class T>
		class UIProfileEditorBase :
			public UIWindow,
			public UIContext,
			virtual protected UIAlignment,
			public UIProfileBase<T>
		{
		public:
			UIProfileEditorBase(
				UIProfileStrings             a_title,
				const char*                  a_strid,
				Localization::ILocalization& a_localization);

			virtual ~UIProfileEditorBase() noexcept = default;

			virtual void Draw() override;

			virtual void DrawProfileEditorMenuBarItems();

		protected:
			virtual WindowLayoutData GetWindowDimensions() const;

			virtual void DrawItem(T& a_profile) = 0;
			virtual void OnProfileReload(const T& a_profile);

		private:
			void SelectFirstPassed();

			virtual ProfileManager<T>& GetProfileManager() const = 0;

			virtual void DrawOptions(const T& a_profile);

			void DrawMenuBar();

			UIGenericFilter  m_filter;
			UIProfileStrings m_title;
			const char*      m_strid;
		};

		template <class T>
		UIProfileEditorBase<T>::UIProfileEditorBase(
			UIProfileStrings             a_title,
			const char*                  a_strid,
			Localization::ILocalization& a_localization) :
			UIProfileBase<T>(a_localization),
			m_title(a_title),
			m_strid(a_strid)
		{}

		template <class T>
		void UIProfileEditorBase<T>::SelectFirstPassed()
		{
			const auto& data = GetProfileManager().Data();

			for (const auto& e : data.getvec())
			{
				if (m_filter.Test(*e->first))
				{
					SetSelected(e->first);
					break;
				}
			}
		}

		template <class T>
		void UIProfileEditorBase<T>::Draw()
		{
			SetWindowDimensions(GetWindowDimensions());

			if (ImGui::Begin(
					LS<UIProfileStrings, 3>(
						m_title,
						m_strid),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				auto& data = GetProfileManager().Data();

				if (m_state.selected)
				{
					if (!data.contains(*m_state.selected))
					{
						m_state.selected.reset();
					}
				}

				if (!m_state.selected)
				{
					SelectFirstPassed();
				}
				else
				{
					if (!m_filter.Test(*(*m_state.selected)))
					{
						m_state.selected.reset();
						SelectFirstPassed();
					}
				}

				ImGui::PushItemWidth(ImGui::GetFontSize() * -9.0f);

				if (ImGui::BeginCombo(
						LS(CommonStrings::Profile, "combo"),
						m_state.selected ?
							m_state.selected->c_str() :
                            nullptr,
						ImGuiComboFlags_HeightLarge))
				{
					const ProfileManager<T>::storage_type::value_type* newItem = nullptr;

					for (const auto& e : data.getvec())
					{
						if (!m_filter.Test(*e->first))
						{
							continue;
						}

						ImGui::PushID(e);

						bool selected = m_state.selected == e->first;
						if (selected)
						{
							if (ImGui::IsWindowAppearing())
								ImGui::SetScrollHereY();
						}

						if (ImGui::Selectable(LMKID<3>(e->second.Name().c_str(), "1"), selected))
						{
							newItem = e;
						}

						ImGui::PopID();
					}

					if (newItem)
					{
						SetSelected(newItem->first);
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();
				m_filter.DrawButton();

				auto& sh = StringHolder::GetSingleton();

				ImGui::SameLine(
					ImGui::GetWindowContentRegionMax().x -
					GetNextTextOffset(sh.snew, true));

				if (ButtonRight(sh.snew, !AllowCreateNew()))
				{
					ImGui::OpenPopup(LS(UIProfileStrings::NewProfile, POPUP_NEW_ID));
					m_state.new_input[0] = 0;
				}

				m_filter.Draw();

				ImGui::PopItemWidth();

				DrawCreateNew();

				if (m_state.selected)
				{
					if (auto it = data.find(*m_state.selected); it != data.end())
					{
						ImGui::PushID("ctls");

						auto& profile = it->second;

						if (AllowSave())
						{
							if (ImGui::Button(LS(CommonStrings::Save, "1")))
							{
								auto& pm = GetProfileManager();

								if (!pm.SaveProfile(profile.Name()))
								{
									auto& queue = GetPopupQueue_ProfileBase();

									queue.push(
										UIPopupType::Message,
										LS(CommonStrings::Error),
										"%s [%s]\n\n%s",
										LS(UIProfileStrings::SaveError),
										profile.Name().c_str(),
										pm.GetLastException().what());
								}
							}

							ImGui::SameLine();
						}

						if (ImGui::Button(LS(CommonStrings::Delete, "2")))
						{
							auto& queue = GetPopupQueue_ProfileBase();

							queue.push(
									 UIPopupType::Confirm,
									 LS(UIProfileStrings::ConfirmDelete),
									 "%s [%s]",
									 LS(UIProfileStrings::DeletePrompt),
									 profile.Name().c_str())
								.call([this, item = *m_state.selected](const auto&) {
									auto& pm = GetProfileManager();

									if (!pm.DeleteProfile(item))
									{
										auto& queue = GetPopupQueue_ProfileBase();

										queue.push(
											UIPopupType::Message,
											LS(CommonStrings::Error),
											"%s:\n\n%s",
											LS(UIProfileStrings::DeleteError),
											pm.GetLastException().what());
									}
								});
						}

						ImGui::SameLine();
						if (ImGui::Button(LS(CommonStrings::Rename, "3")))
						{
							auto& queue = GetPopupQueue_ProfileBase();

							queue.push(
									 UIPopupType::Input,
									 LS(CommonStrings::Rename),
									 "%s:",
									 LS(UIProfileStrings::ProfileNamePrompt))
								.fmt_input("%s", profile.Name().c_str())
								.call([this, item = *m_state.selected](const auto& a_p) {
									std::string newName(a_p.GetInput());

									if (newName.empty())
									{
										return;
									}

									auto& pm = GetProfileManager();
									if (!pm.RenameProfile(item, newName))
									{
										auto& queue = GetPopupQueue_ProfileBase();

										queue.push(
											UIPopupType::Message,
											LS(CommonStrings::Error),
											"%s\n\n%s",
											LS(UIProfileStrings::RenameError),
											pm.GetLastException().what());
									}
								});
						}

						ImGui::SameLine();
						if (ImGui::Button(LS(CommonStrings::Reload, "4")))
						{
							if (!profile.Load())
							{
								auto& queue = GetPopupQueue_ProfileBase();

								queue.push(
									UIPopupType::Message,
									LS(CommonStrings::Error),
									"%s [%s]\n\n%s",
									LS(UIProfileStrings::ReloadError),
									profile.Name().c_str(),
									profile.GetLastException().what());
							}
							else
							{
								OnProfileReload(profile);
							}
						}

						ImGui::PopID();

						ImGui::PushID("pe_options");
						DrawOptions(profile);
						ImGui::PopID();

						ImGui::Separator();

						if (profile.HasParserErrors())
						{
							ImGui::Spacing();

							ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
							ImGui::TextWrapped("%s", LS(UIProfileStrings::ProfileParserErrorWarning));
							ImGui::PopStyleColor();

							ImGui::Separator();
						}

						ImGui::PushID("pe_item");
						DrawItem(profile);
						ImGui::PopID();
					}
				}

				ImGui::PopItemWidth();
			}

			ImGui::End();
		}

		template <class T>
		void UIProfileEditorBase<T>::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(LS(CommonStrings::File, "1")))
				{
					if (ImGui::MenuItem(LS(CommonStrings::Close, "1")))
					{
						SetOpenState(false);
					}

					ImGui::EndMenu();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				DrawProfileEditorMenuBarItems();

				ImGui::EndMenuBar();
			}
		}

		template <class T>
		void UIProfileEditorBase<T>::DrawProfileEditorMenuBarItems()
		{
		}

		template <class T>
		WindowLayoutData UIProfileEditorBase<T>::GetWindowDimensions() const
		{
			return { 50.0f, 550.0f, -1.0f, false };
		}

		template <class T>
		void UIProfileEditorBase<T>::DrawOptions(const T& a_profile)
		{}

		template <class T>
		void UIProfileEditorBase<T>::OnProfileReload(const T& a_profile)
		{}

	}
}
