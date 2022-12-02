#pragma once

#include "UIProfileBase.h"

#include "IED/UI/Controls/UIAlignment.h"

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
			virtual protected UIAlignment,
			UIDataBase<T, typename P::base_type>,
			public UIProfileBase<P>
		{
		protected:
			inline static constexpr auto POPUP_ID_APPLY = "popup_apply";
			inline static constexpr auto POPUP_ID_MERGE = "popup_merge";
			inline static constexpr auto POPUP_ID_SAVE  = "popup_save";

			UIProfileSelectorBase(
				Localization::ILocalization& a_localization,
				UIProfileSelectorFlags       a_flags = UIProfileSelectorFlags::kEnableApply);

			virtual ~UIProfileSelectorBase() noexcept = default;

			void DrawProfileSelector(const T& a_data);

			virtual void ApplyProfile(const T& a_data, const P& a_profile){};
			virtual void MergeProfile(const T& a_data, const P& a_profile){};

			virtual void DrawProfileSelectorOptions(const T& a_data);

		private:
			virtual bool DrawProfileImportOptions(const T& a_data, const P& a_profile, bool a_isMerge) { return true; };
			virtual void ResetProfileImportOptions() {}

			stl::flag<UIProfileSelectorFlags> m_flags;
		};

		template <class T, class P>
		UIProfileSelectorBase<T, P>::UIProfileSelectorBase(
			Localization::ILocalization& a_localization,
			UIProfileSelectorFlags       a_flags) :
			UIProfileBase<P>(a_localization),
			m_flags(a_flags)
		{
		}

		template <class T, class P>
		void UIProfileSelectorBase<T, P>::DrawProfileSelector(
			const T& a_data)
		{
			auto& pm   = GetProfileManager();
			auto& data = pm.Data();

			ImGui::PushID("profile_selector_base");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -18.5f);

			const char* preview = nullptr;
			if (m_state.selected)
			{
				if (data.find(*m_state.selected) != data.end())
				{
					preview = m_state.selected->c_str();
				}
				else
				{
					m_state.selected.clear();
				}
			}

			if (ImGui::BeginCombo(
					LS(CommonStrings::Profile, "sel_combo"),
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				for (const auto& e : data.getvec())
				{
					ImGui::PushID(e);

					bool selected = m_state.selected == e->first;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(LMKID<3>(e->second.Name().c_str(), "1"), selected))
					{
						m_state.selected = e->first;
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
				if (ButtonRight(sh.snew, !AllowCreateNew()))
				{
					ImGui::OpenPopup(LS(UIProfileStrings::NewProfile, POPUP_NEW_ID));
					m_state.new_input[0] = 0;
				}
			}

			{
				const auto tmpd = std::make_unique<typename P::base_type>(GetData(a_data));
				DrawCreateNew(tmpd.get());
			}

			if (m_state.selected)
			{
				auto it = data.find(*m_state.selected);
				if (it != data.end())
				{
					auto& profile = it->second;

					if (!m_flags.test(UIProfileSelectorFlags::kDisableControls))
					{
						if (m_flags.test(UIProfileSelectorFlags::kEnableApply))
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.apply));
							if (ButtonRight(sh.apply))
							{
								ResetProfileImportOptions();
								ImGui::OpenPopup(LS(CommonStrings::Confirm, POPUP_ID_APPLY));
							}
						}

						if (m_flags.test(UIProfileSelectorFlags::kEnableMerge))
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.merge));
							if (ButtonRight(sh.merge))
							{
								ResetProfileImportOptions();
								ImGui::OpenPopup(LS(CommonStrings::Confirm, POPUP_ID_MERGE));
							}
						}

						if (ConfirmDialog(
								LS(CommonStrings::Confirm, POPUP_ID_APPLY),
								[&] {
									return DrawProfileImportOptions(a_data, profile, false);
								},
								{},
								"%s [%s]\n\n%s",
								LS(UIProfileStrings::LoadDataFromProfile),
								profile.Name().c_str(),
								LS(UIWidgetCommonStrings::CurrentValuesLost)) == ModalStatus::kAccept)
						{
							ApplyProfile(a_data, profile);
						}

						if (ConfirmDialog(
								LS(CommonStrings::Confirm, POPUP_ID_MERGE),
								[&] {
									return DrawProfileImportOptions(a_data, profile, true);
								},
								{},
								"%s [%s]\n\n%s",
								LS(UIProfileStrings::MergeDataFromProfile),
								profile.Name().c_str(),
								LS(UIWidgetCommonStrings::CurrentValuesOverwritten)) == ModalStatus::kAccept)
						{
							MergeProfile(a_data, profile);
						}

						if (AllowSave())
						{
							ImGui::SameLine(wcm.x - GetNextTextOffset(sh.save));
							if (ButtonRight(sh.save))
							{
								ImGui::OpenPopup(LS(CommonStrings::Confirm, POPUP_ID_SAVE));
							}
						}

						if (ConfirmDialog(
								LS(CommonStrings::Confirm, POPUP_ID_SAVE),
								{},
								{},
								"%s [%s]",
								LS(UIProfileStrings::SaveCurrentToProfile),
								profile.Name().c_str()) == ModalStatus::kAccept)
						{
							if (!pm.SaveProfile(profile.Name(), GetData(a_data)))
							{
								m_state.lastException = pm.GetLastException();

								GetPopupQueue_ProfileBase().push(
									UIPopupType::Message,
									LS(CommonStrings::Error),
									"%s [%s]\n\n%s",
									LS(UIProfileStrings::SaveError),
									profile.Name().c_str(),
									pm.GetLastException().what());
							}
						}
					}

					if (profile.HasParserErrors())
					{
						ImGui::Spacing();

						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped("%s", LS(UIProfileStrings::ProfileParserErrorWarning));
						ImGui::PopStyleColor();
					}

					ImGui::PushID("ps_options");
					DrawProfileSelectorOptions(a_data);
					ImGui::PopID();
				}
			}

			ImGui::PopItemWidth();

			ImGui::PopID();
		}

		template <class T, class P>
		void UIProfileSelectorBase<T, P>::DrawProfileSelectorOptions(const T& a_data)
		{
		}

	}
}