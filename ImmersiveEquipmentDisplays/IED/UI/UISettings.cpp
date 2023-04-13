#include "pch.h"

#include "UIMain.h"
#include "UISettings.h"
#include "UISettingsStrings.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/ObjectDatabaseLevel.h"
#include "IED/EngineExtensions.h"
#include "IED/ReferenceLightController.h"
#include "IED/StringHolder.h"

#include "Widgets/UIWidgetCommonStrings.h"

#include "Drivers/UI.h"
#include "Drivers/UI/Tasks.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_odbmvals = stl::make_array(

			std::make_pair(ObjectDatabaseLevel::kNone, CommonStrings::UsedOnly),
			std::make_pair(ObjectDatabaseLevel::kMin, CommonStrings::Minimum),
			std::make_pair(ObjectDatabaseLevel::kVeryLow, CommonStrings::VeryLow),
			std::make_pair(ObjectDatabaseLevel::kLow, CommonStrings::Low),
			std::make_pair(ObjectDatabaseLevel::kMedium, CommonStrings::Medium),
			std::make_pair(ObjectDatabaseLevel::kHigh, CommonStrings::High),
			std::make_pair(ObjectDatabaseLevel::kVeryHigh, CommonStrings::VeryHigh),
			std::make_pair(ObjectDatabaseLevel::kExtreme, CommonStrings::Extreme),
			std::make_pair(ObjectDatabaseLevel::kMax, CommonStrings::Maximum)

		);

		static constexpr auto s_extraGlyphs = stl::make_array(
			std::make_pair(GlyphPresetFlags::kLatinFull, "Latin full"),
			std::make_pair(GlyphPresetFlags::kCyrilic, "Cyrilic"),
			std::make_pair(GlyphPresetFlags::kJapanese, "Japanese"),
			std::make_pair(GlyphPresetFlags::kChineseSimplifiedCommon, "Chinese simplified common"),
			std::make_pair(GlyphPresetFlags::kChineseFull, "Chinese full"),
			std::make_pair(GlyphPresetFlags::kKorean, "Korean"),
			std::make_pair(GlyphPresetFlags::kThai, "Thai"),
			std::make_pair(GlyphPresetFlags::kVietnamise, "Vietnamise"),
			std::make_pair(GlyphPresetFlags::kGreek, "Greek"),
			std::make_pair(GlyphPresetFlags::kArabic, "Arabic"),
			std::make_pair(GlyphPresetFlags::kArrows, "Arrows"),
			std::make_pair(GlyphPresetFlags::kCommon, "Common"));

		UISettings::UISettings(
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller) :
			UIFormPickerWidget(a_controller, FormInfoFlags::kNone, true),
			UIFormLookupInterface(a_controller),
			m_owner(a_owner),
			m_controller(a_controller)
		{
			SetAllowedTypes({ BGSSoundDescriptorForm::kTypeID });
		}

		void UISettings::Draw()
		{
			SetWindowDimensions(0.0f, 650.f, 825.0f, true);

			if (ImGui::Begin(
					UIL::LS<CommonStrings, 3>(CommonStrings::Settings, WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				DrawGeneralSection();
				DrawDisplaysSection();
				DrawGearPosSection();
				DrawEffectsSection();
				DrawObjectDatabaseSection();
				DrawUISection();
				DrawLocalizationSection();
				DrawSoundSection();
#if defined(IED_ENABLE_I3DI)
				//DrawI3DISection();
#endif

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
					UIL::LS(CommonStrings::General)))
			{
				ImGui::PushID("1");

				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetSettings();

				auto currentLogLevel = gLog.GetLogLevel();

				if (DrawLogLevelSelector(
						"1",
						static_cast<Localization::StringID>(UISettingsStrings::LogLevel),
						currentLogLevel))
				{
					settings.set(settings.data.logLevel, currentLogLevel);
					gLog.SetLogLevel(currentLogLevel);
				}

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(CommonStrings::Notifications, "2"),
						std::addressof(settings.data.ui.enableNotifications))))
				{
					m_controller.UIEnableNotifications(settings.data.ui.enableNotifications);
				}

				if (settings.data.ui.enableNotifications)
				{
					ImGui::Indent();

					if (DrawLogLevelSelector(
							"3",
							static_cast<Localization::StringID>(CommonStrings::Threshold),
							settings.data.ui.notificationThreshold))
					{
						if (settings.data.ui.notificationThreshold > currentLogLevel)
						{
							settings.data.ui.notificationThreshold = currentLogLevel;
						}

						settings.mark_dirty();

						m_controller.UISetLogNotificationThreshold(
							settings.data.ui.notificationThreshold);
					}

					ImGui::Unindent();
				}

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::ParallelUpdates, "4"),
						std::addressof(settings.data.apParallelUpdates))))
				{
					m_controller.SetProcessorTaskParallelUpdates(
						settings.data.apParallelUpdates);
				}
				UITipsInterface::DrawTip(UITip::EffectShadersParallelUpdates);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		void UISettings::DrawDisplaysSection()
		{
			if (CollapsingHeader(
					"tree_displays",
					true,
					"%s",
					UIL::LS(CommonStrings::Displays)))
			{
				ImGui::PushID("2");

				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetSettings();
				auto& data     = settings.data;

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::KeepEquippedLoaded, "1"),
						std::addressof(data.hideEquipped))))
				{
					m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
				}
				UITipsInterface::DrawTip(UITip::HideEquipped);

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::DisableNPCEquipmentSlots, "2"),
						std::addressof(data.disableNPCSlots))))
				{
					m_controller.QueueResetGearAll(ControllerUpdateFlags::kNone);
				}

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::NoCheckFav, "3"),
						std::addressof(data.removeFavRestriction))))
				{
					m_controller.QueueEvaluate(
						Data::IData::GetPlayerRefID(),
						ControllerUpdateFlags::kNone);
				}
				UITipsInterface::DrawTip(UITip::NoCheckFav);

				/*if (data.hkWeaponAnimations)
				{
					ImGui::Indent();

					if (settings.mark_if(ImGui::Checkbox(
							UILI::LS(UISettingsStrings::AnimEventForwarding, "5"),
							std::addressof(data.animEventForwarding))))
					{
						m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
					}
					UITipsInterface::DrawTip(UITip::AnimEventForwarding);

					ImGui::Unindent();
				}*/

				ImGui::Spacing();

				if (ImGui::TreeNodeEx(
						"gt_tree",
						ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						UIL::LS(UISettingsStrings::PlayerGearToggleKeys)))
				{
					ImGui::Spacing();

					auto tmpk = m_controller.GetInputHandlers().playerBlock.GetComboKey();

					if (settings.mark_if(
							UIControlKeySelectorWidget::DrawKeySelector(
								"1",
								static_cast<Localization::StringID>(CommonStrings::ComboKey),
								UIData::g_comboControlMap,
								tmpk,
								true)))
					{
						m_controller.GetInputHandlers().playerBlock.SetComboKey(
							tmpk);

						data.playerBlockKeys->comboKey = tmpk;
						data.playerBlockKeys->key      = m_controller.GetInputHandlers().playerBlock.GetKey();
						data.playerBlockKeys.mark(true);
					}

					tmpk = m_controller.GetInputHandlers().playerBlock.GetKey();

					if (settings.mark_if(UIControlKeySelectorWidget::DrawKeySelector(
							"2",
							static_cast<Localization::StringID>(CommonStrings::Key),
							UIData::g_controlMap,
							tmpk,
							true)))
					{
						m_controller.GetInputHandlers().playerBlock.SetKey(
							tmpk);

						data.playerBlockKeys->key      = tmpk;
						data.playerBlockKeys->comboKey = m_controller.GetInputHandlers().playerBlock.GetComboKey();
						data.playerBlockKeys.mark(true);
					}

					ImGui::TreePop();
				}

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		void UISettings::DrawGearPosSection()
		{
			if (CollapsingHeader(
					"tree_gearpos",
					true,
					"%s",
					UIL::LS(UISettingsStrings::GearPositioning)))
			{
				ImGui::PushID("3");

				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetSettings();
				auto& data     = settings.data;

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::XP32AA, "1"),
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
				UITipsInterface::DrawTip(UITip::XP32AA);

				if (data.enableXP32AA)
				{
					ImGui::Indent();

					if (!m_controller.HasAnimationInfo())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped(
							"%s",
							UIL::LS(UISettingsStrings::NoAnimInfoWarning));
						ImGui::PopStyleColor();

						ImGui::Spacing();
					}

					ImGui::AlignTextToFramePadding();
					ImGui::Text("%s:", UIL::LS(UISettingsStrings::XP32_FF));
					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							UIL::LS(UISettingsStrings::XP32_FF_Idle, "2"),
							std::addressof(data.XP32AABowIdle))))
					{
						m_controller.QueueResetAAAll();
						m_controller.QueueEvaluateAll(
							ControllerUpdateFlags::kNone);
					}

					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							UIL::LS(UISettingsStrings::XP32_FF_Attack, "3"),
							std::addressof(data.XP32AABowAtk))))
					{
						m_controller.QueueResetAAAll();
						m_controller.QueueEvaluateAll(
							ControllerUpdateFlags::kNone);
					}

					UITipsInterface::DrawTip(UITip::XP32AA_FF);

					ImGui::Unindent();
				}

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::RandPlacement, "4"),
						std::addressof(data.placementRandomization))))
				{
					if (!data.placementRandomization)
					{
						m_controller.QueueClearRand();
					}

					m_controller.QueueResetAll(
						ControllerUpdateFlags::kNone);
				}
				UITipsInterface::DrawTip(UITip::RandPlacement);

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::SyncToFirstPerson, "5"),
						std::addressof(data.syncTransformsToFirstPersonSkeleton))))
				{
					m_controller.QueueResetAll(
						ControllerUpdateFlags::kNone);
				}
				//UITipsInterface::DrawTip(UITip::SyncToFirstPerson);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		void UISettings::DrawEffectsSection()
		{
			if (CollapsingHeader(
					"tree_es",
					false,
					"%s",
					UIL::LS(UISettingsStrings::Effects)))
			{
				ImGui::PushID("4");

				auto& settings = m_controller.GetSettings();
				auto& data     = settings.data;

				ImGui::Spacing();
				ImGui::Indent();

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::EffectShaders, "1"),
						std::addressof(data.enableEffectShaders))))
				{
					m_controller.SetShaderProcessingEnabled(
						data.enableEffectShaders);

					m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
				}

				bool disabled = !m_controller.CPUHasSSE41();

				UICommon::PushDisabled(disabled);

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::EquipmentPhysics, "2"),
						std::addressof(data.enableEquipmentPhysics))))
				{
					m_controller.SetPhysicsProcessingEnabled(
						data.enableEquipmentPhysics);

					m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
				}
				UITipsInterface::DrawTip(UITip::EquipmentPhysics);

				if (disabled)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
					ImGui::TextWrapped(
						"%s",
						UIL::LS(UIWidgetCommonStrings::NoSSE41SupportWarning));
					ImGui::PopStyleColor();
				}

				if (data.enableEquipmentPhysics)
				{
					ImGui::Indent();

					if (settings.mark_if(ImGui::DragFloat(
							UIL::LS(UISettingsStrings::MaxDiff, "3"),
							std::addressof(data.physics.maxDiff),
							ImGui::GetIO().KeyShift ? 0.0005f : 0.25f,
							512.0f,
							8192.0f,
							"%.2f",
							ImGuiSliderFlags_AlwaysClamp)))
					{
						PHYSimComponent::SetMaxDiff(data.physics.maxDiff);
					}

					ImGui::Unindent();
				}

				UICommon::PopDisabled(disabled);

				auto& rlc = ReferenceLightController::GetSingleton();

				if (rlc.GetEnabled())
				{
					if (TreeEx(
							"ltre",
							true,
							"%s",
							UIL::LS(CommonStrings::Lights)))
					{
						ImGui::Indent();
						ImGui::Spacing();

						if (settings.mark_if(ImGui::Checkbox(
								UIL::LS(UISettingsStrings::NPCLightUpdates, "1"),
								std::addressof(data.lightEnableNPCUpdates))))
						{
							rlc.SetNPCLightUpdatesEnabled(data.lightEnableNPCUpdates);
						}

						ImGui::Spacing();
						ImGui::Unindent();

						ImGui::TreePop();
					}
				}

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		void UISettings::DrawUISection()
		{
			if (CollapsingHeader(
					"tree_ui",
					true,
					"%s",
					UIL::LS(CommonStrings::UI)))
			{
				ImGui::PushID("5");

				ImGui::Spacing();
				ImGui::Indent();

				auto& settings = m_controller.GetSettings();
				auto& ui       = settings.data.ui;

				if (settings.mark_if(UIStylePresetSelectorWidget::DrawStylePresetSelector(ui.stylePreset)))
				{
					Drivers::UI::SetStyle(ui.stylePreset);
				}

				if (settings.mark_if(ImGui::SliderFloat(
						UIL::LS(UISettingsStrings::Alpha, "1"),
						std::addressof(ui.alpha),
						0.15f,
						1.0f,
						"%.2f")))
				{
					Drivers::UI::SetAlpha(ui.alpha);
				}

				DrawCommonResetContextMenu(
					"ctx_rst_bg",
					static_cast<Localization::StringID>(CommonStrings::Reset),
					ui.bgAlpha.has(),
					[&] {
						ui.bgAlpha.clear();
						Drivers::UI::SetBGAlpha(ui.bgAlpha);
					});

				const bool hasAlpha = ui.bgAlpha.has();

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
						UIL::LS(UISettingsStrings::BGAlpha, "2"),
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
						UIL::LS(UISettingsStrings::ShowIntroBanner, "3"),
						std::addressof(ui.showIntroBanner))))
				{
					if (!ui.showIntroBanner)
					{
						Drivers::UI::QueueRemoveTask(-0xFFFF);
					}
				}

				settings.mark_if(ImGui::Checkbox(
					UIL::LS(UISettingsStrings::CloseOnEsc, "4"),
					std::addressof(ui.closeOnESC)));

				UITipsInterface::DrawTip(UITip::CloseOnESC);

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::ControlLock, "6"),
						std::addressof(ui.enableControlLock))))
				{
					m_owner.SetControlLock(ui.enableControlLock);
					Drivers::UI::EvaluateTaskState();
				}
				UITipsInterface::DrawTip(UITip::ControlLock);

				settings.mark_if(ImGui::Checkbox(
					UIL::LS(UISettingsStrings::ExitOnLastWindowClose, "7"),
					std::addressof(ui.exitOnLastWindowClose)));

				ImGui::NextColumn();

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::FreezeTime, "M"),
						std::addressof(ui.enableFreezeTime))))
				{
					m_owner.SetFreezeTime(ui.enableFreezeTime);
					Drivers::UI::EvaluateTaskState();
				}
				UITipsInterface::DrawTip(UITip::FreezeTime);

				settings.mark_if(ImGui::Checkbox(
					UIL::LS(UISettingsStrings::SelectCrosshairActor, "N"),
					std::addressof(ui.selectCrosshairActor)));

				UITipsInterface::DrawTip(UITip::SelectCrosshairActor);

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::EnableRestrictions, "O"),
						std::addressof(ui.enableRestrictions))))
				{
					m_owner.EnableRestrictions(ui.enableRestrictions);
				}
				UITipsInterface::DrawTip(UITip::EnableRestrictions);

				ImGui::Columns();

				ImGui::Spacing();

				auto tmp = m_scaleTemp ?
				               *m_scaleTemp :
				               ui.scale;

				if (ImGui::SliderFloat(
						UIL::LS(CommonStrings::Scale, "P"),
						std::addressof(tmp),
						0.2f,
						5.0f,
						"%.2f",
						ImGuiSliderFlags_AlwaysClamp))
				{
					m_scaleTemp = tmp;
				}
				UITipsInterface::DrawTip(UITip::UIScale);

				if (m_scaleTemp)
				{
					if (ImGui::Button(UIL::LS(CommonStrings::Apply, "R")))
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
						UIL::LS(CommonStrings::Fonts)))
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
						UIL::LS(UISettingsStrings::InterfaceOpenKeys)))
				{
					ImGui::Indent();
					ImGui::Spacing();

					if (m_controller.UIGetIniKeysForced())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextWrapped("%s", UIL::LS(UISettingsStrings::UIKeyOverrideWarning));
						ImGui::PopStyleColor();
						ImGui::Spacing();
					}

					auto tmpk = m_controller.GetInputHandlers().uiOpen.GetComboKey();

					if (settings.mark_if(UIControlKeySelectorWidget::DrawKeySelector(
							"1",
							static_cast<Localization::StringID>(CommonStrings::ComboKey),
							UIData::g_comboControlMap,
							tmpk,
							true)))
					{
						m_controller.GetInputHandlers().uiOpen.SetComboKey(
							tmpk);

						ui.openKeys->comboKey = tmpk;
						ui.openKeys->key      = m_controller.GetInputHandlers().uiOpen.GetKey();
						ui.openKeys.mark(true);
					}

					tmpk = m_controller.GetInputHandlers().uiOpen.GetKey();

					if (settings.mark_if(UIControlKeySelectorWidget::DrawKeySelector(
							"2",
							static_cast<Localization::StringID>(CommonStrings::Key),
							UIData::g_controlMap,
							tmpk)))
					{
						m_controller.GetInputHandlers().uiOpen.SetKey(tmpk);

						ui.openKeys->key      = tmpk;
						ui.openKeys->comboKey = m_controller.GetInputHandlers().uiOpen.GetComboKey();
						ui.openKeys.mark(true);
					}

					ImGui::Spacing();
					ImGui::Unindent();

					ImGui::TreePop();
				}

				if (TreeEx(
						"rel_keys",
						false,
						"%s",
						UIL::LS(UISettingsStrings::ReleaseControlLockKeys)))
				{
					auto& task = m_owner.As<IUIRenderTaskMain>();

					ImGui::Indent();
					ImGui::Spacing();

					auto& context = task.GetContext();

					auto tmpk = context.ILRHGetComboKey();

					if (settings.mark_if(UIControlKeySelectorWidget::DrawKeySelector(
							"1",
							static_cast<Localization::StringID>(CommonStrings::ComboKey),
							UIData::g_comboControlMap,
							tmpk,
							true)))
					{
						context.ILRHSetKeys(context.ILRHGetKey(), tmpk);

						ui.releaseLockKeys->comboKey = tmpk;
						ui.releaseLockKeys->key      = context.ILRHGetKey();
						ui.releaseLockKeys.mark(true);
					}

					tmpk = context.ILRHGetKey();

					if (settings.mark_if(UIControlKeySelectorWidget::DrawKeySelector(
							"2",
							static_cast<Localization::StringID>(CommonStrings::Key),
							UIData::g_controlMap,
							tmpk)))
					{
						context.ILRHSetKeys(tmpk, context.ILRHGetComboKey());

						ui.releaseLockKeys->key      = tmpk;
						ui.releaseLockKeys->comboKey = context.ILRHGetComboKey();
						ui.releaseLockKeys.mark(true);
					}

					if (settings.mark_if(ImGui::SliderFloat(
							UIL::LS(UISettingsStrings::Alpha, "3"),
							std::addressof(ui.releaseLockAlpha),
							0.0f,
							1.0f,
							"%.2f")))
					{
						context.ILRHSetLockedAlpha(ui.releaseLockAlpha);
					}

					if (settings.mark_if(ImGui::Checkbox(
							UIL::LS(UISettingsStrings::UnfreezeTime, "4"),
							std::addressof(ui.releaseLockUnfreezeTime))))
					{
						context.ILRHSetUnfreezeTime(ui.releaseLockUnfreezeTime);
					}

					ImGui::Spacing();
					ImGui::Unindent();

					ImGui::TreePop();
				}

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
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
						UIL::LS(CommonStrings::Add, "1"),
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
			auto& settings = m_controller.GetSettings();

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
					UIL::LS(CommonStrings::Sound)))
			{
				ImGui::PushID("6");

				ImGui::Spacing();
				ImGui::Indent();

				if (settings.mark_if(ImGui::Checkbox(
						UIL::LS(UISettingsStrings::EnableEquipSounds, "1"),
						std::addressof(settings.data.sound.enabled))))
				{
					m_controller.SetPlaySound(settings.data.sound.enabled);
				}

				if (settings.data.sound.enabled)
				{
					ImGui::SameLine();

					if (settings.mark_if(ImGui::Checkbox(
							UIL::LS(UISettingsStrings::NPCSounds, "2"),
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

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		bool UISettings::DrawLogLevelSelector(
			const char*            a_id,
			Localization::StringID a_title,
			LogLevel&              a_value)
		{
			bool result = false;

			auto desc = ILog::GetLogLevelString(a_value);

			if (ImGui::BeginCombo(
					UIL::LS(a_title, a_id),
					desc))
			{
				for (auto& [i, e] : ILog::GetLogLevels())
				{
					const bool selected = e == a_value;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(i.c_str(), selected))
					{
						a_value = e;
						result  = true;
					}
				}

				ImGui::EndCombo();
			}

			return result;
		}

		void UISettings::DrawObjectDatabaseSection()
		{
			if (CollapsingHeader(
					"tree_mdb",
					true,
					"%s",
					UIL::LS(UIWidgetCommonStrings::ModelDatabase)))
			{
				auto& settings = m_controller.GetSettings();

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
					preview = UIL::LS(it->second);
				}
				else
				{
					stl::snprintf(buf, "%u", stl::underlying(current));
					preview = buf;
				}

				ImGui::PushID("7");

				ImGui::Spacing();
				ImGui::Indent();

				if (ImGui::BeginCombo(
						UIL::LS(CommonStrings::Caching, "1"),
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

						if (ImGui::Selectable(UIL::LS<CommonStrings, 3>(e.second, "1"), selected))
						{
							settings.data.odbLevel = e.first;
							settings.mark_dirty();
							m_controller.SetObjectDatabaseLevel(e.first);
						}

						ImGui::PopID();
					}

					ImGui::EndCombo();
				}

				UITipsInterface::DrawTip(UITip::ModelCache);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(UIL::LS(UISettingsStrings::CachedModelsColon));
				ImGui::SameLine();
				ImGui::Text("%zu", m_controller.GetODBObjectCount());
				ImGui::SameLine();

				if (ImGui::Button(UIL::LS(CommonStrings::Clear, "2")))
				{
					m_controller.QueueObjectDatabaseClear();
				}

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::PopID();
			}
		}

		void UISettings::DrawLocalizationSection()
		{
			if (CollapsingHeader(
					"tree_localization",
					true,
					"%s",
					UIL::LS(CommonStrings::Localization)))
			{
				ImGui::PushID("8");

				ImGui::Indent();
				ImGui::Spacing();

				auto& settings = m_controller.GetSettings();
				auto& ldm      = Localization::LocalizationDataManager::GetSingleton();
				auto& current  = m_controller.GetCurrentLanguageTable();

				if (ImGui::BeginCombo(
						UIL::LS(CommonStrings::Language, "1"),
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

						if (ImGui::Selectable(UIL::LMKID<3>(e.first.c_str(), "1"), selected))
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

				ImGui::PopID();
			}
		}

		void UISettings::DrawI3DISection()
		{
			if (CollapsingHeader(
					"tree_i3di",
					false,
					"%s",
					UIL::LS(UISettingsStrings::I3DI)))
			{
				ImGui::PushID("9");

				ImGui::Indent();
				ImGui::Spacing();

				auto& settings = m_controller.GetSettings();
				auto& data     = settings.data.ui.i3di;

				settings.mark_if(ImGui::Checkbox(
					UIL::LS(UISettingsStrings::EnableWeapons, "1"),
					std::addressof(data.enableWeapons)));

				ImGui::Spacing();
				ImGui::Unindent();

				ImGui::PopID();
			}
		}

		void UISettings::DrawFontSelector()
		{
			ImGui::PushID("font_selector");

			auto& settings = m_controller.GetSettings();

			auto current = Drivers::UI::GetCurrentFont();

			DrawCommonResetContextMenu(
				"ctx_font_rld",
				static_cast<Localization::StringID>(CommonStrings::Reload),
				true,
				[&] {
					Drivers::UI::MarkFontUpdateDataDirty();
				});

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Font, "1"),
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

					if (ImGui::Selectable(UIL::LMKID<3>(e.c_str(), "1"), selected))
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
				static_cast<Localization::StringID>(CommonStrings::Clear),
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
					UIL::LS(CommonStrings::Size, "3"),
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
				if (ImGui::Button(UIL::LS(CommonStrings::Apply, "4")))
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
					UIL::LS(UISettingsStrings::ExtraGlyphs)))
			{
				ImGui::Indent();
				ImGui::Spacing();

				auto& settings = m_controller.GetSettings();

				decltype(s_extraGlyphs)::size_type i = 0;

				auto mid = s_extraGlyphs.size() / 2;
				bool sw  = false;

				ImGui::Columns(2, nullptr, false);

				for (auto& e : s_extraGlyphs)
				{
					ImGui::PushID(stl::underlying(e.first));

					if (settings.mark_if(ImGui::CheckboxFlagsT(
							e.second,
							stl::underlying(std::addressof(settings.data.ui.extraGlyphs.value)),
							stl::underlying(e.first))))
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
			auto& settings = m_controller.GetSettings();
			auto& ui       = settings.data.ui;

			ImGui::PushID("font_misc_opt");

			if (settings.mark_if(ImGui::Checkbox(
					UIL::LS(UISettingsStrings::ReleaseFontData, "1"),
					std::addressof(ui.releaseFontData))))
			{
				Drivers::UI::SetReleaseFontData(ui.releaseFontData);
			}

			UITipsInterface::DrawTipImportant(UITip::ReleaseFontData);

			ImGui::PopID();
		}

		bool UISettings::DrawSoundPairs()
		{
			auto& settings = m_controller.GetSettings();

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
				if (UIL::LCG_MI(CommonStrings::Delete, "1"))
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

				auto tmp = a_soundPair.first ?
				               *a_soundPair.first :
				               Game::FormID{};

				if (DrawFormPicker(
						"1",
						static_cast<Localization::StringID>(CommonStrings::Equip),
						tmp))
				{
					result = true;

					if (tmp)
					{
						a_soundPair.first.emplace(tmp);
					}
					else
					{
						a_soundPair.first.reset();
					}
				}

				tmp = a_soundPair.second ?
				          *a_soundPair.second :
				          Game::FormID{};

				if (DrawFormPicker(
						"2",
						static_cast<Localization::StringID>(CommonStrings::Unequip),
						tmp))
				{
					result = true;

					if (tmp)
					{
						a_soundPair.second.emplace(tmp);
					}
					else
					{
						a_soundPair.second.reset();
					}
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
			return m_controller.GetSettings().data.ui.settingsColStates;
		}

		void UISettings::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UISettings::DrawMenuBar()
		{
			ImGui::PushID("menu_bar");

			if (ImGui::BeginMenuBar())
			{
				if (UIL::LCG_BM(CommonStrings::File, "1"))
				{
					DrawFileMenu();
					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(CommonStrings::Tools, "2"))
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
			if (UIL::LCG_MI(CommonStrings::Save, "1"))
			{
				m_controller.SaveSettings(true, false, true);
			}

			ImGui::Separator();

			if (UIL::LCG_MI(CommonStrings::Close, "2"))
			{
				SetOpenState(false);
			}
		}

		void UISettings::DrawToolsMenu()
		{
			if (UIL::LCG_BM(CommonStrings::Maintenance, "1"))
			{
				DrawMaintenanceMenu();

				ImGui::EndMenu();
			}
		}

		void UISettings::DrawMaintenanceMenu()
		{
			auto& settings = m_controller.GetSettings();

			if (UIL::LCG_MI(UISettingsStrings::ClearStoredColStates, "1"))
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
				settings.data.ui.skeletonExplorer.colStates.clear();
				settings.data.ui.actorInfo.colStates.clear();
				settings.data.ui.condVarEditor.colStates.clear();
				settings.data.ui.condVarProfileEditor.colStates.clear();

				settings.mark_dirty();
			}

			if (UIL::LCG_MI(UISettingsStrings::ClearBipedCache, "2"))
			{
				m_controller.ClearBipedCache();
				m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
			}

			if (UIL::LCG_MI(UISettingsStrings::RemoveBipedCacheTempForms, "3"))
			{
				const auto result = m_controller.RemoveBipedCacheEntries([](Game::FormID a_id) {
					return a_id.IsTemporary();
				});

				if (result)
				{
					m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
				}

				m_controller.QueueToastAsync(
					"%zu temporary reference(s) were removed",
					UICommon::g_colorWarning,
					result);
			}

			if (UIL::LCG_MI(UISettingsStrings::RemoveBipedCacheDeletedTempForms, "4"))
			{
				ITaskPool::AddPriorityTask([&ctrl = m_controller]() {
					const stl::lock_guard lock(ctrl.GetLock());

					const auto result = ctrl.RemoveBipedCacheEntries([](Game::FormID a_id) {
						if (!a_id.IsTemporary())
						{
							return false;
						}

						const auto actor = a_id.As<Actor>();
						return actor ? actor->IsDeleted() : false;
					});

					if (result)
					{
						ctrl.QueueResetAll(ControllerUpdateFlags::kNone);
					}

					ctrl.QueueToast(
						"%zu deleted temporary reference(s) were removed",
						UICommon::g_colorWarning,
						result);
				});
			}
		}

		void UISettings::DrawCommonResetContextMenu(
			const char*            a_imid,
			Localization::StringID a_strid,
			bool                   a_enabled,
			std::function<void()>  a_func)
		{
			ImGui::PushID(a_imid);

			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			//ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						UIL::LS(a_strid, "1"),
						nullptr,
						false,
						a_enabled))
				{
					a_func();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

	}
}