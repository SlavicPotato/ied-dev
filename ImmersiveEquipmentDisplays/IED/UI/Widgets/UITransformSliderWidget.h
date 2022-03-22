#pragma once

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigTransform.h"
#include "UIPopupToggleButtonWidget.h"

namespace IED
{
	namespace UI
	{
		enum class TransformSliderContextAction
		{
			None,
			PasteOver
		};

		class UITransformSliderWidget :
			public virtual UILocalizationInterface
		{
		public:
			enum class TransformUpdateValue
			{
				Position,
				Rotation,
				Scale,
				All
			};

			UITransformSliderWidget(
				Localization::ILocalization& a_localization) :
				UILocalizationInterface(a_localization)
			{
			}

			template <class Tu>
			TransformSliderContextAction DrawTransformHeaderContextMenu(
				Data::configTransform_t& a_data,
				Tu                       a_updateop);

			template <class Tu, class Te>
			void DrawTransformTree(
				Data::configTransform_t& a_data,
				const bool               a_indent,
				Tu                       a_updateop,
				Te                       a_extra);

			template <class Tu>
			void DrawTransformSliders(
				Data::configTransform_t& a_data,
				Tu                       a_updateop);

			template <class Tv, class Tp, class Tu>
			void DrawTransformSliderContextMenu(
				Data::configTransform_t& a_data,
				stl::optional<Tv>&       a_value,
				Tp                       a_resetop,
				Tu                       a_updateop,
				TransformUpdateValue     a_updValue);
		};

		template <class Tu>
		TransformSliderContextAction UITransformSliderWidget::DrawTransformHeaderContextMenu(
			Data::configTransform_t& a_data,
			Tu                       a_updateop)
		{
			TransformSliderContextAction result{ TransformSliderContextAction::None };

			ImGui::PushID("xfrm_tree_ctx");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(LS(CommonStrings::Clear, "1")))
				{
					a_data.clear();
					a_updateop(TransformUpdateValue::All);
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configTransform_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						a_updateop(TransformUpdateValue::All);

						result = TransformSliderContextAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		template <class Tu, class Te>
		void UITransformSliderWidget::DrawTransformTree(
			Data::configTransform_t& a_data,
			const bool               a_indent,
			Tu                       a_updateop,
			Te                       a_extra)
		{
			ImGui::PushID("xfrm_tree");

			if (DrawTransformHeaderContextMenu(
					a_data,
					a_updateop) == TransformSliderContextAction::PasteOver)
			{
				ImGui::SetNextItemOpen(true);
			}

			if (ImGui::TreeNodeEx(
					"xfrm",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Transform)))
			{
				a_extra();

				if (a_indent)
				{
					ImGui::Indent();
				}

				DrawTransformSliders(
					a_data,
					a_updateop);

				if (a_indent)
				{
					ImGui::Unindent();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class Tu>
		void UITransformSliderWidget::DrawTransformSliders(
			Data::configTransform_t& a_data,
			Tu                       a_updateop)
		{
			ImGui::PushID("transform_sliders");

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.005f : 0.5f;

			ImGui::Spacing();

			ImGui::PushID("vpos");
			ImGui::BeginGroup();

			DrawTransformSliderContextMenu(
				a_data,
				a_data.position,
				[](auto& a_v) { a_v.reset(); },
				a_updateop,
				TransformUpdateValue::Position);

			bool tmpb = a_data.position.has();

			if (!tmpb)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::DragFloat3(
					"Position",
					*a_data.position,
					dragSpeed,
					-5000.0f,
					5000.0f,
					"%.4f",
					ImGuiSliderFlags_AlwaysClamp |
						ImGuiSliderFlags_NoRoundToFormat))
			{
				a_data.position.mark(true);
				a_data.update_tag();

				a_updateop(TransformUpdateValue::Position);
			}

			if (!tmpb)
			{
				ImGui::PopStyleVar();
			}

			ImGui::EndGroup();
			ImGui::PopID();

			ImGui::PushID("vrot");
			ImGui::BeginGroup();

			DrawTransformSliderContextMenu(
				a_data,
				a_data.rotation,
				[](auto& a_v) { a_v.reset(); },
				a_updateop,
				TransformUpdateValue::Rotation);

			tmpb = a_data.rotation.has();

			constexpr auto pi = std::numbers::pi_v<float>;

			NiPoint3 v;

			if (!tmpb)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_Alpha,
					ImGui::GetStyle().Alpha * 0.5f);
			}
			else
			{
				v = *a_data.rotation * (180.0f / pi);
			}

			constexpr auto degmax = (pi * 2.0f) * (180.0f / pi);

			if (ImGui::DragFloat3(
					"Rotation",
					v,
					dragSpeed,
					-degmax,
					degmax,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp |
						ImGuiSliderFlags_NoRoundToFormat))
			{
				a_data.rotation = v * (pi / 180.0f);
				a_data.update_tag();

				a_updateop(TransformUpdateValue::Rotation);
			}

			if (!tmpb)
			{
				ImGui::PopStyleVar();
			}

			ImGui::EndGroup();
			ImGui::PopID();

			ImGui::PushID("vscale");
			ImGui::BeginGroup();

			DrawTransformSliderContextMenu(
				a_data,
				a_data.scale,
				[](auto& a_v) { a_v.clear(); *a_v = 1.0f; },
				a_updateop,
				TransformUpdateValue::Scale);

			tmpb = a_data.scale.has();

			if (!tmpb)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_Alpha,
					ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::DragFloat(
					"Scale",
					std::addressof(*a_data.scale),
					dragSpeed * 0.01f,
					0.01f,
					100.0f,
					"%.4f",
					ImGuiSliderFlags_AlwaysClamp |
						ImGuiSliderFlags_NoRoundToFormat))
			{
				a_data.scale.mark(true);
				a_data.update_tag();

				a_updateop(TransformUpdateValue::Scale);
			}

			if (!tmpb)
			{
				ImGui::PopStyleVar();
			}

			ImGui::EndGroup();
			ImGui::PopID();

			ImGui::PopID();
		}

		template <class Tv, class Tp, class Tu>
		void UITransformSliderWidget::DrawTransformSliderContextMenu(
			Data::configTransform_t& a_data,
			stl::optional<Tv>&       a_value,
			Tp                       a_resetop,
			Tu                       a_updateop,
			TransformUpdateValue     a_updValue)
		{
			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem("Reset"))
				{
					if (a_value)
					{
						a_data.update_tag();
						a_resetop(a_value);
						a_updateop(a_updValue);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

	}
}