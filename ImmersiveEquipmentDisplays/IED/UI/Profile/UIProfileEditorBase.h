#pragma once

#include "UIProfileBase.h"

#include "IED/UI/Controls/UIAlignment.h"
#include "IED/UI/Widgets/Filters/UIGenericFilter.h"

#include "UIProfileEditorBaseStrings.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/Window/UIWindow.h"

namespace IED
{
	namespace UI
	{
		enum class UIProfileEditorBaseFlags : std::uint8_t
		{
			kNone = 0,

			kDisableSaveUnmodified = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(UIProfileEditorBaseFlags);

		namespace concepts
		{
			template <class T>
			concept accept_cached_item_string = std::is_convertible_v<T, stl::fixed_string>;
		}

		template <class T>
		class UIProfileEditorBase :
			public UIContext,
			public UIWindow,
			public UIProfileBase<T>,
			virtual protected UIAlignment
		{
		protected:
			template <class Tc>
			struct cachedItem_t
			{
				cachedItem_t() = delete;

				template <concepts::accept_cached_item_string Ts, class... Args>
				constexpr cachedItem_t(
					Ts&& a_name,
					Args&&... a_args) :
					name(std::forward<Ts>(a_name)),
					data(std::forward<Args>(a_args)...)
				{
				}

				stl::fixed_string name;
				Tc                data;
			};

		public:
			UIProfileEditorBase(
				UIProfileStrings         a_title,
				const char*              a_strid,
				UIProfileEditorBaseFlags a_flags = UIProfileEditorBaseFlags::kNone);

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

			UIGenericFilter                     m_filter;
			UIProfileStrings                    m_title;
			const char*                         m_strid;
			stl::flag<UIProfileEditorBaseFlags> m_flags;
		};

		template <class T>
		UIProfileEditorBase<T>::UIProfileEditorBase(
			UIProfileStrings         a_title,
			const char*              a_strid,
			UIProfileEditorBaseFlags a_flags) :
			UIProfileBase<T>(),
			m_title(a_title),
			m_strid(a_strid),
			m_flags(a_flags)
		{}

		template <class T>
		void UIProfileEditorBase<T>::SelectFirstPassed()
		{
			const auto& data = GetProfileManager().Data();

			for (const auto& e : data.getvec())
			{
				if (m_filter.Test(*e->first))
				{
					this->SetSelected(e->first);
					break;
				}
			}
		}

		template <class T>
		void UIProfileEditorBase<T>::Draw()
		{
			SetWindowDimensions(GetWindowDimensions());

			if (ImGui::Begin(
					UIL::LS<UIProfileStrings, 3>(
						m_title,
						m_strid),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				auto& data = GetProfileManager().Data();

				if (this->m_state.selected)
				{
					if (!data.contains(*this->m_state.selected))
					{
						this->m_state.selected.reset();
					}
				}

				if (!this->m_state.selected)
				{
					SelectFirstPassed();
				}
				else
				{
					if (!m_filter.Test(*(*this->m_state.selected)))
					{
						this->m_state.selected.reset();
						SelectFirstPassed();
					}
				}

				ImGui::PushItemWidth(ImGui::GetFontSize() * -9.0f);

				if (ImGui::BeginCombo(
						UIL::LS(CommonStrings::Profile, "combo"),
						this->m_state.selected ?
							this->m_state.selected->c_str() :
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

						bool selected = this->m_state.selected == e->first;
						if (selected)
						{
							if (ImGui::IsWindowAppearing())
								ImGui::SetScrollHereY();
						}

						if (ImGui::Selectable(UIL::LMKID<3>(e->second.Name().c_str(), "1"), selected))
						{
							newItem = e;
						}

						ImGui::PopID();
					}

					if (newItem)
					{
						this->SetSelected(newItem->first);
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();
				m_filter.DrawButton();

				auto& sh = StringHolder::GetSingleton();

				ImGui::SameLine(
					ImGui::GetWindowContentRegionMax().x -
					GetNextTextOffset(sh.snew, true));

				if (ButtonRight(sh.snew, !this->AllowCreateNew()))
				{
					ImGui::OpenPopup(UIL::LS(UIProfileStrings::NewProfile, UIProfileBase<T>::POPUP_NEW_ID));
					this->m_state.new_input.clear();
				}

				m_filter.Draw();

				ImGui::PopItemWidth();

				this->DrawCreateNew();

				if (this->m_state.selected)
				{
					if (auto it = data.find(*this->m_state.selected); it != data.end())
					{
						ImGui::PushID("ctls");

						auto& profile = it->second;

						if (this->AllowSave())
						{
							const bool disabled = m_flags.test(UIProfileEditorBaseFlags::kDisableSaveUnmodified) && !profile.IsModified();

							UICommon::PushDisabled(disabled);

							if (ImGui::Button(UIL::LS(CommonStrings::Save, "1")))
							{
								auto& pm = GetProfileManager();

								if (!pm.SaveProfile(profile.Name()))
								{
									auto& queue = this->GetPopupQueue_ProfileBase();

									queue.push(
										UIPopupType::Message,
										UIL::LS(CommonStrings::Error),
										"%s [%s]\n\n%s",
										UIL::LS(UIProfileStrings::SaveError),
										profile.Name().c_str(),
										pm.GetLastException().what());
								}
							}

							UICommon::PopDisabled(disabled);

							ImGui::SameLine();
						}

						if (ImGui::Button(UIL::LS(CommonStrings::Delete, "2")))
						{
							auto& queue = this->GetPopupQueue_ProfileBase();

							queue.push(
									 UIPopupType::Confirm,
									 UIL::LS(UIProfileStrings::ConfirmDelete),
									 "%s [%s]",
									 UIL::LS(UIProfileStrings::DeletePrompt),
									 profile.Name().c_str())
								.call([this, item = *this->m_state.selected](const auto&) {
									auto& pm = GetProfileManager();

									if (!pm.DeleteProfile(item))
									{
										auto& queue = this->GetPopupQueue_ProfileBase();

										queue.push(
											UIPopupType::Message,
											UIL::LS(CommonStrings::Error),
											"%s:\n\n%s",
											UIL::LS(UIProfileStrings::DeleteError),
											pm.GetLastException().what());
									}
								});
						}

						ImGui::SameLine();
						if (ImGui::Button(UIL::LS(CommonStrings::Rename, "3")))
						{
							auto& queue = this->GetPopupQueue_ProfileBase();

							queue.push(
									 UIPopupType::Input,
									 UIL::LS(CommonStrings::Rename),
									 "%s:",
									 UIL::LS(UIProfileStrings::ProfileNamePrompt))
								.set_input(*profile.Name())
								.call([this, item = *this->m_state.selected](const auto& a_p) {
									auto& newName = a_p.GetInput();

									if (newName.empty() || 
										stl::fixed_string::key_type::test_equal(newName, *item))
									{
										return;
									}

									auto& pm = GetProfileManager();
									if (!pm.RenameProfile(item, newName))
									{
										auto& queue = this->GetPopupQueue_ProfileBase();

										queue.push(
											UIPopupType::Message,
											UIL::LS(CommonStrings::Error),
											"%s\n\n%s",
											UIL::LS(UIProfileStrings::RenameError),
											pm.GetLastException().what());
									}
								});
						}

						ImGui::SameLine();
						if (ImGui::Button(UIL::LS(CommonStrings::Reload, "4")))
						{
							if (!profile.Load())
							{
								auto& queue = this->GetPopupQueue_ProfileBase();

								queue.push(
									UIPopupType::Message,
									UIL::LS(CommonStrings::Error),
									"%s [%s]\n\n%s",
									UIL::LS(UIProfileStrings::ReloadError),
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
							ImGui::TextWrapped("%s", UIL::LS(UIProfileStrings::ProfileParserErrorWarning));
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
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::File, "1")))
				{
					if (ImGui::MenuItem(UIL::LS(CommonStrings::Close, "1")))
					{
						SetOpenState(false);
					}

					ImGui::EndMenu();
				}

				if (const auto& selected = this->m_state.selected)
				{
					auto& data = GetProfileManager().Data();

					if (auto it = data.find(*selected); it != data.end())
					{
						if (ImGui::BeginMenu(UIL::LS(CommonStrings::Settings, "2")))
						{
							auto& flags = it->second.GetFlags();

							if (ImGui::MenuItem(
									UIL::LS(UIProfileEditorBaseStrings::MergeOnly, "1"),
									nullptr,
									flags.test(ProfileFlags::kMergeOnly)))
							{
								flags.toggle(ProfileFlags::kMergeOnly);
								it->second.MarkModified();
							}

							ImGui::Separator();

							if (ImGui::MenuItem(UIL::LS(CommonStrings::Description, "2")))
							{
								auto& queue = this->GetPopupQueue_ProfileBase();

								auto& e =
									queue.push(
											 UIPopupType::MultilineInput,
											 UIL::LS(CommonStrings::Description))
										.set_allow_empty(true)
										.call([this, item = *this->m_state.selected](auto& a_p) {
											auto& data = GetProfileManager().Data();
											if (auto it = data.find(item); it != data.end())
											{
												auto& text = a_p.GetInput();
												if (!text.empty())
												{
													it->second.SetDescription(std::move(a_p.GetInput()));
												}
												else
												{
													it->second.ClearDescription();
												}
											}
										});

								if (auto& desc = it->second.GetDescription())
								{
									e.set_input(*desc);
								}
							}

							ImGui::EndMenu();
						}
					}
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
