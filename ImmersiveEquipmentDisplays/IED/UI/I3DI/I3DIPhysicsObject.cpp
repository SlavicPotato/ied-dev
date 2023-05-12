#include "pch.h"

#include "I3DIBoundingSphere.h"
#include "I3DICommonData.h"
#include "I3DIInputHelpers.h"
#include "I3DIPhysicsObject.h"
#include "I3DISphereObject.h"

#include "IED/Physics/SimComponent.h"

#include "IED/D3D/D3DPrimitiveBatch.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		static constexpr XMVECTOR diffuseColor1 = { 1, 0, 0, 1 };
		static constexpr XMVECTOR diffuseColor2 = { 1, 0.175f, 0.175f, 1 };

		I3DIPhysicsObject::I3DIPhysicsObject(
			I3DICommonData&        a_data,
			I3DIActorContext&      a_actorContext,
			const PHYSimComponent& a_sc) noexcept(false) :
			I3DIModelObject(
				a_data.scene.GetDevice().Get(),
				BoundingShape::kSphere,
				a_data.assets.GetModel(I3DIModelID::kSphere)),
			I3DIDraggable(I3DIDraggableType::Static),
			m_actorContext(a_actorContext),
			m_tag(a_sc.GetLUID()),
			m_flags(a_sc.GetConfig().valueFlags),
			m_name(a_sc.GetObject()->m_name.c_str()),
			m_virtSphere(std::make_shared<I3DISphereObject>(a_data, this)),
			m_constraintSphere(std::make_shared<I3DISphereObject>(a_data, this)),
			m_cogSphere(std::make_shared<I3DISphereObject>(a_data, this))
		{
			m_objectFlags.set(I3DIObjectFlags::kHideOtherWhenSelected);

			SetDiffuseColor(diffuseColor1);
			SetAlpha(0.8f);
			EnableDepth(false);
			SetLightingEnabled(true);
			//SetRasterizerState(D3DObjectRasterizerState::kWireframe);

			m_virtSphere->EnableDepth(false);
			m_virtSphere->SetAlpha(0.8f);
			m_virtSphere->SetDiffuseColor({ 0, 1, 0, 1 });
			m_virtSphere->SetLightingEnabled(true);

			m_cogSphere->EnableDepth(false);
			m_cogSphere->SetAlpha(0.8f);
			m_cogSphere->SetDiffuseColor({ 0.9882f, 0.4666f, 0.0117f, 1 });
			m_cogSphere->SetLightingEnabled(true);

			m_constraintSphere->EnableDepth(false);
			m_constraintSphere->SetAlpha(0.5f);
			m_constraintSphere->SetDiffuseColor({ 1, 1, 1, 1 });
			m_constraintSphere->SetLightingEnabled(false);
			m_constraintSphere->SetRasterizerState(D3DObjectRasterizerState::kWireframe);

			UpdateData(a_sc);
		}

		void I3DIPhysicsObject::UpdateData(const PHYSimComponent& a_sc)
		{
			m_parentTransform = a_sc.GetCachedParentWorldTransform();
			m_objectTransform = Bullet::btTransformEx(a_sc.GetObject()->m_worldTransform);

			auto& initialTransform = a_sc.GetObjectInitialTransform();

			m_axis = m_parentTransform.getBasis() * (initialTransform.getBasis() * a_sc.GetRotationAxis());

			m_bbMin = initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetN;
			m_bbMax = initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetP;

			UpdateWorldMatrix(VectorMath::NiTransformToMatrix4x4(a_sc.GetObject()->m_worldTransform));
			SetHasWorldData(true);
			UpdateBound();

			auto ms = XMMatrixScalingFromVector(m_objectTransform.getScale().get128() * 0.6f);
			auto mt = XMMatrixTranslationFromVector((m_parentTransform * (initialTransform.getOrigin() + a_sc.GetVirtualPos())).get128());

			m_virtSphere->UpdateWorldMatrix(ms * mt);
			m_virtSphere->SetHasWorldData(true);
			m_virtSphere->UpdateBound();

			mt = XMMatrixTranslationFromVector((m_parentTransform * a_sc.GetConfig().cogOffset).get128());

			m_cogSphere->UpdateWorldMatrix(ms * mt);
			m_cogSphere->SetHasWorldData(true);
			m_cogSphere->UpdateBound();

			m_flags = a_sc.GetConfig().valueFlags;

			if (m_flags.test(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint))
			{
				ms = XMMatrixScalingFromVector(m_parentTransform.getScale().get128() * a_sc.GetConfig().maxOffsetSphereRadius);
				mt = XMMatrixTranslationFromVector((m_parentTransform * (initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetSphereOffset)).get128());

				m_constraintSphere->UpdateWorldMatrix(ms * mt);
				m_constraintSphere->SetHasWorldData(true);
				m_constraintSphere->UpdateBound();
			}

			m_constraintSphere->SetGeometryHidden(!m_flags.test(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint));

			if (m_afd)
			{
				a_sc.ApplyForce(m_afd->second, 120.0f);
			}

			/*m_virtld           = a_sc.GetVirtualPos();
			m_velocity         = a_sc.GetVelocity();
			m_initialTransform = initialTransform;*/
		}

		/*void I3DIPhysicsObject::UpdateBound()
		{
			auto& bound = GetBoundingShape<I3DIBoundingSphere>()->GetBound();

			bound.Radius = 0.75f * m_objectTransform.getScale();
			XMStoreFloat3(&bound.Center, m_objectTransform.getOrigin().get128());
		}*/

		void I3DIPhysicsObject::OnObjectRegistered(I3DIObjectController& a_data)
		{
			a_data.RegisterObject(m_virtSphere);
			a_data.RegisterObject(m_constraintSphere);
			a_data.RegisterObject(m_cogSphere);
		}

		void I3DIPhysicsObject::OnObjectUnregistered(I3DIObjectController& a_data)
		{
			a_data.UnregisterObject(m_cogSphere);
			a_data.UnregisterObject(m_constraintSphere);
			a_data.UnregisterObject(m_virtSphere);
		}

		bool I3DIPhysicsObject::OnDragBegin(I3DICommonData& a_data, ImGuiMouseButton a_button)
		{
			if (a_button == ImGuiMouseButton_Left)
			{
				if (auto dist = GetLastDistance())
				{
					m_afd.emplace(*dist, a_data.cursorRay.origin + a_data.cursorRay.dir * *dist);

					return true;
				}
			}

			return false;
		}

		void I3DIPhysicsObject::OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target)
		{
			m_afd.reset();
		}

		void I3DIPhysicsObject::OnDragUpdate(I3DICommonData& a_data)
		{
			if (m_afd)
			{
				m_afd->second =
					a_data.cursorRay.origin +
					a_data.cursorRay.dir * m_afd->first;
			}
		}

		void I3DIPhysicsObject::OnMouseMoveOver(I3DICommonData& a_data)
		{
			SetDiffuseColor(diffuseColor2);
		}

		void I3DIPhysicsObject::OnMouseMoveOut(I3DICommonData& a_data)
		{
			SetDiffuseColor(diffuseColor1);
		}

		void I3DIPhysicsObject::DrawObjectExtra(I3DICommonData& a_data)
		{
			DrawImpl(a_data.batchNoDepth, g_XMOne.v);

			if (m_objectFlags.test(I3DIObjectFlags::kHovered))
			{
				char b[64];

				const XMVECTOR wp = (m_objectTransform * (g_XMIdentityR2.v * 30.0f)).get128();

				const auto sp = VectorMath::WorldToScreenSpace(
					a_data.scene.GetViewport(),
					wp,
					a_data.scene.GetViewMatrix(),
					a_data.scene.GetProjectionMatrix());

				a_data.commonPopup.SetLineWorldOrigin(m_objectTransform.getOrigin().get128());
				a_data.commonPopup.SetPosition(sp, { 0.5f, 1.f });

				stl::snprintf(b, "###i3di_obj_po_%p", this);

				a_data.commonPopup.Draw(b, a_data, [&] {
					ImGui::Text(m_name.c_str());
				});
			}
		}

		bool I3DIPhysicsObject::IsSelectable()
		{
			return true;
		}

		bool I3DIPhysicsObject::OnSelect(I3DICommonData& a_data)
		{
			SetAlpha(1);
			m_virtSphere->SetAlpha(1);
			m_cogSphere->SetAlpha(1);
			m_constraintSphere->SetAlpha(0.6f);

			return true;
		}

		void I3DIPhysicsObject::OnUnselect(I3DICommonData& a_data)
		{
			if (!m_objectFlags.test_any(I3DIObjectFlags::kHSMask))
			{
				SetAlpha(0.8f);
				m_virtSphere->SetAlpha(0.8f);
				m_cogSphere->SetAlpha(0.8f);
				m_constraintSphere->SetAlpha(0.5f);
			}
		}

		void XM_CALLCONV I3DIPhysicsObject::DrawImpl(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color)
		{
			if (m_flags.test(Data::ConfigNodePhysicsFlags::kEnableBoxConstraint))
			{
				DrawBoxConstraint(a_batch, XMVectorSetW(a_color, 0.6f));
			}

			DrawOrientation(a_batch);

			/*auto p1 = (m_parentTransform * m_initialTransform.getOrigin());
			auto p2 = (m_parentTransform * (m_initialTransform.getOrigin() + m_virtld));

			const auto n = XMVector3Normalize((p2 - p1).get128());

			auto nx = XMVector3Normalize((m_velocity - n * XMVector3Dot(m_velocity.get128(), n)).get128());

			const XMVECTOR pos = m_objectTransform.getOrigin().get128();

			constexpr XMVECTOR col1 = { 1, 0, 0, 1 };
			constexpr XMVECTOR col2 = { 0, 1, 0, 1 };
			constexpr XMVECTOR col3 = { 0, 0, 1, 1 };

			constexpr XMVECTOR lb = { 20, 20, 20, 0 };

			const auto l = lb * m_objectTransform.getScale();

			const auto pf = pos + n * l;
			const auto px = pos + nx * l;

			a_batch.AddLine(pos, pf, col1);
			a_batch.AddLine(pos, px, col2);*/
		}

		void XM_CALLCONV I3DIPhysicsObject::DrawBoxConstraint(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color)
		{
			const auto p1 = m_parentTransform * m_bbMin;
			const auto p2 = m_parentTransform * btVector3(m_bbMax.x(), m_bbMin.y(), m_bbMin.z());
			const auto p3 = m_parentTransform * btVector3(m_bbMax.x(), m_bbMax.y(), m_bbMin.z());
			const auto p4 = m_parentTransform * btVector3(m_bbMin.x(), m_bbMax.y(), m_bbMin.z());
			const auto p5 = m_parentTransform * btVector3(m_bbMin.x(), m_bbMin.y(), m_bbMax.z());
			const auto p6 = m_parentTransform * btVector3(m_bbMax.x(), m_bbMin.y(), m_bbMax.z());
			const auto p7 = m_parentTransform * m_bbMax;
			const auto p8 = m_parentTransform * btVector3(m_bbMin.x(), m_bbMax.y(), m_bbMax.z());

			a_batch.AddLine(p1.get128(), p2.get128(), a_color);
			a_batch.AddLine(p2.get128(), p3.get128(), a_color);
			a_batch.AddLine(p3.get128(), p4.get128(), a_color);
			a_batch.AddLine(p4.get128(), p1.get128(), a_color);
			a_batch.AddLine(p1.get128(), p5.get128(), a_color);
			a_batch.AddLine(p2.get128(), p6.get128(), a_color);
			a_batch.AddLine(p3.get128(), p7.get128(), a_color);
			a_batch.AddLine(p4.get128(), p8.get128(), a_color);
			a_batch.AddLine(p5.get128(), p6.get128(), a_color);
			a_batch.AddLine(p6.get128(), p7.get128(), a_color);
			a_batch.AddLine(p7.get128(), p8.get128(), a_color);
			a_batch.AddLine(p8.get128(), p5.get128(), a_color);
		}

		/*void XM_CALLCONV I3DIPhysicsObject::DrawSphereConstraint(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color)
		{
			DrawSphere(a_batch, m_sphereOrigin.get128(), m_sphereRadius, a_color);
		}*/

		void I3DIPhysicsObject::DrawOrientation(D3DPrimitiveBatch& a_batch)
		{
			const auto& basis = m_objectTransform.getBasis();

			const XMVECTOR fwd   = basis.getColumn(0).get128();
			const XMVECTOR up    = basis.getColumn(1).get128();
			const XMVECTOR right = basis.getColumn(2).get128();
			const XMVECTOR pos   = m_objectTransform.getOrigin().get128();

			constexpr XMVECTOR lb = { 20, 20, 20, 0 };

			const auto l = lb * m_objectTransform.getScale().get128();

			const auto pf = pos + fwd * l;
			const auto pu = pos + up * l;
			const auto pr = pos + right * l;

			const auto alpha = IsSelected() ? 1.0f : 0.8f;

			const XMVECTOR col1 = { 1, 0, 0, alpha };
			const XMVECTOR col2 = { 0, 1, 0, alpha };
			const XMVECTOR col3 = { 0, 0, 1, alpha };

			a_batch.AddLine(pos, pf, col1);
			a_batch.AddLine(pos, pu, col2);
			a_batch.AddLine(pos, pr, col3);

			const XMVECTOR cola = { 1, 0, 1, alpha };
			const XMVECTOR colb = { 1, 1, 0, alpha };

			const auto pm = m_axis.get128() * l;

			a_batch.AddLine(pos - pm, cola, pos + pm, colb);
		}

	}
}
