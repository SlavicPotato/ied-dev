#include "pch.h"

#include "I3DIModelObject.h"

#include "I3DICommonData.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIModelObject::I3DIModelObject(
			ID3D11Device*                        a_device,
			ID3D11DeviceContext*                 a_context,
			const std::shared_ptr<D3DModelData>& a_data) :
			D3DObject(a_device, a_data)
		{
		}

		void I3DIModelObject::RenderObject(D3DCommon& a_data)
		{
			Draw(a_data);
		}

		void I3DIModelObject::OnMouseMoveOver(I3DICommonData& a_data)
		{
			m_effect->SetAlpha(1.0f);
		}

		void I3DIModelObject::OnMouseMoveOut(I3DICommonData& a_data)
		{
			if (!m_objectFlags.test_any(I3DIObjectFlags::kHSMask))
			{
				m_effect->SetAlpha(0.5f);
			}
		}

		void I3DIModelObject::OnSelect(I3DICommonData& a_data)
		{
			m_effect->SetAlpha(1.0f);
		}

		void I3DIModelObject::OnUnselect(I3DICommonData& a_data)
		{
			if (!m_objectFlags.test_any(I3DIObjectFlags::kHSMask))
			{
				m_effect->SetAlpha(0.5f);
			}
		}

		bool  I3DIModelObject::ObjectIntersects(
			I3DICommonData& a_data,
			float&            a_dist)
		{
			return Intersects(a_data.ray.origin, a_data.ray.dir, a_dist);
		}

		bool I3DIModelObject::WantDrawTooltip()
		{
			return false;
		}

		bool I3DIModelObject::WantDrawBound()
		{
			return false;
		}

		void I3DIModelObject::DrawObjectExtra(I3DICommonData& a_data)
		{
			auto& bound = GetBound();

			if (WantDrawBound())
			{
				bound.DrawBox(a_data.batchDepth, XMVectorReplicate(0.5f));
			}

			if (WantDrawTooltip())
			{
				//XMVECTOR origin;

				/*constexpr XMVECTOR dir = { -1.0f, 0.0f, 0.0f, 0.0f };

				auto pos = bound.GetPopupAnchorPoint(
					a_data.scene,
					dir,
					12.5f,
					origin);*/

				constexpr XMVECTOR offset = { 30.0f, 30.0f, 30.0f, 30.0f };
				constexpr XMVECTOR maxLen = { 50.0f, 50.0f, 50.0f, 50.0f };

				auto origin = GetWorldMatrix().r[3];

				auto v   = origin - GetParentCenter();
				auto len = XMVectorMin(XMVector3Length(v) + offset, maxLen);

				auto p = origin + (XMVector3Normalize(v) * len);

				auto pos = VectorMath::WorldToScreenSpace(
					a_data.scene.GetViewport(),
					p,
					a_data.scene.GetViewMatrix(),
					a_data.scene.GetProjectionMatrix());

				a_data.commonPopup.SetLineWorldOrigin(origin);
				a_data.commonPopup.SetPosition(pos, { 0.5f, 1.f });

				char b[64];
				stl::snprintf(b, "###i3di_obj_%p", this);

				a_data.commonPopup.Draw(b, a_data, [&] {
					DrawTooltip(a_data);
				});
			}
		}

	}
}