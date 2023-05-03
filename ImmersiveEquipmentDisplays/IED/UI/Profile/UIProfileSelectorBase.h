#pragma once

#include "UIProfileBase.h"

#include "IED/UI/Controls/UIAlignment.h"

#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		enum class UIProfileSelectorFlags : std::uint32_t
		{
			kNone = 0u,

			kEnableApply = 1u << 0,
			kEnableMerge = 1u << 1,

			kDisableControls = 1u << 2
		};

		DEFINE_ENUM_CLASS_BITWISE(UIProfileSelectorFlags);

		template <class T, class C>
		class UIDataBase
		{
		protected:
			[[nodiscard]] virtual C GetData(const T& a_data) = 0;
		};

		template <class T, class P>
		class UIProfileSelectorBase :
			public UIProfileBase<P>,
			UIDataBase<T, typename P::base_type>,
			virtual protected UIAlignment
		{
		protected:
			static constexpr auto POPUP_ID_APPLY = "popup_apply";
			static constexpr auto POPUP_ID_MERGE = "popup_merge";
			static constexpr auto POPUP_ID_SAVE  = "popup_save";

			UIProfileSelectorBase(
				UIProfileSelectorFlags a_flags = UIProfileSelectorFlags::kEnableApply);

			virtual ~UIProfileSelectorBase() noexcept = default;

			void DrawProfileSelector(const T& a_data);

			virtual void ApplyProfile(const T& a_data, const P& a_profile){};
			virtual void MergeProfile(const T& a_data, const P& a_profile){};

			virtual void DrawProfileSelectorOptions(const T& a_data);

		private:
			virtual bool DrawProfileImportOptions(const T& a_data, const P& a_profile, bool a_isMerge) { return true; };
			virtual void ResetProfileImportOptions() {}

			void DrawDescription(const P& a_profile);

			stl::flag<UIProfileSelectorFlags> m_flags;
		};

		template <class T, class P>
		UIProfileSelectorBase<T, P>::UIProfileSelectorBase(
			UIProfileSelectorFlags a_flags) :
			UIProfileBase<P>(),
			m_flags(a_flags)
		{
		}

		template <class T, class P>
		void UIProfileSelectorBase<T, P>::DrawProfileSelector(
			const T& a_data)
		{
			auto& pm   = this->GetProfileManager();
			auto& data = pm.Data();

			ImGui::PushID("profile_selector_base");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -18.5f);

			const char* preview = nullptr;
			if (this->m_state.selected)
			{
				if (data.find(*this->m_state.selected) != data.end())
				{
					preview = this->m_state.selected->c_str();
				}
				else
				{
					this->m_state.selected.reset();
				}
			}

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Profile, "sel_combo"),
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				for (const auto& e : data.getvec())
				{
					ImGui::PushID(e);

					const bool selected = this->m_state.selected == e->first;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(UIL::LMKID<3>(e->second.Name().c_str(), "1"), selected))
					{
						this->m_state.selected = e->first;
					}

					ImGui::PopID();
				}
				ImGui::EndCombo();
			}

			auto wcm = ImGui::GetWindowContentRegionMax();

			auto& sh = StringHolder::GetSingleton();

			if (!m_flags.test(UIProfileSelectorFlags::kDisableControls))
			{
				ImGui::SameLine(wcm.x - GetNextTextOffset(sh.snew, true));
				if (ButtonRight(sh.snew, !this->AllowCreateNew()))
				{
					ImGui::OpenPopup(UIL::LS(UIProfileStrings::NewProfile, UIProfileBase<P>::POPUP_NEW_ID));
					this->m_state.new_input.clear();
				}
			}

			this->DrawCreateNew(
				std::make_unique<typename P::base_type>(this->GetData(a_data)));

			if (this->m_state.selected)
			{
				auto it = data.find(*this->m_state.selected);
				if (it != data.end())
				{
					auto& profile = it->second;

					if (!m_flags.test(UIProfileSelectorFlags::kDisableControls))
					{
						if (m_flags.test(UIProfileSelectorFlags::kEnableApply) &&
						    !profile.GetFlags().test(ProfileFlags::kMergeOnly))
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.apply));
							if (ButtonRight(sh.apply))
							{
								ResetProfileImportOptions();
								ImGui::OpenPopup(UIL::LS(CommonStrings::Confirm, POPUP_ID_APPLY));
							}
						}

						if (m_flags.test(UIProfileSelectorFlags::kEnableMerge))
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.merge));
							if (ButtonRight(sh.merge))
							{
								ResetProfileImportOptions();
								ImGui::OpenPopup(UIL::LS(CommonStrings::Confirm, POPUP_ID_MERGE));
							}
						}

						if (this->ConfirmDialog(
								UIL::LS(CommonStrings::Confirm, POPUP_ID_APPLY),
								[&] {
									return DrawProfileImportOptions(a_data, profile, false);
								},
								{},
								"%s [%s]\n\n%s",
								UIL::LS(UIProfileStrings::LoadDataFromProfile),
								profile.Name().c_str(),
								UIL::LS(UIWidgetCommonStrings::CurrentValuesLost)) == UICommonModals::ModalStatus::kAccept)
						{
							ApplyProfile(a_data, profile);
						}

						if (this->ConfirmDialog(
								UIL::LS(CommonStrings::Confirm, POPUP_ID_MERGE),
								[&] {
									return DrawProfileImportOptions(a_data, profile, true);
								},
								{},
								"%s [%s]\n\n%s",
								UIL::LS(UIProfileStrings::MergeDataFromProfile),
								profile.Name().c_str(),
								UIL::LS(UIWidgetCommonStrings::CurrentValuesOverwritten)) == UICommonModals::ModalStatus::kAccept)
						{
							MergeProfile(a_data, profile);
						}

						if (this->AllowSave())
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.save));
							if (ButtonRight(sh.save))
							{
								ImGui::OpenPopup(UIL::LS(CommonStrings::Confirm, POPUP_ID_SAVE));
							}
						}

						if (this->ConfirmDialog(
								UIL::LS(CommonStrings::Confirm, POPUP_ID_SAVE),
								{},
								{},
								"%s [%s]",
								UIL::LS(UIProfileStrings::SaveCurrentToProfile),
								profile.Name().c_str()) == UICommonModals::ModalStatus::kAccept)
						{
							if (!pm.SaveProfile(profile.Name(), this->GetData(a_data)))
							{
								this->m_state.lastException = pm.GetLastException();

								this->GetPopupQueue_ProfileBase().push(
									UIPopupType::Message,
									UIL::LS(CommonStrings::Error),
									"%s [%s]\n\n%s",
									UIL::LS(UIProfileStrings::SaveError),
									profile.Name().c_str(),
									pm.GetLastException().what());
							}
						}
					}

					if (profile.HasParserErrors())
					{
						ImGui::Spacing();

						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped("%s", UIL::LS(UIProfileStrings::ProfileParserErrorWarning));
						ImGui::PopStyleColor();
					}

					ImGui::PushID("ps_options");
					DrawProfileSelectorOptions(a_data);
					ImGui::PopID();

					DrawDescription(profile);
				}
			}

			ImGui::PopItemWidth();

			ImGui::PopID();
		}

		template <class T, class P>
		void UIProfileSelectorBase<T, P>::DrawProfileSelectorOptions(const T& a_data)
		{
		}

		template <class T, class P>
		void UIProfileSelectorBase<T, P>::DrawDescription(const P& a_profile)
		{
			auto& desc = a_profile.GetDescription();
			if (!desc || desc->empty())
			{
				return;
			}

			auto window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return;
			}

			ImGui::Spacing();

			const float wrapWidth = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, 0.0f);

			const auto size = ImGui::CalcTextSize(desc->c_str(), nullptr, false, wrapWidth);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

			ImGui::SetNextWindowSizeConstraints({ 0, 0 }, { -1, 200 });

			if (ImGui::BeginChild(
					"ps_desc",
					{ 0.0f, size.y },
					false,
					ImGuiWindowFlags_None))
			{
				ImGui::TextWrapped("%s", desc->c_str());
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();
		}

	}
}