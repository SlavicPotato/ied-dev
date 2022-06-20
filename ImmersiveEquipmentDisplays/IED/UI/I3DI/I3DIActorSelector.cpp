#include "pch.h"

#include "I3DIActorSelector.h"

#include "IED/Controller/Controller.h"

#include "IED/D3D/D3DCommon.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIActorSelector::I3DIActorSelector(
			ID3D11Device*        a_device,
			ID3D11DeviceContext* a_context) noexcept(false) :
			m_batch(a_device, a_context)
		{
			m_batch.EnableDepth(true);
		}

		void I3DIActorSelector::Update(
			Controller& a_controller)
		{
			for (const auto& [i, e] : a_controller.GetData())
			{
				auto& actor = e.GetActor();

				if (!e.IsCellAttached() || !actor->formID)
				{
					m_data.erase(i);
					continue;
				}

				auto& r = m_data.try_emplace(i).first->second;

				const auto min = actor->GetBoundMin().GetMM();
				const auto max = actor->GetBoundMax().GetMM();

				auto center = (min + max) * 0.5f;
				auto extent = (max - min) * 0.5f;

				r.data.first.first  = center;
				r.data.first.second = extent;

				XMFLOAT3 m, n;

				XMStoreFloat3(std::addressof(m), center);
				XMStoreFloat3(std::addressof(n), extent);

				BoundingOrientedBox tmp(m, n, { 0, 0, 0, 1.0f });
				r.data.second = VectorMath::NiTransformTo4x4Matrix(e.GetNPCRoot()->m_worldTransform);
				tmp.Transform(r.bound, r.data.second);
			}

			for (auto it = m_data.begin(); it != m_data.end();)
			{
				if (!a_controller.GetData().contains(it->first))
				{
					it = m_data.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void XM_CALLCONV I3DIActorSelector::DrawBox(
			const Entry& a_entry,
			XMVECTOR     a_color)
		{
			const auto min = a_entry.data.first.first - a_entry.data.first.second;
			const auto max = a_entry.data.first.first + a_entry.data.first.second;

			const auto worldMatrix = a_entry.data.second;

			const auto p1 = XMVector3Transform(min, worldMatrix);
			const auto p2 = XMVector3Transform(XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(min), 0.0f), worldMatrix);
			const auto p3 = XMVector3Transform(XMVectorSet(XMVectorGetX(max), XMVectorGetY(max), XMVectorGetZ(min), 0.0f), worldMatrix);
			const auto p4 = XMVector3Transform(XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(min), 0.0f), worldMatrix);
			const auto p5 = XMVector3Transform(XMVectorSet(XMVectorGetX(min), XMVectorGetY(min), XMVectorGetZ(max), 0.0f), worldMatrix);
			const auto p6 = XMVector3Transform(XMVectorSet(XMVectorGetX(max), XMVectorGetY(min), XMVectorGetZ(max), 0.0f), worldMatrix);
			const auto p7 = XMVector3Transform(max, worldMatrix);
			const auto p8 = XMVector3Transform(XMVectorSet(XMVectorGetX(min), XMVectorGetY(max), XMVectorGetZ(max), 0.0f), worldMatrix);

			m_batch.AddLine(p1, p2, a_color);
			m_batch.AddLine(p2, p3, a_color);
			m_batch.AddLine(p3, p4, a_color);
			m_batch.AddLine(p4, p1, a_color);
			m_batch.AddLine(p1, p5, a_color);
			m_batch.AddLine(p2, p6, a_color);
			m_batch.AddLine(p3, p7, a_color);
			m_batch.AddLine(p4, p8, a_color);
			m_batch.AddLine(p5, p6, a_color);
			m_batch.AddLine(p6, p7, a_color);
			m_batch.AddLine(p7, p8, a_color);
			m_batch.AddLine(p8, p5, a_color);
		}

		DirectX::XMVECTOR XM_CALLCONV I3DIActorSelector::GetPopupAnchorPoint(
			D3DCommon&   a_scene,
			const Entry& a_entry,
			XMVECTOR&    a_origin)
		{
			const auto origin = a_entry.data.first.first + XMVectorSet(0.0f, 0.0f, XMVectorGetZ(a_entry.data.first.second), 0.0f);
			const auto anchor = origin + XMVectorSet(0.0f, 0.0f, 12.5f, 0.0f);

			a_origin = XMVector3Transform(origin, a_entry.data.second);

			return VectorMath::WorldToScreenSpace(
				a_scene.GetViewport(),
				anchor,
				a_scene.GetViewMatrix(),
				a_scene.GetProjectionMatrix(),
				a_entry.data.second);
		}

		void I3DIActorSelector::Render(D3DCommon& a_scene)
		{
			m_batch.Draw(a_scene);
		}
	}
}