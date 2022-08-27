#include "pch.h"

#include "I3DIActorObject.h"

#include "I3DIActorContext.h"
#include "I3DICommonData.h"

#include "Common/VectorMath.h"

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

		void I3DIActorObject::DrawObjectExtra(I3DICommonData& a_data)
		{
			if (IsHovered())
			{
				if (!a_data.actorContext || a_data.actorContext->GetActorObject().get() != this)
				{
					m_bound.DrawBox(a_data.batchDepth, XMVectorReplicate(0.5f));

					constexpr XMVECTOR up = { 0.0f, 0.0f, 1.0f, 0.0f };

					XMVECTOR origin;

					auto pos = m_bound.GetPopupAnchorPoint(
						a_data.scene,
						up,
						15.0f,
						origin);

					a_data.commonPopup.SetLineWorldOrigin(origin);
					a_data.commonPopup.SetPosition(pos, { 0.5f, 1.f });

					char b[64];
					stl::snprintf(b, "###act_%p", this);

					a_data.commonPopup.Draw(b, a_data, [&] {
						ImGui::Text("%.8X", m_actor);
					});
				}
			}
		}

		void I3DIActorObject::OnClick(I3DICommonData& a_data)
		{
			if (a_data.actorContext && a_data.actorContext->GetActorObject().get() == this)
			{
				return;
			}

			a_data.queuedActor = m_actor;
		}

		bool I3DIActorObject::ObjectIntersects(
			I3DICommonData& a_data,
			float&          a_dist)
		{
			if (a_data.actorContext && a_data.actorContext->GetActorObject().get() == this)
			{
				return false;
			}

			return m_bound.Intersects(a_data.ray.origin, a_data.ray.dir, a_dist);
		}

		void I3DIActorObject::Update(
			const ActorObjectHolder& a_holder)
		{
			auto& actor = a_holder.GetActor();

			const auto min = actor->GetBoundMin().GetMM();
			const auto max = actor->GetBoundMax().GetMM();

			const auto center = (min + max) * 0.5f;
			const auto extent = (max - min) * 0.5f;

			XMFLOAT3 m, n;

			XMStoreFloat3(std::addressof(m), center);
			XMStoreFloat3(std::addressof(n), extent);

			BoundingOrientedBox tmp(m, n, { 0, 0, 0, 1.0f });
			tmp.Transform(m_bound, VectorMath::NiTransformToMatrix4x4(a_holder.GetNPCRoot()->m_worldTransform));
		}
	}
}