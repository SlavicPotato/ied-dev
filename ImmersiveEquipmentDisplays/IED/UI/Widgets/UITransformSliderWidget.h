#pragma once

#include "../UICommon.h"

#include "IED/ConfigOverrideTransform.h"
#include "UIPopupToggleButtonWidget.h"

namespace IED
{
	namespace UI
	{
		class UITransformSliderWidget :
			virtual public UIPopupToggleButtonWidget
		{
		public:
			enum class TransformUpdateValue
			{
				Position,
				Rotation,
				Scale
			};

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
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
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
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
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
			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });

			ImGui::PushID("context_area");

			DrawPopupToggleButton("open", "context_menu");
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

			//ImGui::PopStyleVar();
		}

	}
}