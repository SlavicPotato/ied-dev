#include "pch.h"

#include "UIFormInfoTooltipWidget.h"

#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormInfoCache.h"
#include "IED/UI/UIWeaponTypeStrings.h"

#include "IED/UI/Widgets/UIWeatherClassSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/IForm.h"
#include "IED/Controller/ObjectManagerData.h"

namespace IED
{
	namespace UI
	{
		UIFormInfoTooltipWidget::UIFormInfoTooltipWidget(
			Controller& a_controller) :
			UIMiscTextInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIFormInfoTooltipWidget::DrawFormInfoTooltip(
			const FormInfoResult*  a_info,
			const ObjectEntryBase& a_entry)
		{
			bool          drawnSep1 = false;
			std::uint32_t numFF     = 0;

			const auto ds = [&]<class T = std::nullptr_t>(T a_func = T())
			{
				if (a_info && !drawnSep1)
				{
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					drawnSep1 = true;
				}

				if constexpr (std::invocable<T>)
				{
					if (numFF > 0)
					{
						ImGui::SameLine();
						ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
						ImGui::SameLine();
					}

					numFF++;

					a_func();
				}
				else
				{
					numFF = 0;
				}
			};

			ImGui::BeginTooltip();
			//ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

			if (a_info)
			{
				DrawFormInfoResult(*a_info);
			}

			if (a_entry.data.state->flags.test(ObjectEntryFlags::kScbLeft))
			{
				ds([] { ImGui::TextUnformatted(UIL::LS(UIWidgetCommonStrings::LeftScbAttached)); });
			}

			if (a_entry.data.state->anim.holder)
			{
				ds([] { ImGui::TextUnformatted(UIL::LS(CommonStrings::Animation)); });
			}

			if (a_entry.data.state->light)
			{
				ds([] { ImGui::TextUnformatted(UIL::LS(CommonStrings::Light)); });
			}

			if (a_entry.data.state->simComponent)
			{
				ds([] { ImGui::TextUnformatted(UIL::LS(CommonStrings::Physics)); });
			}

			if (const auto soundForm = a_entry.data.state->sound.form)
			{
				ds();

				ImGui::Text("%s:", UIL::LS(CommonStrings::Sound));

				auto& flc = m_controller.UIGetFormLookupCache();

				if (const auto formInfo = flc.LookupForm(soundForm->formID))
				{
					ImGui::Indent();
					DrawFormInfoResult(*formInfo);
					ImGui::Unindent();
				}
				else
				{
					ImGui::SameLine();
					ImGui::Text("%.8X", soundForm->formID.get());
				}
			}

			//ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		void UIFormInfoTooltipWidget::DrawObjectEntryHeaderInfo(
			const FormInfoResult*  a_info,
			const ObjectEntryBase& a_entry)
		{
			auto& state = a_entry.data.state;

			if (!state)
			{
				return;
			}

			if (!a_entry.IsNodeVisible())
			{
				ImGui::TextColored(
					UICommon::g_colorGreyed,
					"[%s]",
					UIL::LS(CommonStrings::Hidden));

				ImGui::SameLine();
			}

			if (a_info)
			{
				if (a_info->form.name.empty())
				{
					ImGui::TextColored(
						UICommon::g_colorLightBlue,
						"%.8X",
						a_info->form.id.get());
				}
				else
				{
					ImGui::TextColored(
						UICommon::g_colorLightBlue,
						"%s",
						a_info->form.name.c_str());
				}

				if (ImGui::IsItemHovered())
				{
					DrawFormInfoTooltip(a_info, a_entry);
				}
			}
			else
			{
				ImGui::TextColored(
					UICommon::g_colorLightBlue,
					"%.8X",
					state->form->formID.get());

				if (ImGui::IsItemHovered())
				{
					DrawFormInfoTooltip(nullptr, a_entry);
				}
			}

			if (state->modelForm && state->form != state->modelForm)
			{
				ImGui::SameLine(0.0f, 5.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine(0.0f, 8.0f);

				ImGui::Text("%s:", UIL::LS(CommonStrings::Model));
				ImGui::SameLine();

				auto& flc = m_controller.UIGetFormLookupCache();

				if (auto formInfo = flc.LookupForm(state->modelForm->formID))
				{
					if (!formInfo->form.name.empty())
					{
						ImGui::TextColored(
							UICommon::g_colorLightBlue,
							"%s",
							formInfo->form.name.c_str());
					}
					else
					{
						ImGui::TextColored(
							UICommon::g_colorLightBlue,
							"%.8X",
							state->modelForm->formID.get());
					}

					if (ImGui::IsItemHovered())
					{
						DrawFormInfoTooltip(formInfo, a_entry);
					}
				}
				else
				{
					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						UIL::LS(CommonStrings::Unknown));
				}
			}
		}

		void UIFormInfoTooltipWidget::DrawFormWithInfo(Game::FormID a_form)
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return;
			}

			if (a_form)
			{
				const auto result = TextCopyable("%.8X", a_form.get());

				if (result >= TextCopyableResult::kHovered)
				{
					DrawGeneralFormInfoTooltip(a_form);
				}
			}
			else
			{
				ImGui::TextUnformatted(UIL::LS(CommonStrings::None));
			}
		}

		void UIFormInfoTooltipWidget::DrawFormWithInfoWrapped(Game::FormID a_form)
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return;
			}

			if (a_form)
			{
				const auto result = TextWrappedCopyable("%.8X", a_form.get());

				if (result >= TextCopyableResult::kHovered)
				{
					DrawGeneralFormInfoTooltip(a_form);
				}
			}
			else
			{
				ImGui::TextWrapped("%s", UIL::LS(CommonStrings::None));
			}
		}

		void UIFormInfoTooltipWidget::DrawGeneralFormInfoTooltip(Game::FormID a_form)
		{
			auto& flc = m_controller.UIGetFormLookupCache();
			DrawGeneralFormInfoTooltip(flc.LookupForm(a_form));
		}

		void UIFormInfoTooltipWidget::DrawGeneralFormInfoTooltip(
			const FormInfoResult* a_info)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 8 });

			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

			if (a_info)
			{
				DrawFormInfoResult(*a_info);
			}
			else
			{
				ImGui::TextColored(
					UICommon::g_colorWarning,
					"%s",
					UIL::LS(CommonStrings::Unknown));
			}

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();

			ImGui::PopStyleVar();
		}

		void UIFormInfoTooltipWidget::DrawFormInfoResult(const FormInfoResult& a_info)
		{
			DrawFormInfo(a_info.form);

			if (auto& base = a_info.base)
			{
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("%s:", UIL::LS(CommonStrings::Base));
				ImGui::Spacing();

				ImGui::Indent();

				DrawFormInfo(*base);

				ImGui::Unindent();
			}
		}

		void UIFormInfoTooltipWidget::DrawFormInfo(const FormInfoData& a_info)
		{
			ImGui::Text("%s:", UIL::LS(CommonStrings::FormID));
			ImGui::SameLine();
			ImGui::Text("%.8X", a_info.id.get());

			std::uint32_t modIndex;
			if (a_info.id.GetPluginPartialIndex(modIndex))
			{
				auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

				auto itm = modList.find(modIndex);
				if (itm != modList.end())
				{
					ImGui::Text("%s:", UIL::LS(CommonStrings::Mod));
					ImGui::SameLine();
					ImGui::Text(
						"[%X] %s",
						itm->second.GetPartialIndex(),
						itm->second.name.c_str());
				}
			}

			ImGui::Text("%s:", UIL::LS(CommonStrings::Type));

			if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(a_info.type))
			{
				ImGui::SameLine();
				ImGui::Text("%s [%hhu]", typeDesc, a_info.type);
			}
			else
			{
				ImGui::SameLine();
				ImGui::Text("%hhu", a_info.type);
			}

			if (!a_info.name.empty())
			{
				ImGui::Text("%s:", UIL::LS(CommonStrings::Name));
				ImGui::SameLine();
				ImGui::Text("%s", a_info.name.c_str());
			}

			ImGui::Text("%s:", UIL::LS(CommonStrings::Flags));
			ImGui::SameLine();
			ImGui::Text("%.8X", a_info.formFlags);

			if (auto& extra = a_info.extraInfo)
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawExtraFormInfo(*extra);

				ImGui::Unindent();
			}
		}

		static Localization::StringID constexpr weapon_type_to_strid(WEAPON_TYPE a_type) noexcept
		{
			using namespace Localization;

			switch (a_type)
			{
			case WEAPON_TYPE::kHandToHandMelee:
				return static_cast<StringID>(UIWeaponTypeStrings::HandToHandMelee);
			case WEAPON_TYPE::kOneHandSword:
				return static_cast<StringID>(UIWeaponTypeStrings::OneHandSword);
			case WEAPON_TYPE::kOneHandDagger:
				return static_cast<StringID>(UIWeaponTypeStrings::OneHandDagger);
			case WEAPON_TYPE::kOneHandAxe:
				return static_cast<StringID>(UIWeaponTypeStrings::OneHandAxe);
			case WEAPON_TYPE::kOneHandMace:
				return static_cast<StringID>(UIWeaponTypeStrings::OneHandMace);
			case WEAPON_TYPE::kTwoHandSword:
				return static_cast<StringID>(UIWeaponTypeStrings::TwoHandSword);
			case WEAPON_TYPE::kTwoHandAxe:
				return static_cast<StringID>(UIWeaponTypeStrings::TwoHandAxe);
			case WEAPON_TYPE::kBow:
				return static_cast<StringID>(UIWeaponTypeStrings::Bow);
			case WEAPON_TYPE::kStaff:
				return static_cast<StringID>(UIWeaponTypeStrings::Staff);
			case WEAPON_TYPE::kCrossbow:
				return static_cast<StringID>(UIWeaponTypeStrings::Crossbow);
			default:
				return static_cast<StringID>(CommonStrings::Unknown);
			}
		}

		void UIFormInfoTooltipWidget::DrawExtraFormInfo(const BaseExtraFormInfo& a_info)
		{
			switch (a_info.type)
			{
			case ExtraFormInfoTESWeather::FORM_TYPE::kTypeID:
				{
					auto& v = static_cast<const ExtraFormInfoTESWeather&>(a_info);

					ImGui::Text(
						"%s: %s",
						UIL::LS(UIWidgetCommonStrings::WeatherClass),
						UIWeatherClassSelectorWidget{}.weather_class_to_desc(v.classFlags));
				}
				break;
			case ExtraFormInfoTESObjectWEAP::FORM_TYPE::kTypeID:
				{
					auto& v = static_cast<const ExtraFormInfoTESObjectWEAP&>(a_info);

					ImGui::Text(
						"%s: %hhu [%s]",
						UIL::LS(UIWidgetCommonStrings::WeaponType),
						v.weaponType,
						UIL::LS(weapon_type_to_strid(v.weaponType)));

					ImGui::Text(
						"%s: %s",
						UIL::LS(CommonStrings::Playable),
						UIL::LS(v.flags.test(TESObjectWEAP::Data::Flag::kNonPlayable) ? CommonStrings::No : CommonStrings::Yes));

					if (v.flags2.test(TESObjectWEAP::Data::Flag2::kBoundWeapon))
					{
						ImGui::TextUnformatted(UIL::LS(UIWidgetCommonStrings::BoundWeapon));
					}
				}
				break;
			case ExtraFormInfoTESAmmo::FORM_TYPE::kTypeID:
				{
					auto& v = static_cast<const ExtraFormInfoTESAmmo&>(a_info);

					ImGui::Text(
						"%s: %s",
						UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt),
						UIL::LS(v.flags.test(AMMO_DATA::Flag::kNonBolt) ? CommonStrings::No : CommonStrings::Yes));

					ImGui::Text(
						"%s: %s",
						UIL::LS(CommonStrings::Playable),
						UIL::LS(v.flags.test(AMMO_DATA::Flag::kNonPlayable) ? CommonStrings::No : CommonStrings::Yes));
				}
				break;
			case ExtraFormInfoTESObjectARMA::FORM_TYPE::kTypeID:
				{
					auto& v = static_cast<const ExtraFormInfoTESObjectARMA&>(a_info);

					ImGui::Text(
						"%s: %f",
						UIL::LS(UINodeOverrideEditorWidgetStrings::WeaponAdjust),
						v.weaponAdjust);
				}
				break;
			}
		}
	}
}