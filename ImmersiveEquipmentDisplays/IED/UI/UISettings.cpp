#include "pch.h"

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

				DrawGeneralSection();
				DrawDisplaysSection();
				DrawObjectDatabaseSection();
				DrawUISection();
				DrawLocalizationSection();
				DrawSoundSection();
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

				auto& config = m_controller.GetConfigStore().settings;
				auto& data = config.data;

				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::KeepEquippedLoaded, "1"),
						std::addressof(data.hideEquipped))))
				{
					m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
				}

				DrawTip(UITip::HideEquipped);

				ImGui::Spacing();

				if (ImGui::TreeNodeEx(
						"gt_tree",
						ImGuiTreeNodeFlags_SpanAvailWidth |
							ImGuiTreeNodeFlags_DefaultOpen,
						"%s",
						LS(UISettingsStrings::PlayerGearToggleKeys)))
				{
					ImGui::Spacing();

					auto tmpk = m_controller.GetInputHandlers().playerBlock.GetComboKey();

					if (config.mark_if(DrawKeySelector(
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

					if (config.mark_if(DrawKeySelector(
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

					ImGui::TreePop();
				}

				ImGui::Spacing();

				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::KeepLoadedWhenToggledOff, "2"),
						std::addressof(data.toggleKeepLoaded))))
				{
					m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
				}
				
				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::DisableNPCEquipmentSlots, "3"),
						std::addressof(data.disableNPCSlots))))
				{
					m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
				}

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

				auto& config = m_controller.GetConfigStore().settings;
				auto& ui = config.data.ui;

				config.mark_if(ImGui::Checkbox(
					LS(UISettingsStrings::CloseOnEsc, "1"),
					std::addressof(ui.closeOnESC)));

				DrawTip(UITip::CloseOnESC);

				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::EnableRestrictions, "2"),
						std::addressof(ui.enableRestrictions))))
				{
					m_controller.UIEnableRestrictions(ui.enableRestrictions);
				}
				DrawTip(UITip::EnableRestrictions);

				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::ControlLock, "3"),
						std::addressof(ui.enableControlLock))))
				{
					m_controller.UISetLock(ui.enableControlLock);
				}
				DrawTip(UITip::ControlLock);

				config.mark_if(ImGui::Checkbox(
					LS(UISettingsStrings::SelectCrosshairActor, "4"),
					std::addressof(ui.selectCrosshairActor)));

				DrawTip(UITip::SelectCrosshairActor);

				ImGui::Spacing();

				auto tmp = m_scaleTemp ?
                               *m_scaleTemp :
                               ui.scale;

				if (ImGui::SliderFloat(
						LS(CommonStrings::Scale, "5"),
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
					//ImGui::SameLine();

					if (ImGui::Button(LS(CommonStrings::Apply, "6")))
					{
						Drivers::UI::QueueSetScale(*m_scaleTemp);
						config.set(ui.scale, m_scaleTemp.clear_and_get());
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

					if (config.mark_if(DrawKeySelector(
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

					if (config.mark_if(DrawKeySelector(
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

		void UISettings::DrawSoundSection()
		{
			if (CollapsingHeader(
					"tree_snd",
					false,
					"%s",
					LS(CommonStrings::Sound)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				auto& config = m_controller.GetConfigStore().settings;

				if (config.mark_if(ImGui::Checkbox(
						LS(UISettingsStrings::EnableEquipSounds, "1"),
						std::addressof(config.data.sound.enabled))))
				{
					m_controller.SetPlaySound(config.data.sound.enabled);
				}

				if (config.data.sound.enabled)
				{
					ImGui::SameLine();

					if (config.mark_if(ImGui::Checkbox(
							LS(UISettingsStrings::NPCSounds, "2"),
							std::addressof(config.data.sound.npc))))
					{
						m_controller.SetPlaySoundNPC(config.data.sound.npc);
					}

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (DrawSoundPairs())
					{
						m_controller.QueueUpdateSoundForms();
					}
				}

				ImGui::Unindent();
				ImGui::Spacing();
			}
		}

		void UISettings::DrawLogLevelSelector()
		{
			auto& config = m_controller.GetConfigStore().settings;

			auto current = gLog.GetLogLevel();
			auto desc = ILog::GetLogLevelString(current);

			if (ImGui::BeginCombo(LS(UISettingsStrings::LogLevel, "ll_sel"), desc))
			{
				for (auto& e : ILog::GetLogLevels())
				{
					bool selected = e.second == current;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(e.first.c_str(), selected))
					{
						config.data.logLevel = e.second;
						config.mark_dirty();
						gLog.SetLogLevel(e.second);
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
				auto& config = m_controller.GetConfigStore().settings;

				char buf[std::numeric_limits<std::uint32_t>::digits10 + 3];
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

				if (ImGui::BeginCombo(LS(CommonStrings::Caching, "1"), preview))
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

						if (ImGui::Selectable(LS(e.second), selected))
						{
							config.data.odbLevel = e.first;
							config.mark_dirty();
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

				auto& config = m_controller.GetConfigStore().settings;
				auto& ldm = Localization::LocalizationDataManager::GetSingleton();
				auto& current = m_controller.GetCurrentLanguageTable();

				const char* desc = current ?
                                       current->GetLang().c_str() :
                                       nullptr;

				if (ImGui::BeginCombo(LS(CommonStrings::Language, "1"), desc))
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

						if (ImGui::Selectable(e.first.c_str(), selected))
						{
							config.data.language = e.first;
							config.mark_dirty();

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

			auto& config = m_controller.GetConfigStore().settings;

			auto current = Drivers::UI::GetCurrentFont();

			if (ImGui::BeginCombo(
					LS(CommonStrings::Font, "1"),
					current->first.c_str()))
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

					if (ImGui::Selectable(e.c_str(), selected))
					{
						Drivers::UI::QueueFontChange(e);

						config.data.ui.font = e;
						config.mark_dirty();
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::SameLine();
			if (ImGui::Button(LS(CommonStrings::Reload, "2")))
			{
				Drivers::UI::MarkFontUpdateDataDirty();
			}

			float tmp;
			if (m_fontSizeTemp)
			{
				tmp = *m_fontSizeTemp;
			}
			else if (config.data.ui.fontSize)
			{
				tmp = *config.data.ui.fontSize;
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
				//ImGui::SameLine();
				if (ImGui::Button(LS(CommonStrings::Apply, "4")))
				{
					Drivers::UI::QueueSetFontSize(*m_fontSizeTemp);
					config.set(config.data.ui.fontSize, m_fontSizeTemp.clear_and_get());
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

				auto& config = m_controller.GetConfigStore().settings;

				decltype(s_extraGlyphs)::size_type i = 0;

				auto mid = s_extraGlyphs.size() / 2;
				bool sw = false;

				ImGui::Columns(2, nullptr, false);

				for (auto& e : s_extraGlyphs)
				{
					ImGui::PushID(stl::underlying(e.first.value));

					if (config.mark_if(ImGui::CheckboxFlagsT(
							e.second,
							stl::underlying(std::addressof(config.data.ui.extraGlyphs.value)),
							stl::underlying(e.first.value))))
					{
						Drivers::UI::QueueSetExtraGlyphs(config.data.ui.extraGlyphs);
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

		bool UISettings::DrawSoundPairs()
		{
			auto& config = m_controller.GetConfigStore().settings;

			bool result = false;

			ImGui::PushID("snd_pairs");

			result |= config.mark_if(DrawSoundPair(
				"1",
				stl::underlying(CommonStrings::Weapon),
				config.data.sound.weapon));

			result |= config.mark_if(DrawSoundPair(
				"3",
				stl::underlying(CommonStrings::Armor),
				config.data.sound.armor));

			result |= config.mark_if(DrawSoundPair(
				"2",
				stl::underlying(CommonStrings::Arrow),
				config.data.sound.arrow));

			result |= config.mark_if(DrawSoundPair(
				"4",
				stl::underlying(CommonStrings::Generic),
				config.data.sound.gen));

			ImGui::PopID();

			return result;
		}

		bool UISettings::DrawSoundPair(
			const char* a_strid,
			Localization::StringID a_label,
			Data::ConfigSound<Game::FormID>::soundPair_t& a_soundPair)
		{
			bool result = false;

			if (TreeEx(
					a_strid,
					true,
					"%s",
					LS(a_label)))
			{
				ImGui::Indent();
				ImGui::Spacing();

				if ((result |= DrawFormPicker("1", LS(CommonStrings::Equip), *a_soundPair.first)))
				{
					a_soundPair.first.mark(true);
				}

				if ((result |= DrawFormPicker("2", LS(CommonStrings::Unequip), *a_soundPair.second)))
				{
					a_soundPair.second.mark(true);
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

			if (settings.data.ui.fontSize)
			{
				if (LCG_MI(UISettingsStrings::ClearFontSizeOverride, "2"))
				{
					auto& queue = m_controller.UIGetPopupQueue();

					queue.push(
							 UIPopupType::Confirm,
							 LS(CommonStrings::Confirm),
							 "%s",
							 LS(UISettingsStrings::ClearFontSizeOverridePrompt))
						.call([this](auto&) {
							auto& settings = m_controller.GetConfigStore().settings;

							settings.data.ui.fontSize.clear();
							settings.mark_dirty();

							Drivers::UI::QueueResetFontSize();
						});
				}
			}
		}

	}
}