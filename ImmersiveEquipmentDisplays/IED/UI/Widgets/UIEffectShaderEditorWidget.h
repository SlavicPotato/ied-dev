#pragma once

#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UITips.h"

#include "UIDescriptionPopup.h"
#include "UIEffectShaderAlphaFunctionSelector.h"
#include "UIEffectShaderDepthModeSelector.h"
#include "UIEffectShaderFunctionEditorWidget.h"
#include "UIPopupToggleButtonWidget.h"
#include "UITextureClampModeSelector.h"

#include "IED/ConfigEffectShader.h"

#include "UIEffectShaderEditorWidgetStrings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class EffectShaderContextAction
		{
			None,
			Delete,
			PasteOver,
			Add,
			Swap,
			Paste
		};

		struct EffectShaderFunctionContextResult
		{
			EffectShaderContextAction      action{ EffectShaderContextAction::None };
			Data::EffectShaderFunctionType functionType;
			SwapDirection                  dir;
		};

		struct EffectShaderEntryResult
		{
			EffectShaderContextAction action{ EffectShaderContextAction::None };
		};

		template <class T>
		class UIEffectShaderEditorWidget :
			UIEffectShaderFunctionEditorWidget,
			UITextureClampModeSelector,
			UIEffectShaderDepthModeSelector,
			UIEffectShaderAlphaFunctionSelector,
			public virtual UIDescriptionPopupWidget,
			public virtual UITipsInterface,
			public virtual UILocalizationInterface
		{
		public:
			UIEffectShaderEditorWidget(Controller& a_controller);

			void DrawEffectShaderEditor(
				const T&                          a_params,
				Data::configEffectShaderHolder_t& a_data);

		private:
			EffectShaderContextAction DrawEffectShaderEntryContextMenu(
				const T&                        a_params,
				Data::configEffectShaderData_t& a_data);

			void DrawEffectShaderEntries(
				const T&                          a_params,
				Data::configEffectShaderHolder_t& a_data);

			void DrawEffectShaderEntry(
				const T&                        a_params,
				Data::configEffectShaderData_t& a_data);

			void DrawTextureTree(
				const char*                        a_id,
				const char*                        a_title,
				const T&                           a_params,
				Data::configEffectShaderTexture_t& a_data,
				const bool                         a_mustBeSet);

			void DrawEffectShaderFlags(
				const T&                        a_params,
				Data::configEffectShaderData_t& a_data);

			EffectShaderContextAction DrawTargetNodesContextMenu(
				const T&                      a_params,
				Data::configFixedStringSet_t& a_data);

			void DrawTargetNodes(
				const T&                      a_params,
				Data::configFixedStringSet_t& a_data);

			void DrawTargetNodeTable(
				const T&                      a_params,
				Data::configFixedStringSet_t& a_data);

			EffectShaderContextAction DrawFunctionsContextMenu(
				const T&                                a_params,
				Data::configEffectShaderFunctionList_t& a_data);

			void DrawFunctions(
				const T&                                a_params,
				Data::configEffectShaderFunctionList_t& a_data);

			EffectShaderFunctionContextResult DrawFunctionTableEntryContextMenu(
				const T&                            a_params,
				Data::configEffectShaderFunction_t& a_data);

			void DrawFunctionTable(
				const T&                                a_params,
				Data::configEffectShaderFunctionList_t& a_data);

			virtual void OnEffectShaderUpdate(const T& a_params, bool a_updateTag = true) = 0;

			Data::EffectShaderFunctionType m_newType{ Data::EffectShaderFunctionType::None };

			char m_inputBuffer1[MAX_PATH]{ 0 };
		};

		template <class T>
		UIEffectShaderEditorWidget<T>::UIEffectShaderEditorWidget(
			Controller& a_controller) :
			UITextureClampModeSelector(a_controller),
			UIEffectShaderDepthModeSelector(a_controller),
			UIEffectShaderAlphaFunctionSelector(a_controller),
			UIEffectShaderFunctionEditorWidget(a_controller)
		{
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawEffectShaderEditor(
			const T&                          a_params,
			Data::configEffectShaderHolder_t& a_data)
		{
			ImGui::PushID("es_editor");

			ImGui::PushID("header");

			if (ImGui::CheckboxFlagsT(
					LS(CommonStrings::Enabled, "1"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderHolderFlags::kEnabled)))
			{
				OnEffectShaderUpdate(a_params);
			}

			ImGui::PopID();

			ImGui::Spacing();

			bool disabled = !a_data.enabled();

			UICommon::PushDisabled(disabled);

			DrawEffectShaderEntries(a_params, a_data);

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		EffectShaderContextAction UIEffectShaderEditorWidget<T>::DrawEffectShaderEntryContextMenu(
			const T&                        a_params,
			Data::configEffectShaderData_t& a_data)
		{
			EffectShaderContextAction result{ EffectShaderContextAction::None };

			ImGui::PushID("tn_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(LS(CommonStrings::Delete, "1")))
				{
					result = EffectShaderContextAction::Delete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configEffectShaderData_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						OnEffectShaderUpdate(a_params);

						result = EffectShaderContextAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawEffectShaderEntries(
			const T&                          a_params,
			Data::configEffectShaderHolder_t& a_data)
		{
			if (a_data.data.empty())
			{
				return;
			}

			ImGui::PushID("es_entries");

			auto it = a_data.data.begin();

			while (it != a_data.data.end())
			{
				ImGui::PushID(it->first.c_str());

				const auto result = DrawEffectShaderEntryContextMenu(
					a_params,
					it->second);

				switch (result)
				{
				case EffectShaderContextAction::Delete:

					it = a_data.data.erase(it);
					OnEffectShaderUpdate(a_params);
					break;

				case EffectShaderContextAction::PasteOver:

					OnEffectShaderUpdate(a_params);
					ImGui::SetNextItemOpen(true);
					break;
				}

				if (it != a_data.data.end())
				{
					if (ImGui::TreeNodeEx(
							"entry",
							ImGuiTreeNodeFlags_DefaultOpen |
								ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							it->first.c_str()))
					{
						ImGui::Spacing();

						DrawEffectShaderEntry(a_params, it->second);

						ImGui::Spacing();

						ImGui::TreePop();
					}

					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawEffectShaderEntry(
			const T&                        a_params,
			Data::configEffectShaderData_t& a_data)
		{
			DrawTextureTree(
				"1",
				LS(UIEffectShaderEditorWidgetStrings::BaseTexture),
				a_params,
				a_data.baseTexture,
				true);

			DrawTextureTree(
				"2",
				LS(UIEffectShaderEditorWidgetStrings::PalleteTexture),
				a_params,
				a_data.paletteTexture,
				false);

			DrawTextureTree(
				"3",
				LS(UIEffectShaderEditorWidgetStrings::BlockOutTexture),
				a_params,
				a_data.blockOutTexture,
				false);

			ImGui::Spacing();

			if (DrawTextureClampModeSelector(
					static_cast<Localization::StringID>(
						UIEffectShaderEditorWidgetStrings::TextureClampMode),
					a_data.textureClampMode))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (DrawAlphaFuncSelector(
					static_cast<Localization::StringID>(
						UIEffectShaderEditorWidgetStrings::SrcBlend),
					a_data.srcBlend))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (DrawAlphaFuncSelector(
					static_cast<Localization::StringID>(
						UIEffectShaderEditorWidgetStrings::DstBlend),
					a_data.destBlend))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (DrawDepthModeSelector(
					static_cast<Localization::StringID>(
						UIEffectShaderEditorWidgetStrings::zTestFunc),
					a_data.zTestFunc))
			{
				OnEffectShaderUpdate(a_params);
			}

			ImGui::Spacing();

			if (ImGui::ColorEdit4(
					LS(UIEffectShaderEditorWidgetStrings::FillColor, "4"),
					a_data.fillColor,
					ImGuiColorEditFlags_AlphaBar))
			{
				a_data.fillColor.clamp();

				OnEffectShaderUpdate(a_params, false);
			}

			if (ImGui::ColorEdit4(
					LS(UIEffectShaderEditorWidgetStrings::RimColor, "5"),
					a_data.rimColor,
					ImGuiColorEditFlags_AlphaBar))
			{
				a_data.rimColor.clamp();

				OnEffectShaderUpdate(a_params, false);
			}

			if (ImGui::DragFloat(
					LS(UIEffectShaderEditorWidgetStrings::EdgeExponent, "6"),
					std::addressof(a_data.edgeExponent),
					0.01f,
					0.0f,
					25.0f,
					"%.3f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				OnEffectShaderUpdate(a_params, false);
			}

			/*if (ImGui::DragFloat(
					LS(UIEffectShaderEditorWidgetStrings::BoundDiameter, "7"),
					std::addressof(a_data.boundDiameter),
					0.01f,
					0.0f,
					125.0f,
					"%.3f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				OnEffectShaderUpdate(a_params, false);
			}*/

			ImGui::Spacing();

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.00005f : 0.005f;

			if (ImGui::DragFloat2(
					LS(UIEffectShaderEditorWidgetStrings::uvOffset, "8"),
					a_data.uvOffset,
					dragSpeed,
					-25.0f,
					25.0f,
					"%.3f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				OnEffectShaderUpdate(a_params, false);
			}

			dragSpeed = ImGui::GetIO().KeyShift ? 0.0001f : 0.01f;

			if (ImGui::DragFloat2(
					LS(UIEffectShaderEditorWidgetStrings::uvScale, "9"),
					a_data.uvScale,
					dragSpeed,
					0.0f,
					1000.0f,
					"%.3f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				OnEffectShaderUpdate(a_params, false);
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			DrawEffectShaderFlags(a_params, a_data);

			ImGui::Spacing();

			DrawTargetNodes(a_params, a_data.targetNodes);
			DrawFunctions(a_params, a_data.functions);
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawTextureTree(
			const char*                        a_id,
			const char*                        a_title,
			const T&                           a_params,
			Data::configEffectShaderTexture_t& a_data,
			const bool                         a_mustBeSet)
		{
			if (ImGui::TreeNodeEx(
					a_id,
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					a_title))
			{
				ImGui::Spacing();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s:", LS(CommonStrings::Texture));
				ImGui::SameLine();

				UICommon::PushDisabled(a_mustBeSet);

				if (ImGui::RadioButton(
						LS(CommonStrings::None, "0"),
						a_data.fbf.selected == Data::EffectShaderSelectedTexture::None))
				{
					a_data.fbf.selected = Data::EffectShaderSelectedTexture::None;
					OnEffectShaderUpdate(a_params);
				}

				UICommon::PopDisabled(a_mustBeSet);

				ImGui::SameLine();

				if (ImGui::RadioButton(
						LS(CommonStrings::Black, "1"),
						a_data.fbf.selected == Data::EffectShaderSelectedTexture::Black))
				{
					a_data.fbf.selected = Data::EffectShaderSelectedTexture::Black;
					OnEffectShaderUpdate(a_params);
				}

				ImGui::SameLine();

				if (ImGui::RadioButton(
						LS(CommonStrings::White, "2"),
						a_data.fbf.selected == Data::EffectShaderSelectedTexture::White))
				{
					a_data.fbf.selected = Data::EffectShaderSelectedTexture::White;
					OnEffectShaderUpdate(a_params);
				}

				ImGui::SameLine();

				if (ImGui::RadioButton(
						LS(CommonStrings::Grey, "3"),
						a_data.fbf.selected == Data::EffectShaderSelectedTexture::Grey))
				{
					a_data.fbf.selected = Data::EffectShaderSelectedTexture::Grey;
					OnEffectShaderUpdate(a_params);
				}

				ImGui::SameLine();

				if (ImGui::RadioButton(
						LS(CommonStrings::Custom, "4"),
						a_data.fbf.selected == Data::EffectShaderSelectedTexture::Custom))
				{
					a_data.fbf.selected = Data::EffectShaderSelectedTexture::Custom;
					OnEffectShaderUpdate(a_params);
				}

				ImGui::Spacing();

				if (a_data.fbf.selected == Data::EffectShaderSelectedTexture::Custom)
				{
					auto l = a_data.path.get().copy(
						m_inputBuffer1,
						sizeof(m_inputBuffer1) - 1);

					m_inputBuffer1[l] = 0;

					if (ImGui::InputText(
							LS(CommonStrings::Texture, "5"),
							m_inputBuffer1,
							sizeof(m_inputBuffer1),
							ImGuiInputTextFlags_EnterReturnsTrue))
					{
						a_data.path = m_inputBuffer1;
						OnEffectShaderUpdate(a_params);
					}
				}

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawEffectShaderFlags(
			const T&                        a_params,
			Data::configEffectShaderData_t& a_data)
		{
			ImGui::PushID("flags");

			ImGui::Columns(2, nullptr, false);

			if (ImGui::CheckboxFlagsT(
					LS(CommonStrings::Force, "1"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kForce)))
			{
				OnEffectShaderUpdate(a_params);
			}

			ImGui::Columns();

			ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);

			if (ImGui::CheckboxFlagsT(
					"kGrayscaleToColor",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kGrayscaleToColor)))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (ImGui::CheckboxFlagsT(
					"kGrayscaleToAlpha",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kGrayscaleToAlpha)))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (ImGui::CheckboxFlagsT(
					"kIgnoreTextureAlpha",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kIgnoreTextureAlpha)))
			{
				OnEffectShaderUpdate(a_params);
			}

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					"kBaseTextureProjectedUVs",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kBaseTextureProjectedUVs)))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (ImGui::CheckboxFlagsT(
					"kIgnoreBaseGeomTexAlpha",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kIgnoreBaseGeomTexAlpha)))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (ImGui::CheckboxFlagsT(
					"kLighting",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kLighting)))
			{
				OnEffectShaderUpdate(a_params);
			}

			if (ImGui::CheckboxFlagsT(
					"kAlpha",
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::EffectShaderDataFlags::kAlpha)))
			{
				OnEffectShaderUpdate(a_params);
			}

			ImGui::Columns();

			ImGui::PopID();
		}

		template <class T>
		EffectShaderContextAction UIEffectShaderEditorWidget<T>::DrawTargetNodesContextMenu(
			const T&                      a_params,
			Data::configFixedStringSet_t& a_data)
		{
			EffectShaderContextAction result{ EffectShaderContextAction::None };

			ImGui::PushID("tn_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::BeginMenu(LS(CommonStrings::Add, "1")))
				{
					if (DrawDescriptionPopup())
					{
						auto r = a_data.emplace(GetDescriptionPopupBuffer());
						if (r.second)
						{
							result = EffectShaderContextAction::Add;
						}
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFixedStringSet_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						result = EffectShaderContextAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawTargetNodes(
			const T&                      a_params,
			Data::configFixedStringSet_t& a_data)
		{
			ImGui::PushID("tn_area");

			const auto result = DrawTargetNodesContextMenu(
				a_params,
				a_data);

			switch (result)
			{
			case EffectShaderContextAction::Add:
			case EffectShaderContextAction::PasteOver:
				OnEffectShaderUpdate(a_params);
				ImGui::SetNextItemOpen(true);
				break;
			}

			bool disabled = a_data.empty();

			UICommon::PushDisabled(disabled);

			if (ImGui::TreeNodeEx(
					"tn_tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(UIEffectShaderEditorWidgetStrings::TargetNodes)))
			{
				if (!a_data.empty())
				{
					ImGui::Spacing();

					DrawTargetNodeTable(a_params, a_data);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawTargetNodeTable(
			const T&                      a_params,
			Data::configFixedStringSet_t& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			constexpr int NUM_COLUMNS = 2;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.f }))
			{
				ImGui::TableSetupColumn(
					"",
					ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed,
					ImGui::CalcTextSize("X", nullptr, true).x + (4.0f * 2.0f));

				ImGui::TableSetupColumn(LS(CommonStrings::Node), ImGuiTableColumnFlags_None, 75.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto it = a_data.begin();

				int i = 0;

				while (it != a_data.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

					if (ImGui::Button("X"))
					{
						it = a_data.erase(it);

						OnEffectShaderUpdate(a_params);
					}

					ImGui::PopStyleVar();

					if (it != a_data.end())
					{
						ImGui::TableSetColumnIndex(1);

						ImGui::Text("%s", it->c_str());

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		EffectShaderContextAction UIEffectShaderEditorWidget<T>::DrawFunctionsContextMenu(
			const T&                                a_params,
			Data::configEffectShaderFunctionList_t& a_data)
		{
			EffectShaderContextAction result{ EffectShaderContextAction::None };

			ImGui::PushID("fn_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newType = Data::EffectShaderFunctionType::None;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::BeginMenu(LS(CommonStrings::Add, "1")))
				{
					if (DrawEffectShaderFunctionSelector(m_newType))
					{
						if (m_newType != Data::EffectShaderFunctionType::None)
						{
							a_data.emplace_back(m_newType);
							result = EffectShaderContextAction::Add;

							ImGui::CloseCurrentPopup();
						}
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Sync, "2")))
				{
					OnEffectShaderUpdate(a_params);
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configEffectShaderFunctionList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						result = EffectShaderContextAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawFunctions(
			const T&                                a_params,
			Data::configEffectShaderFunctionList_t& a_data)
		{
			ImGui::PushID("fn_area");

			const auto result = DrawFunctionsContextMenu(
				a_params,
				a_data);

			switch (result)
			{
			case EffectShaderContextAction::Add:
			case EffectShaderContextAction::PasteOver:
				OnEffectShaderUpdate(a_params);
				ImGui::SetNextItemOpen(true);
				break;
			}

			bool disabled = a_data.empty();

			UICommon::PushDisabled(disabled);

			if (ImGui::TreeNodeEx(
					"fn_tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Functions)))
			{
				if (!a_data.empty())
				{
					ImGui::Spacing();

					DrawFunctionTable(a_params, a_data);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		EffectShaderFunctionContextResult UIEffectShaderEditorWidget<T>::DrawFunctionTableEntryContextMenu(
			const T&                            a_params,
			Data::configEffectShaderFunction_t& a_data)
		{
			EffectShaderFunctionContextResult result;

			ImGui::PushID("ft_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newType = Data::EffectShaderFunctionType::None;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = EffectShaderContextAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = EffectShaderContextAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::BeginMenu(LS(CommonStrings::Insert, "1")))
				{
					if (ImGui::BeginMenu(LS(CommonStrings::New, "1")))
					{
						if (DrawEffectShaderFunctionSelector(m_newType))
						{
							if (m_newType != Data::EffectShaderFunctionType::None)
							{
								result.action       = EffectShaderContextAction::Add;
								result.functionType = m_newType;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "2"),
							nullptr,
							false,
							UIClipboard::Get<Data::configEffectShaderFunction_t>() != nullptr))
					{
						result.action = EffectShaderContextAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "2")))
				{
					result.action = EffectShaderContextAction::Delete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "X")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configEffectShaderFunction_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "Y"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData && clipData->type == a_data.type)
					{
						a_data = *clipData;

						OnEffectShaderUpdate(a_params);

						result.action = EffectShaderContextAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIEffectShaderEditorWidget<T>::DrawFunctionTable(
			const T&                                a_params,
			Data::configEffectShaderFunctionList_t& a_data)
		{
			ImGui::PushStyleVar(
				ImGuiStyleVar_CellPadding,
				{ 5.f, 5.f });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"ef_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.f }))
			{
				auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(LS(CommonStrings::Name), ImGuiTableColumnFlags_None, 200.0f);
				ImGui::TableSetupColumn("LUID", ImGuiTableColumnFlags_None, 200.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				int i = 0;

				auto it = a_data.begin();

				while (it != a_data.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawFunctionTableEntryContextMenu(a_params, *it);

					switch (result.action)
					{
					case EffectShaderContextAction::Add:
						if (result.functionType != Data::EffectShaderFunctionType::None)
						{
							it = a_data.emplace(it, result.functionType);
							OnEffectShaderUpdate(a_params);
						}
						break;
					case EffectShaderContextAction::Paste:
						if (auto clipData = UIClipboard::Get<Data::configEffectShaderFunction_t>())
						{
							it = a_data.emplace(it, *clipData);
							OnEffectShaderUpdate(a_params);
						}
						break;
					case EffectShaderContextAction::Delete:
						it = a_data.erase(it);
						OnEffectShaderUpdate(a_params);
						break;
					case EffectShaderContextAction::Swap:
						if (IterSwap(a_data, it, result.dir))
						{
							OnEffectShaderUpdate(a_params);
						}
						break;
					}

					if (it != a_data.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (ImGui::Selectable(
								LMKID<3>(esf_to_desc(e.type), "sel_ctl"),
								false,
								ImGuiSelectableFlags_DontClosePopups))
						{
							OpenEffectShaderFunctionEditor();
						}

						auto r = DrawEffectShaderFunction(e);
						if (r)
						{
							OnEffectShaderUpdate(a_params, r.reset);
						}

						ImGui::TableSetColumnIndex(2);

						auto& luid = e.get_unique_id().get_tag_data();

						ImGui::Text("%llx.%llx", luid.p1, luid.p2);

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

	}
}
