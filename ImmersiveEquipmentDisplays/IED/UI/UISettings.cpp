#include "pch.h"

#include "UIMain.h"
#include "UISettings.h"
#include "UISettingsStrings.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/ObjectDatabaseLevel.h"
#include "IED/StringHolder.h"

#include "Widgets/UIWidgetCommonStrings.h"

#include "Drivers/UI.h"

namespace IED
{
	namespace UI
	{
		static const std::array<std::pair<ObjectDatabaseLevel, CommonStrings>, 10> s_odbmvals{ {

			{ ObjectDatabaseLevel::kDisabled, CommonStrings::Disabled },
			{ ObjectDatabaseLevel::kNone, CommonStrings::UsedOnly },
			{ ObjectDatabaseLevel::kMin, CommonStrings::Minimum },
			{ ObjectDatabaseLevel::kVeryLow, CommonStrings::VeryLow },
			{ ObjectDatabaseLevel::kLow, CommonStrings::Low },
			{ ObjectDatabaseLevel::kMedium, CommonStrings::Medium },
			{ ObjectDatabaseLevel::kHigh, CommonStrings::High },
			{ ObjectDatabaseLevel::kVeryHigh, CommonStrings::VeryHigh },
			{ ObjectDatabaseLevel::kExtreme, CommonStrings::Extreme },
			{ ObjectDatabaseLevel::kMax, CommonStrings::Maximum }

		} };

		static const std::array<std::pair<stl::flag<GlyphPresetFlags>, const char*>, 12> s_extraGlyphs{ {

			{ GlyphPresetFlags::kLatinFull, "Latin full" },
			{ GlyphPresetFlags::kCyrilic, "Cyrilic" },
			{ GlyphPresetFlags::kJapanese, "Japanese" },
			{ GlyphPresetFlags::kChineseSimplifiedCommon, "Chinese simplified common" },
			{ GlyphPresetFlags::kChineseFull, "Chinese full" },
			{ GlyphPresetFlags::kKorean, "Korean" },
			{ GlyphPresetFlags::kThai, "Thai" },
			{ GlyphPresetFlags::kVietnamise, "Vietnamise" },
			{ GlyphPresetFlags::kGreek, "Greek" },
			{ GlyphPresetFlags::kArabic, "Arabic" },
			{ GlyphPresetFlags::kArrows, "Arrows" },
			{ GlyphPresetFlags::kCommon, "Common" },

		} };

		UISettings::UISettings(Controller& a_controller) :
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			UIFormPickerWidget(a_controller, FormInfoFlags::kNone, true),
			UIFormLookupInterface(a_controller),
			UIStylePresetSelectorWidget(a_controller),
			UIFormTypeSelectorWidget(a_controller),
			m_controller(a_controller)
		{
			SetAllowedTypes({ BGSSoundDescriptorForm::kTypeID });
		}

		void UISettings::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			SetWindowDimensions(0.0f, 650.f, 825.0f, true);

			if (ImGui::Begin(
					LS<CommonStrings, 3>(CommonStrings::Settings, WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				DrawGeneralSection();
				DrawDisplaysSection();
				DrawGearPosSection();
				DrawObjectDatabaseSection();
				DrawUISection();
				DrawLocalizationSection();
				DrawSoundSection();

				ImGui::PopItemWidth();
			}

			ImGui::End();
		}

		void UISettings::DrawGeneralSection()
		{
			if (CollapsingHeader(
					"tree_general",
					true,
					"%s",
					LS(CommonStrings::General)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawLogLevelSelector();

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		void UISettings::DrawDisplaysSection()
		{
			if (CollapsingHeader(
					"tree_displays",
					true,
					"%s",
					LS(CommonStrings::Displays)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetConfigStore().settings;
				auto& data     = settings.data;

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::KeepEquippedLoaded, "1"),
						std::addressof(data.hideEquipped))))
				{
					m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
				}
				DrawTip(UITip::HideEquipped);

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::DisableNPCEquipmentSlots, "2"),
						std::addressof(data.disableNPCSlots))))
				{
					m_controller.QueueResetGearAll(ControllerUpdateFlags::kNone);
				}

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::NoCheckFav, "3"),
						std::addressof(data.removeFavRestriction))))
				{
					m_controller.QueueEvaluate(
						Data::IData::GetPlayerRefID(),
						ControllerUpdateFlags::kNone);
				}
				DrawTip(UITip::NoCheckFav);

				ImGui::Spacing();

				if (ImGui::TreeNodeEx(
						"gt_tree",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						LS(UISettingsStrings::PlayerGearToggleKeys)))
				{
					ImGui::Spacing();

					auto tmpk = m_controller.GetInputHandlers().playerBlock.GetComboKey();

					if (settings.mark_if(DrawKeySelector(
							LS(CommonStrings::ComboKey, "1"),
							UIData::g_comboControlMap,
							tmpk,
							true)))
					{
						m_controller.GetInputHandlers().playerBlock.SetComboKey(
							tmpk);

						data.playerBlockKeys->comboKey = tmpk;
						data.playerBlockKeys.mark(true);
					}

					tmpk = m_controller.GetInputHandlers().playerBlock.GetKey();

					if (settings.mark_if(DrawKeySelector(
							LS(CommonStrings::Key, "2"),
							UIData::g_controlMap,
							tmpk,
							true)))
					{
						m_controller.GetInputHandlers().playerBlock.SetKey(
							tmpk);

						data.playerBlockKeys->key = tmpk;
						data.playerBlockKeys.mark(true);
					}

					ImGui::Spacing();

					if (settings.mark_if(ImGui::Checkbox(
							LS(UISettingsStrings::KeepLoadedWhenToggledOff, "3"),
							std::addressof(data.toggleKeepLoaded))))
					{
						m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
					}

					ImGui::TreePop();
				}

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		void UISettings::DrawGearPosSection()
		{
			if (CollapsingHeader(
					"tree_gearpos",
					true,
					"%s",
					LS(UISettingsStrings::GearPositioning)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetConfigStore().settings;
				auto& data     = settings.data;

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::XP32AA, "1"),
						std::addressof(data.enableXP32AA))))
				{
					if (data.enableXP32AA)
					{
						m_controller.QueueEvaluateAll(
							ControllerUpdateFlags::kNone);
					}
					else
					{
						m_controller.QueueResetAAAll();
					}
				}
				DrawTip(UITip::XP32AA);

				if (data.enableXP32AA)
				{
					ImGui::Indent();

					if (!m_controller.HasAnimationInfo())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped(
							"%s",
							LS(UISettingsStrings::NoAnimInfoWarning));
						ImGui::PopStyleColor();

						ImGui::Spacing();
					}

					ImGui::AlignTextToFramePadding();
					ImGui::Text("%s:", LS(UISettingsStrings::XP32_FF));
					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							LS(UISettingsStrings::XP32_FF_Idle, "2"),
							std::addressof(data.XP32AABowIdle))))
					{
						m_controller.QueueResetAAAll();
						m_controller.QueueEvaluateAll(
							ControllerUpdateFlags::kNone);
					}

					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							LS(UISettingsStrings::XP32_FF_Attack, "3"),
							std::addressof(data.XP32AABowAtk))))
					{
						m_controller.QueueResetAAAll();
						m_controller.QueueEvaluateAll(
							ControllerUpdateFlags::kNone);
					}

					DrawTip(UITip::XP32AA_FF);

					ImGui::Unindent();
				}

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::RandPlacement, "4"),
						std::addressof(data.placementRandomization))))
				{
					if (!data.placementRandomization)
					{
						m_controller.QueueClearRand();
					}

					m_controller.QueueResetAll(
						ControllerUpdateFlags::kNone);
				}

				DrawTip(UITip::RandPlacement);

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		void UISettings::DrawUISection()
		{
			if (CollapsingHeader(
					"tree_ui",
					true,
					"%s",
					LS(CommonStrings::UI)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetConfigStore().settings;
				auto& ui       = settings.data.ui;

				if (settings.mark_if(DrawStylePresetSelector(ui.stylePreset)))
				{
					Drivers::UI::SetStyle(ui.stylePreset);
				}

				if (settings.mark_if(ImGui::SliderFloat(
						LS(UISettingsStrings::Alpha, "1"),
						std::addressof(ui.alpha),
						0.15f,
						1.0f,
						"%.2f")))
				{
					Drivers::UI::SetAlpha(ui.alpha);
				}

				DrawCommonResetContextMenu(
					"ctx_rst_bg",
					CommonStrings::Reset,
					ui.bgAlpha.has(),
					[&] {
						ui.bgAlpha.clear();
						Drivers::UI::SetBGAlpha(ui.bgAlpha);
					});

				bool hasAlpha = ui.bgAlpha.has();

				float tmpbga = hasAlpha ?
				                   *ui.bgAlpha :
                                   ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;

				if (!hasAlpha)
				{
					ImGui::PushStyleVar(
						ImGuiStyleVar_Alpha,
						ImGui::GetStyle().Alpha * 0.65f);
				}

				if (settings.mark_if(ImGui::SliderFloat(
						LS(UISettingsStrings::BGAlpha, "2"),
						std::addressof(tmpbga),
						0.1f,
						1.0f,
						"%.2f")))
				{
					ui.bgAlpha = tmpbga;
					Drivers::UI::SetBGAlpha(ui.bgAlpha);
				}

				if (!hasAlpha)
				{
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Columns(2, nullptr, false);

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::ShowIntroBanner, "3"),
						std::addressof(ui.showIntroBanner))))
				{
					if (!ui.showIntroBanner)
					{
						Drivers::UI::QueueRemoveTask(-0xFFFF);
					}
				}

				settings.mark_if(ImGui::Checkbox(
					LS(UISettingsStrings::CloseOnEsc, "4"),
					std::addressof(ui.closeOnESC)));

				DrawTip(UITip::CloseOnESC);

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::ControlLock, "6"),
						std::addressof(ui.enableControlLock))))
				{
					m_controller.UIGetRenderTask()->SetLock(ui.enableControlLock);
					Drivers::UI::EvaluateTaskState();
				}
				DrawTip(UITip::ControlLock);

				ImGui::NextColumn();

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::FreezeTime, "7"),
						std::addressof(ui.enableFreezeTime))))
				{
					m_controller.UIGetRenderTask()->SetFreeze(ui.enableFreezeTime);
					Drivers::UI::EvaluateTaskState();
				}
				DrawTip(UITip::FreezeTime);

				settings.mark_if(ImGui::Checkbox(
					LS(UISettingsStrings::SelectCrosshairActor, "8"),
					std::addressof(ui.selectCrosshairActor)));

				DrawTip(UITip::SelectCrosshairActor);

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::EnableRestrictions, "5"),
						std::addressof(ui.enableRestrictions))))
				{
					m_controller.UIGetRenderTask()->EnableRestrictions(ui.enableRestrictions);
				}
				DrawTip(UITip::EnableRestrictions);

				ImGui::Columns();

				ImGui::Spacing();

				auto tmp = m_scaleTemp ?
				               *m_scaleTemp :
                               ui.scale;

				if (ImGui::SliderFloat(
						LS(CommonStrings::Scale, "9"),
						std::addressof(tmp),
						0.2f,
						5.0f,
						"%.2f",
						ImGuiSliderFlags_AlwaysClamp))
				{
					m_scaleTemp = tmp;
				}
				DrawTip(UITip::UIScale);

				if (m_scaleTemp)
				{
					if (ImGui::Button(LS(CommonStrings::Apply, "A")))
					{
						Drivers::UI::QueueSetScale(*m_scaleTemp);
						settings.set(ui.scale, m_scaleTemp.clear_and_get());
					}
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (TreeEx(
						"font_ctl",
						true,
						"%s",
						LS(CommonStrings::Fonts)))
				{
					ImGui::Indent();
					ImGui::Spacing();

					DrawFontSelector();
					ImGui::Spacing();
					DrawExtraGlyphs();
					ImGui::Spacing();
					DrawFontMiscOptions();

					ImGui::Spacing();
					ImGui::Unindent();

					ImGui::TreePop();
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (TreeEx(
						"if_toggle",
						true,
						"%s",
						LS(UISettingsStrings::InterfaceOpenKeys)))
				{
					ImGui::Indent();
					ImGui::Spacing();

					if (m_controller.UIGetIniKeysForced())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped("%s", LS(UISettingsStrings::UIKeyOverrideWarning));
						ImGui::PopStyleColor();
						ImGui::Spacing();
					}

					auto tmpk = m_controller.GetInputHandlers().uiOpen.GetComboKey();

					if (settings.mark_if(DrawKeySelector(
							LS(CommonStrings::ComboKey, "1"),
							UIData::g_comboControlMap,
							tmpk,
							true)))
					{
						m_controller.GetInputHandlers().uiOpen.SetComboKey(
							tmpk);

						ui.openKeys->comboKey = tmpk;
						ui.openKeys.mark(true);
					}

					tmpk = m_controller.GetInputHandlers().uiOpen.GetKey();

					if (settings.mark_if(DrawKeySelector(
							LS(CommonStrings::Key, "2"),
							UIData::g_controlMap,
							tmpk)))
					{
						m_controller.GetInputHandlers().uiOpen.SetKey(tmpk);

						ui.openKeys->key = tmpk;
						ui.openKeys.mark(true);
					}

					ImGui::Spacing();
					ImGui::Unindent();

					ImGui::TreePop();
				}

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		auto UISettings::DrawSoundContextMenu(
			Data::ConfigSound<Game::FormID>& a_data)
			-> ContextMenuAction
		{
			ContextMenuAction result{ ContextMenuAction ::None };

			ImGui::PushID("context_area");

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_tmpFormType.reset();
			}

			ImGui::SameLine();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::BeginMenu(
						LS(CommonStrings::Add, "1"),
						a_data.enabled))
				{
					if (DrawFormTypeSelector(
							m_tmpFormType,
							[](std::uint8_t a_formType) {
								return IFormCommon::IsValidCustomFormType(a_formType);
							}))
					{
						a_data.data.try_emplace(*m_tmpFormType);

						result = ContextMenuAction::Add;

						m_tmpFormType.reset();

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UISettings::DrawSoundSection()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			ImGui::PushID("sound_sect");

			const auto result = DrawSoundContextMenu(settings.data.sound);

			if (result == ContextMenuAction::Add)
			{
				settings.mark_dirty();
				ImGui::SetNextItemOpen(true);
			}

			if (CollapsingHeader(
					"tree",
					false,
					"%s",
					LS(CommonStrings::Sound)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				if (settings.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::EnableEquipSounds, "1"),
						std::addressof(settings.data.sound.enabled))))
				{
					m_controller.SetPlaySound(settings.data.sound.enabled);
				}

				if (settings.data.sound.enabled)
				{
					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							LS(UISettingsStrings::NPCSounds, "2"),
							std::addressof(settings.data.sound.npc))))
					{
						m_controller.SetPlaySoundNPC(settings.data.sound.npc);
					}

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (DrawSoundPairs())
					{
						settings.mark_dirty();
						m_controller.QueueUpdateSoundForms();
					}
				}

				ImGui::Unindent();
				ImGui::Spacing();
			}

			ImGui::PopID();
		}

		void UISettings::DrawLogLevelSelector()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			auto current = gLog.GetLogLevel();
			auto desc    = ILog::GetLogLevelString(current);

			if (ImGui::BeginCombo(
					LS(UISettingsStrings::LogLevel, "ll_sel"),
					desc))
			{
				for (auto& [i, e] : ILog::GetLogLevels())
				{
					bool selected = e == current;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(i.c_str(), selected))
					{
						settings.data.logLevel = e;
						settings.mark_dirty();
						gLog.SetLogLevel(e);
					}
				}

				ImGui::EndCombo();
			}
		}

		void UISettings::DrawObjectDatabaseSection()
		{
			if (CollapsingHeader(
					"tree_mdb",
					true,
					"%s",
					LS(UIWidgetCommonStrings::ModelDatabase)))
			{
				auto& settings = m_controller.GetConfigStore().settings;

				char        buf[std::numeric_limits<std::uint32_t>::digits10 + 3];
				const char* preview;

				auto current = m_controller.GetODBLevel();

				auto it = std::find_if(
					s_odbmvals.begin(),
					s_odbmvals.end(),
					[current](auto& a_v) {
						return a_v.first == current;
					});

				if (it != s_odbmvals.end())
				{
					preview = LS(it->second);
				}
				else
				{
					stl::snprintf(buf, "%u", stl::underlying(current));
					preview = buf;
				}

				ImGui::Spacing();
				ImGui::Indent();

				if (ImGui::BeginCombo(
						LS(CommonStrings::Caching, "1"),
						preview))
				{
					for (auto& e : s_odbmvals)
					{
						ImGui::PushID(stl::underlying(e.first));

						bool selected = e.first == current;
						if (selected)
						{
							if (ImGui::IsWindowAppearing())
								ImGui::SetScrollHereY();
						}

						if (ImGui::Selectable(LS<CommonStrings, 3>(e.second, "1"), selected))
						{
							settings.data.odbLevel = e.first;
							settings.mark_dirty();
							m_controller.SetObjectDatabaseLevel(e.first);
						}

						ImGui::PopID();
					}

					ImGui::EndCombo();
				}

				DrawTip(UITip::ModelCache);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(LS(UISettingsStrings::CachedModelsColon));
				ImGui::SameLine();
				ImGui::Text("%zu", m_controller.GetODBObjectCount());
				ImGui::SameLine();

				if (ImGui::Button(LS(CommonStrings::Clear, "2")))
				{
					m_controller.QueueObjectDatabaseClear();
				}

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		void UISettings::DrawLocalizationSection()
		{
			if (CollapsingHeader(
					"tree_localization",
					true,
					"%s",
					LS(CommonStrings::Localization)))
			{
				ImGui::Indent();
				ImGui::Spacing();

				auto& settings = m_controller.GetConfigStore().settings;
				auto& ldm      = Localization::LocalizationDataManager::GetSingleton();
				auto& current  = m_controller.GetCurrentLanguageTable();

				if (ImGui::BeginCombo(
						LS(CommonStrings::Language, "1"),
						current ?
							current->GetLang().c_str() :
                            nullptr,
						ImGuiComboFlags_HeightLarge))
				{
					for (auto& e : ldm.GetData())
					{
						ImGui::PushID(std::addressof(e));

						bool selected = (current && e.first == current->GetLang());
						if (selected)
						{
							if (ImGui::IsWindowAppearing())
								ImGui::SetScrollHereY();
						}

						if (ImGui::Selectable(LMKID<3>(e.first.c_str(), "1"), selected))
						{
							settings.data.language = e.first;
							settings.mark_dirty();

							m_controller.QueueSetLanguage(e.first);
						}

						ImGui::PopID();
					}

					ImGui::EndCombo();
				}

				ImGui::Spacing();
				ImGui::Unindent();
			}
		}

		void UISettings::DrawFontSelector()
		{
			ImGui::PushID("font_selector");

			auto& settings = m_controller.GetConfigStore().settings;

			auto current = Drivers::UI::GetCurrentFont();

			DrawCommonResetContextMenu(
				"ctx_font_rld",
				CommonStrings::Reload,
				true,
				[&] {
					Drivers::UI::MarkFontUpdateDataDirty();
				});

			if (ImGui::BeginCombo(
					LS(CommonStrings::Font, "1"),
					current->first.c_str(),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : Drivers::UI::GetAvailableFonts())
				{
					ImGui::PushID(std::addressof(e));

					bool selected = e == current->first;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(LMKID<3>(e.c_str(), "1"), selected))
					{
						Drivers::UI::QueueFontChange(e);

						settings.data.ui.font = e;
						settings.mark_dirty();
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			DrawCommonResetContextMenu(
				"ctx_clr_fsz",
				CommonStrings::Clear,
				settings.data.ui.fontSize.has(),
				[&] {
					settings.data.ui.fontSize.clear();
					settings.mark_dirty();

					Drivers::UI::QueueResetFontSize();
				});

			float tmp;
			if (m_fontSizeTemp)
			{
				tmp = *m_fontSizeTemp;
			}
			else if (settings.data.ui.fontSize)
			{
				tmp = *settings.data.ui.fontSize;
			}
			else
			{
				tmp = Drivers::UI::GetCurrentFont()->second.size;
			}

			if (ImGui::SliderFloat(
					LS(CommonStrings::Size, "3"),
					std::addressof(tmp),
					1.0f,
					140.0f,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				m_fontSizeTemp = tmp;
			}

			if (m_fontSizeTemp)
			{
				ImGui::SameLine();
				if (ImGui::Button(LS(CommonStrings::Apply, "4")))
				{
					Drivers::UI::QueueSetFontSize(*m_fontSizeTemp);
					settings.set(settings.data.ui.fontSize, m_fontSizeTemp.clear_and_get());
				}
			}

			ImGui::PopID();
		}

		void UISettings::DrawExtraGlyphs()
		{
			if (TreeEx(
					"extra_glyphs",
					false,
					"%s",
					LS(UISettingsStrings::ExtraGlyphs)))
			{
				ImGui::Indent();
				ImGui::Spacing();

				auto& settings = m_controller.GetConfigStore().settings;

				decltype(s_extraGlyphs)::size_type i = 0;

				auto mid = s_extraGlyphs.size() / 2;
				bool sw  = false;

				ImGui::Columns(2, nullptr, false);

				for (auto& e : s_extraGlyphs)
				{
					ImGui::PushID(stl::underlying(e.first.value));

					if (settings.mark_if(ImGui::CheckboxFlagsT(
							e.second,
							stl::underlying(std::addressof(settings.data.ui.extraGlyphs.value)),
							stl::underlying(e.first.value))))
					{
						Drivers::UI::QueueSetExtraGlyphs(settings.data.ui.extraGlyphs);
					}

					ImGui::PopID();

					i++;

					if (!sw && i >= mid)
					{
						ImGui::NextColumn();
						sw = true;
					}
				}

				ImGui::Columns();

				ImGui::Spacing();
				ImGui::Unindent();

				ImGui::TreePop();
			}
		}

		void UISettings::DrawFontMiscOptions()
		{
			auto& settings = m_controller.GetConfigStore().settings;
			auto& ui       = settings.data.ui;

			ImGui::PushID("font_misc_opt");

			if (settings.mark_if(ImGui::Checkbox(
					LS(UISettingsStrings::ReleaseFontData, "1"),
					std::addressof(ui.releaseFontData))))
			{
				Drivers::UI::SetReleaseFontData(ui.releaseFontData);
			}

			DrawTipWarn(UITip::ReleaseFontData);

			ImGui::PopID();
		}

		bool UISettings::DrawSoundPairs()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			bool result = false;

			auto& data = settings.data.sound.data;

			ImGui::PushID("snd_pairs");

			for (auto it = data.begin(); it != data.end();)
			{
				ImGui::PushID(it->first);

				const auto r = DrawSoundPairContextMenu();

				if (r == ContextMenuAction::Delete)
				{
					it     = data.erase(it);
					result = true;
				}
				else
				{
					result |= DrawSoundPair(it->first, it->second);
					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();

			return result;
		}

		auto UISettings::DrawSoundPairContextMenu()
			-> ContextMenuAction
		{
			ContextMenuAction result{ ContextMenuAction ::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_MI(CommonStrings::Delete, "1"))
				{
					result = ContextMenuAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UISettings::DrawSoundPair(
			std::uint8_t                                  a_formType,
			Data::ConfigSound<Game::FormID>::soundPair_t& a_soundPair)
		{
			bool result = false;

			bool r;

			if (auto desc = form_type_to_desc(a_formType))
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					desc);
			}
			else
			{
				r = ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%hhu",
					a_formType);
			}

			if (r)
			{
				ImGui::Indent();
				ImGui::Spacing();

				if (DrawFormPicker(
						"1",
						static_cast<Localization::StringID>(CommonStrings::Equip),
						*a_soundPair.first))
				{
					result = true;
					a_soundPair.first.mark(*a_soundPair.first != 0);
				}

				if (DrawFormPicker(
						"2",
						static_cast<Localization::StringID>(CommonStrings::Unequip),
						*a_soundPair.second))
				{
					result = true;
					a_soundPair.second.mark(*a_soundPair.second != 0);
				}

				ImGui::Spacing();
				ImGui::Unindent();

				ImGui::TreePop();
			}

			return result;
		}

		auto UISettings::GetCollapsibleStatesData()
			-> UIData::UICollapsibleStates&
		{
			return m_controller.GetConfigStore().settings.data.ui.settingsColStates;
		}

		void UISettings::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UISettings::DrawMenuBar()
		{
			ImGui::PushID("menu_bar");

			if (ImGui::BeginMenuBar())
			{
				if (LCG_BM(CommonStrings::File, "1"))
				{
					DrawFileMenu();
					ImGui::EndMenu();
				}

				if (LCG_BM(CommonStrings::Tools, "2"))
				{
					DrawToolsMenu();
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::PopID();
		}

		void UISettings::DrawFileMenu()
		{
			if (LCG_MI(CommonStrings::Save, "1"))
			{
				m_controller.GetConfigStore().settings.SaveIfDirty();
			}

			ImGui::Separator();

			if (LCG_MI(CommonStrings::Exit, "2"))
			{
				SetOpenState(false);
			}
		}

		void UISettings::DrawToolsMenu()
		{
			if (LCG_BM(CommonStrings::Maintenance, "1"))
			{
				DrawMaintenanceMenu();

				ImGui::EndMenu();
			}
		}

		void UISettings::DrawMaintenanceMenu()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			if (LCG_MI(UISettingsStrings::ClearStoredColStates, "1"))
			{
				settings.data.ui.settingsColStates.clear();
				settings.data.ui.statsColStates.clear();

				for (auto& e : settings.data.ui.slotEditor.colStates)
				{
					e.clear();
				}

				for (auto& e : settings.data.ui.customEditor.colStates)
				{
					e.clear();
				}

				for (auto& e : settings.data.ui.transformEditor.colStates)
				{
					e.clear();
				}

				settings.data.ui.slotProfileEditor.colStates.clear();
				settings.data.ui.customProfileEditor.colStates.clear();
				settings.data.ui.transformProfileEditor.colStates.clear();

				settings.mark_dirty();
			}
		}

	}
}