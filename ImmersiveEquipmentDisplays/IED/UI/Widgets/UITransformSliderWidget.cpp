#include "pch.h"

#include "UITransformSliderWidget.h"

namespace IED
{
	namespace UI
	{
		TransformSliderContextAction UITransformSliderWidget::DrawTransformHeaderContextMenu(
			Data::configTransform_t& a_data,
			func_upd_t               a_updateop)
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
				if (ImGui::MenuItem(UIL::LS(CommonStrings::Clear, "1")))
				{
					a_data.clear();
					a_updateop(TransformUpdateValue::All);
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configTransform_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "3"),
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

		void UITransformSliderWidget::DrawTransformTree(
			Data::configTransform_t& a_data,
			const bool               a_indent,
			func_upd_t               a_updateop,
			func_ext_t               a_extra,
			Localization::StringID   a_label,
			ImGuiTreeNodeFlags       a_flags)
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
					a_flags |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(a_label)))
			{
				ImGui::PushID("extra");
				a_extra();
				ImGui::PopID();

				ImGui::Spacing();

				if (a_indent)
				{
					ImGui::Indent();
				}

				DrawTransformFlags(a_data, a_updateop);
				ImGui::Spacing();
				DrawTransformSliders(a_data, a_updateop);

				if (a_indent)
				{
					ImGui::Unindent();
				}

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void UITransformSliderWidget::DrawTransformFlags(
			Data::configTransform_t& a_data,
			func_upd_t               a_updateop)
		{
			ImGui::PushID("flags");

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UITransformSliderWidgetStrings::FixedFrameRotation, "0"),
					stl::underlying(std::addressof(a_data.xfrmFlags.value)),
					stl::underlying(Data::ConfigTransformFlags::kExtrinsicRotation)))
			{
				a_data.update_rotation_matrix();
				a_data.update_tag();

				a_updateop(TransformUpdateValue::Rotation);
			}

			ImGui::PopID();
		}

		void UITransformSliderWidget::DrawTransformSliders(
			Data::configTransform_t& a_data,
			func_upd_t               a_updateop)
		{
			ImGui::PushID("sliders");

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.005f : 0.5f;

			ImGui::PushID("vpos");
			ImGui::BeginGroup();

			if (DrawTransformSliderContextMenu() == ContextMenuResult::kReset)
			{
				if (a_data.position)
				{
					a_data.position.reset();
					a_data.update_tag();
					a_updateop(TransformUpdateValue::Position);
				}
			}

			bool tmpb = a_data.position.has();

			if (!tmpb)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::DragFloat3(
					UIL::LS(CommonStrings::Position, "1"),
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

			if (DrawTransformSliderContextMenu() == ContextMenuResult::kReset)
			{
				if (a_data.rotation || a_data.rotationMatrix)
				{
					a_data.rotation.reset();
					a_data.rotationMatrix.reset();
					a_data.update_tag();
					a_updateop(TransformUpdateValue::Rotation);
				}
			}

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
					UIL::LS(CommonStrings::Rotation, "2"),
					v,
					dragSpeed,
					-degmax,
					degmax,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp |
						ImGuiSliderFlags_NoRoundToFormat))
			{
				a_data.rotation = v * (pi / 180.0f);

				a_data.update_rotation_matrix();
				a_data.update_tag();

				a_updateop(TransformUpdateValue::Rotation);
			}
			UITipsInterface::DrawTip(UITip::Rotation);

			if (!tmpb)
			{
				ImGui::PopStyleVar();
			}

			ImGui::EndGroup();
			ImGui::PopID();

			ImGui::PushID("vscale");
			ImGui::BeginGroup();

			if (DrawTransformSliderContextMenu() == ContextMenuResult::kReset)
			{
				if (a_data.scale)
				{
					a_data.scale.clear();
					*a_data.scale = 1.0f;
					a_data.update_tag();
					a_updateop(TransformUpdateValue::Rotation);
				}
			}

			tmpb = a_data.scale.has();

			if (!tmpb)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_Alpha,
					ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::DragFloat(
					UIL::LS(CommonStrings::Scale, "3"),
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

		auto UITransformSliderWidget::DrawTransformSliderContextMenu()
			-> ContextMenuResult
		{
			ContextMenuResult result{ ContextMenuResult::kNone };

			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem("Reset"))
				{
					result = ContextMenuResult::kReset;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

	}
}