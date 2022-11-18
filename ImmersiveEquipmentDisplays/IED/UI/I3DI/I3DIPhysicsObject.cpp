#include "pch.h"

#include "I3DIBoundingSphere.h"
#include "I3DICommonData.h"
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
			m_actorContext(a_actorContext),
			m_tag(a_sc.GetConfig()),
			m_flags(a_sc.GetConfig().valueFlags),
			m_name(a_sc.GetObject()->m_name.c_str()),
			m_virtSphere(std::make_unique<I3DISphereObject>(a_data)),
			m_constraintSphere(std::make_unique<I3DISphereObject>(a_data))
		{
			SetDiffuseColor(diffuseColor1);
			SetAlpha(1);
			EnableDepth(false);
			//SetLightingEnabled(false);
			//SetRasterizerState(D3DObjectRasterizerState::kWireframe);

			m_virtSphere->EnableDepth(false);
			m_virtSphere->SetAlpha(1);
			m_virtSphere->SetDiffuseColor({ 0, 1, 0, 1 });
			m_virtSphere->SetLightingEnabled(true);

			m_constraintSphere->EnableDepth(false);
			m_constraintSphere->SetAlpha(0.65f);
			m_constraintSphere->SetDiffuseColor({ 1, 1, 1, 1 });
			m_constraintSphere->SetLightingEnabled(true);
			m_constraintSphere->SetRasterizerState(D3DObjectRasterizerState::kWireframe);

			UpdateData(a_sc);
		}

		void I3DIPhysicsObject::UpdateData(const PHYSimComponent& a_sc)
		{
			m_parentTransform = a_sc.GetCachedParentWorldTransform();
			m_objectTransform = Bullet::btTransformEx(a_sc.GetObject()->m_worldTransform);

			auto& initialTransform = a_sc.GetObjectInitialTransform();

			m_axis = m_parentTransform.getBasis() * (initialTransform.getBasis() * a_sc.GetRotationAxis());

			//m_sphereRadius = a_sc.GetConfig().maxOffsetSphereRadius * m_parentTransform.getScale();
			//m_sphereOrigin = m_parentTransform * (initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetSphereOffset);

			m_bbMin = initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetN;
			m_bbMax = initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetP;

			UpdateWorldMatrix(VectorMath::NiTransformToMatrix4x4(a_sc.GetObject()->m_worldTransform));
			//SetOriginalWorldMatrix(m);
			SetHasWorldData(true);
			UpdateBound();

			//auto mr = XMMatrixIdentity();
			auto ms = XMMatrixScalingFromVector(XMVectorReplicate(0.6f * m_objectTransform.getScale()));
			auto mt = XMMatrixTranslationFromVector((m_parentTransform * a_sc.GetVirtualPos()).get128());

			m_virtSphere->UpdateWorldMatrix(ms * mt);
			m_virtSphere->SetHasWorldData(true);
			m_virtSphere->UpdateBound();

			if (m_flags.test(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint))
			{
				//mr = XMMatrixIdentity();
				ms = XMMatrixScalingFromVector(XMVectorReplicate(a_sc.GetConfig().maxOffsetSphereRadius * m_parentTransform.getScale()));
				mt = XMMatrixTranslationFromVector((m_parentTransform * (initialTransform.getOrigin() + a_sc.GetConfig().maxOffsetSphereOffset)).get128());

				m_constraintSphere->UpdateWorldMatrix(ms * mt);
				m_constraintSphere->SetHasWorldData(true);
				m_constraintSphere->UpdateBound();
			}
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
		}

		void I3DIPhysicsObject::OnObjectUnregistered(I3DIObjectController& a_data)
		{
			a_data.UnregisterObject(m_virtSphere);
			a_data.UnregisterObject(m_constraintSphere);
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

		void XM_CALLCONV I3DIPhysicsObject::DrawImpl(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color)
		{
			/*constexpr XMVECTOR cols1n = { 1, 0, 0, 1 };
			constexpr XMVECTOR cols1h = { 1, 0.5f, 0.5f, 1 };
			constexpr XMVECTOR cols2 = { 0, 1, 0, 1 };*/

			/*DrawSphere(
				a_batch,
				m_objectTransform.getOrigin().get128(),
				0.75f * m_objectTransform.getScale(),
				m_objectFlags.test(I3DIObjectFlags::kHovered) ? cols1h : cols1n);*/

			/*DrawSphere(
				a_batch,
				m_virtualPos.get128(),
				0.6f * m_objectTransform.getScale(),
				cols2);*/

			if (m_flags.test(Data::ConfigNodePhysicsFlags::kEnableBoxConstraint))
			{
				DrawBoxConstraint(a_batch, XMVectorSetW(a_color, 0.6f));
			}

			/*if (m_flags.test(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint))
			{
				DrawSphereConstraint(a_batch, XMVectorSetW(a_color, 0.2f));
			}*/

			m_constraintSphere->SetGeometryHidden(!m_flags.test(Data::ConfigNodePhysicsFlags::kEnableSphereConstraint));

			DrawOrientation(a_batch);
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

			const auto l = lb * m_objectTransform.getScale();

			const auto pf = pos + fwd * l;
			const auto pu = pos + up * l;
			const auto pr = pos + right * l;

			constexpr XMVECTOR col1 = { 1, 0, 0, 1 };
			constexpr XMVECTOR col2 = { 0, 1, 0, 1 };
			constexpr XMVECTOR col3 = { 0, 0, 1, 1 };

			a_batch.AddLine(pos, pf, col1);
			a_batch.AddLine(pos, pu, col2);
			a_batch.AddLine(pos, pr, col3);

			constexpr XMVECTOR cola = { 1, 0, 1, 1 };
			constexpr XMVECTOR colb = { 1, 1, 0, 1 };

			const auto pm = m_axis.get128() * l;

			a_batch.AddLine(pos - pm, cola, pos + pm, colb);
		}

		void XM_CALLCONV I3DIPhysicsObject::DrawSphere(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_pos,
			float              a_radius,
			XMVECTOR           a_color)
		{
			XMVECTOR vertices[(NB_SECTORS_SPHERE + 1) * (NB_STACKS_SPHERE + 1) + (NB_SECTORS_SPHERE + 1)];

			// Vertices
			const float sectorStep = 2 * std::numbers::pi_v<float> / NB_SECTORS_SPHERE;
			const float stackStep  = std::numbers::pi_v<float> / NB_STACKS_SPHERE;

			for (std::uint32_t i = 0; i <= NB_STACKS_SPHERE; i++)
			{
				const float stackAngle = std::numbers::pi_v<float> / 2 - i * stackStep;

				float s, c;
				XMScalarSinCos(&s, &c, stackAngle);

				const float radiusCosStackAngle = a_radius * c;
				const float z                   = a_radius * s;

				for (std::uint32_t j = 0; j <= NB_SECTORS_SPHERE; j++)
				{
					const float sectorAngle = j * sectorStep;

					XMScalarSinCos(&s, &c, sectorAngle);

					const float x = radiusCosStackAngle * c;
					const float y = radiusCosStackAngle * s;

					assert(i * (NB_SECTORS_SPHERE + 1) + j < sizeof(vertices));

					vertices[i * (NB_SECTORS_SPHERE + 1) + j] = a_pos + DirectX::XMVectorSet(x, y, z, 0);
				}
			}

			// Faces
			for (std::uint32_t i = 0; i < NB_STACKS_SPHERE; i++)
			{
				std::uint32_t a1 = i * (NB_SECTORS_SPHERE + 1);
				std::uint32_t a2 = a1 + NB_SECTORS_SPHERE + 1;

				for (std::uint32_t j = 0; j < NB_SECTORS_SPHERE; j++, a1++, a2++)
				{
					// 2 triangles per sector except for the first and last stacks

					if (i != 0)
					{
						a_batch.AddTriangle(vertices[a1], vertices[a2], vertices[a1 + 1], a_color);
					}

					if (i != (NB_STACKS_SPHERE - 1))
					{
						a_batch.AddTriangle(vertices[a1 + 1], vertices[a2], vertices[a2 + 1], a_color);
					}
				}
			}
		}
	}
}
