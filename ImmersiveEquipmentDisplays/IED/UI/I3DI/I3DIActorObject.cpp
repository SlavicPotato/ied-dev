#include "pch.h"

#include "I3DIActorObject.h"

#include "I3DIActorContext.h"
#include "I3DICommonData.h"

#include "Common/VectorMath.h"

#include "IED/Controller/ActorObjectHolder.h"
#include "IED/Controller/ObjectManagerData.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIActorObject::I3DIActorObject(
			Game::FormID a_actor) :
			m_actor(a_actor)
		{
		}

		const D3DBoundingOrientedBox* I3DIActorObject::GetBoundingBox() const
		{
			return std::addressof(m_bound);
		}

		void I3DIActorObject::DrawObjectExtra(I3DICommonData& a_data)
		{
			if (!IsHovered())
			{
				return;
			}

			if (a_data.IsCurrentActorObject(this))
			{
				return;
			}

			m_bound.DrawBox(a_data.batchDepth, XMVectorReplicate(0.5f));

			XMVECTOR origin;

			const auto pos = m_bound.GetPopupAnchorPoint(
				a_data.scene,
				DirectX::g_XMIdentityR2.v, // up
				15.0f,
				origin);

			a_data.commonPopup.SetLineWorldOrigin(origin);
			a_data.commonPopup.SetPosition(pos, { 0.5f, 1.f });

			char b[64];
			stl::snprintf(b, "###act_%.8X", this);

			a_data.commonPopup.Draw(b, a_data, [&] {
				ImGui::Text("%.8X", m_actor);
			});
		}

		void I3DIActorObject::OnClick(I3DICommonData& a_data)
		{
			if (!a_data.IsCurrentActorObject(this))
			{
				a_data.queuedActor = m_actor;
			}
		}

		bool I3DIActorObject::ObjectIntersects(
			I3DICommonData& a_data,
			const I3DIRay&  a_ray,
			float&          a_dist)
		{
			if (a_data.IsCurrentActorObject(this))
			{
				return false;
			}

			return m_bound.Intersects(a_ray.origin, a_ray.dir, a_dist);
		}

		void I3DIActorObject::Update(
			const ActorObjectHolder& a_holder)
		{
			NiPointer<TESObjectREFR> refr;
			if (!a_holder.GetHandle().Lookup(refr))
			{
				return;
			}

			const auto min = refr->GetBoundMin().GetMM();
			const auto max = refr->GetBoundMax().GetMM();

			const auto center = (min + max) * DirectX::g_XMOneHalf.v;
			const auto extent = (max - min) * DirectX::g_XMOneHalf.v;

			XMFLOAT3 m, n;

			XMStoreFloat3(std::addressof(m), center);
			XMStoreFloat3(std::addressof(n), extent);

			BoundingOrientedBox tmp(m, n, DirectX::SimpleMath::Quaternion::Identity);
			tmp.Transform(m_bound, VectorMath::NiTransformToMatrix4x4(a_holder.GetNPCRoot()->m_worldTransform));
		}
	}
}