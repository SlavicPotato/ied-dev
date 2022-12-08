#include "pch.h"

#include "UIPhysicsValueEditorWidget.h"

#include "IED/UI/UICommon.h"

#include "IED/ConfigNodePhysicsValues.h"

namespace IED
{
	namespace UI
	{
		bool UIPhysicsValueEditorWidget::DrawPhysicsValues(
			Data::configNodePhysicsValues_t& a_data)
		{
			const bool result = DrawPhysicsValuesImpl(a_data);

			if (result)
			{
				a_data.update_tag();
			}

			return result;
		}

		bool UIPhysicsValueEditorWidget::DrawPhysicsValuesImpl(
			Data::configNodePhysicsValues_t& a_data)
		{
			bool result = false;

			ImGui::PushID("pv");

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Disable, "0"),
				stl::underlying(std::addressof(a_data.valueFlags.value)),
				stl::underlying(Data::ConfigNodePhysicsFlags::kDisabled));

			ImGui::Spacing();

			const auto disabled = a_data.valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled);

			UICommon::PushDisabled(disabled);

			if (ImGui::TreeNodeEx(
					"tr1",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::General)))
			{
				ImGui::Spacing();

				result |= DrawGeneralOpts(a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::Spacing();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::SphereConstraint, "1"),
				stl::underlying(std::addressof(a_data.valueFlags.value)),
				stl::underlying(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::BoxConstraint, "2"),
				stl::underlying(std::addressof(a_data.valueFlags.value)),
				stl::underlying(Data::ConfigNodePhysicsFlags::kEnableBoxConstraint));

			if (a_data.valueFlags.test_any((Data::ConfigNodePhysicsFlags::kConstraintMask)))
			{
				ImGui::Spacing();

				if (a_data.valueFlags.test((Data::ConfigNodePhysicsFlags::kEnableSphereConstraint)))
				{
					if (ImGui::TreeNodeEx(
							"tr2",
							ImGuiTreeNodeFlags_DefaultOpen |
								ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							UIL::LS(UIPhysicsValueEditorWidgetStrings::SphereConstraint)))
					{
						ImGui::Spacing();

						result |= DrawSphereConstraintOpts(a_data);

						ImGui::Spacing();

						ImGui::TreePop();
					}
				}

				if (a_data.valueFlags.test((Data::ConfigNodePhysicsFlags::kEnableBoxConstraint)))
				{
					if (ImGui::TreeNodeEx(
							"tr3",
							ImGuiTreeNodeFlags_DefaultOpen |
								ImGuiTreeNodeFlags_SpanAvailWidth,
							"%s",
							UIL::LS(UIPhysicsValueEditorWidgetStrings::BoxConstraint)))
					{
						ImGui::Spacing();

						result |= DrawBoxConstraintOpts(a_data);

						ImGui::Spacing();

						ImGui::TreePop();
					}
				}
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			return result;
		}

		bool UIPhysicsValueEditorWidget::DrawGeneralOpts(Data::configNodePhysicsValues_t& a_data)
		{
			bool result = false;

			auto dragSpeed    = ImGui::GetIO().KeyShift ? 0.0001f : 0.04f;
			auto dragSpeedDeg = ImGui::GetIO().KeyShift ? 0.0005f : 0.25f;

			ImGui::PushID("go");

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Stiffness, "1"),
				std::addressof(a_data.stiffness),
				dragSpeed,
				0,
				500);
			UITipsInterface::DrawTip(UITip::PVStiffness);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Stiffness2, "2"),
				std::addressof(a_data.stiffness2),
				dragSpeed,
				0,
				500);
			UITipsInterface::DrawTip(UITip::PVStiffness2);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::SpringSlackOffset, "3"),
				std::addressof(a_data.springSlackOffset),
				dragSpeed,
				0,
				5000);
			UITipsInterface::DrawTip(UITip::PVSpringSlackOffset);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::SpringSlackMag, "4"),
				std::addressof(a_data.springSlackMag),
				dragSpeed,
				0,
				500);
			UITipsInterface::DrawTip(UITip::PVSpringSlackMag);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Damping, "5"),
				std::addressof(a_data.damping),
				dragSpeed,
				0,
				500);
			UITipsInterface::DrawTip(UITip::PVDamping);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Resistance, "6"),
				std::addressof(a_data.resistance),
				dragSpeed,
				0,
				20);
			UITipsInterface::DrawTip(UITip::PVResistance);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::MaxVelocity, "7"),
				std::addressof(a_data.maxVelocity),
				dragSpeed,
				10,
				50000);
			UITipsInterface::DrawTip(UITip::PVMaxVelocity);

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::LinearScale, "8"),
				a_data.linear,
				dragSpeed,
				0,
				10);
			UITipsInterface::DrawTip(UITip::PVLinearScale);

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::RotationScale, "9"),
				a_data.rotational,
				dragSpeed,
				-10,
				10);
			UITipsInterface::DrawTip(UITip::PVRotationScale);

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::RotationAdjust, "A"),
				a_data.rotAdjust,
				dragSpeedDeg,
				-360,
				360);
			UITipsInterface::DrawTip(UITip::PVRotationAdjust);

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::CogOffset, "B"),
				a_data.cogOffset,
				dragSpeed,
				-100,
				100);
			UITipsInterface::DrawTip(UITip::PVCogOffset);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Mass, "C"),
				std::addressof(a_data.mass),
				dragSpeed,
				0.001f,
				1000.0f);
			UITipsInterface::DrawTip(UITip::PVMass);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::GravityBias, "D"),
				std::addressof(a_data.gravityBias),
				dragSpeed,
				0,
				8000);
			UITipsInterface::DrawTip(UITip::PVGravityBias);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::GravityCorrection, "E"),
				std::addressof(a_data.gravityCorrection),
				dragSpeed,
				-500,
				500);
			UITipsInterface::DrawTip(UITip::PVGravityCorrection);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::RotGravityCorrection, "F"),
				std::addressof(a_data.rotGravityCorrection),
				dragSpeed,
				-5000,
				5000);
			UITipsInterface::DrawTip(UITip::PVRotGravityCorrection);

			ImGui::PopID();

			return result;
		}

		bool UIPhysicsValueEditorWidget::DrawSphereConstraintOpts(Data::configNodePhysicsValues_t& a_data)
		{
			bool result = false;

			auto dragSpeed = ImGui::GetIO().KeyShift ? 0.0001f : 0.04f;

			ImGui::PushID("sco");

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::SphereOffset, "1"),
				a_data.maxOffsetSphereOffset,
				dragSpeed,
				-50,
				50);
			UITipsInterface::DrawTip(UITip::PVSphereOffset);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::SphereRadius, "2"),
				std::addressof(a_data.maxOffsetSphereRadius),
				dragSpeed,
				0,
				500);
			UITipsInterface::DrawTip(UITip::PVSphereRadius);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Friction, "3"),
				std::addressof(a_data.maxOffsetSphereFriction),
				dragSpeed,
				0,
				1);
			UITipsInterface::DrawTip(UITip::PVConstraintFriction);

			ImGui::Spacing();

			result |= DrawOffsetParams(a_data.maxOffsetParamsSphere);

			ImGui::PopID();

			return result;
		}

		bool UIPhysicsValueEditorWidget::DrawBoxConstraintOpts(Data::configNodePhysicsValues_t& a_data)
		{
			bool result = false;

			auto dragSpeed = ImGui::GetIO().KeyShift ? 0.0001f : 0.04f;

			ImGui::PushID("bco");

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::BoxMin, "1"),
				a_data.maxOffsetN,
				dragSpeed,
				-128,
				0);
			UITipsInterface::DrawTip(UITip::PVBoxMin);

			result |= ImGui::DragFloat3(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::BoxMax, "2"),
				a_data.maxOffsetP,
				dragSpeed,
				0,
				128);
			UITipsInterface::DrawTip(UITip::PVBoxMax);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::Friction, "3"),
				std::addressof(a_data.maxOffsetBoxFriction),
				dragSpeed,
				0,
				1);
			UITipsInterface::DrawTip(UITip::PVConstraintFriction);

			ImGui::Spacing();

			result |= DrawOffsetParams(a_data.maxOffsetParamsBox);

			ImGui::PopID();

			return result;
		}

		bool UIPhysicsValueEditorWidget::DrawOffsetParams(btVector4& a_params)
		{
			bool result = false;

			auto dragSpeed = ImGui::GetIO().KeyShift ? 0.0001f : 0.04f;

			ImGui::PushID("opar");

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::VelocityResponseScale, "1"),
				std::addressof(a_params[0]),
				dragSpeed,
				0,
				1);
			UITipsInterface::DrawTip(UITip::PVVelocityResponseScale);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::PenBiasFactor, "2"),
				std::addressof(a_params[3]),
				dragSpeed,
				0,
				20);
			UITipsInterface::DrawTip(UITip::PVPenBiasFactor);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::PenBiasDepthLimit, "3"),
				std::addressof(a_params[1]),
				dragSpeed,
				0.5f,
				50000.0f);
			UITipsInterface::DrawTip(UITip::PVPenBiasDepthLimit);

			result |= ImGui::DragFloat(
				UIL::LS(UIPhysicsValueEditorWidgetStrings::RestitutionCoefficient, "4"),
				std::addressof(a_params[2]),
				dragSpeed,
				0,
				1);
			UITipsInterface::DrawTip(UITip::PVRestitutionCoefficient);

			ImGui::PopID();

			return result;
		}
	}
}