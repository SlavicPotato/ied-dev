#include "pch.h"

#include "I3DIModelObject.h"

#include "I3DIActorObject.h"
#include "I3DIBoundingOrientedBox.h"
#include "I3DIBoundingSphere.h"
#include "I3DICommonData.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIModelObject::I3DIModelObject(
			ID3D11Device*                        a_device,
			BoundingShape                        a_boundingShape,
			const std::shared_ptr<D3DModelData>& a_data) :
			I3DIBoundObject(a_boundingShape),
			D3DObject(a_device, a_data)
		{
		}

		void I3DIModelObject::RenderObject(D3DCommon& a_data)
		{
			Draw(a_data);
		}

		void I3DIModelObject::OnMouseMoveOver(I3DICommonData& a_data)
		{
			SetAlpha(1.0f);
		}

		void I3DIModelObject::OnMouseMoveOut(I3DICommonData& a_data)
		{
			if (!m_objectFlags.test_any(I3DIObjectFlags::kHSMask))
			{
				SetAlpha(0.5f);
			}
		}

		bool I3DIModelObject::OnSelect(I3DICommonData& a_data)
		{
			SetAlpha(1.0f);
			return true;
		}

		void I3DIModelObject::OnUnselect(I3DICommonData& a_data)
		{
			if (!m_objectFlags.test_any(I3DIObjectFlags::kHSMask))
			{
				SetAlpha(0.5f);
			}
		}

		void I3DIModelObject::SetLastDistance(const std::optional<float>& a_distance)
		{
			m_lastDistance = a_distance;
		}

		std::optional<float> I3DIModelObject::GetLastDistance() const
		{
			return m_lastDistance;
		}

		I3DIBoundObject* I3DIModelObject::GetParentObject() const
		{
			return nullptr;
		}

		bool I3DIModelObject::WantDrawTooltip()
		{
			return false;
		}

		bool I3DIModelObject::WantDrawBound()
		{
			return false;
		}

		bool I3DIModelObject::WantDrawContents()
		{
			return false;
		}

		void I3DIModelObject::UpdateBound()
		{
			switch (GetBoundingShapeType())
			{
			case BoundingShape::kOrientedBox:
				GetModelData()->GetBoundingOrientedBox().Transform(
					GetBoundingShape<I3DIBoundingOrientedBox>()->GetBound(),
					m_world);
				break;
			case BoundingShape::kSphere:
				GetModelData()->GetBoundingSphere().Transform(
					GetBoundingShape<I3DIBoundingSphere>()->GetBound(),
					m_world);
				break;
			default:
				throw std::runtime_error(__FUNCTION__ ": bad bounding shape type");
			}
		}

		void I3DIModelObject::DrawObjectExtra(I3DICommonData& a_data)
		{
			if (WantDrawBound())
			{
				DrawBoundingShape(a_data.batchDepth, XMVectorReplicate(0.5f));
			}

			char b[64];

			if (WantDrawTooltip())
			{
				if (auto parent = GetParentObject())
				{
					if (auto parentBound = parent->GetBoundingShape<I3DIBoundingOrientedBox>())
					{
						constexpr XMVECTOR offset = { 30.0f, 30.0f, 30.0f, 30.0f };
						constexpr XMVECTOR maxLen = { 50.0f, 50.0f, 50.0f, 50.0f };
						constexpr XMVECTOR zScale = { 0.2f, 0.2f, 0.2f, 0.2f };

						const auto& pbox = parentBound->GetBound();

						const auto origin      = GetBoundingShapeCenter();
						const auto center      = XMLoadFloat3(std::addressof(pbox.Center));
						const auto extents     = XMLoadFloat3(std::addressof(pbox.Extents));
						const auto orientation = XMLoadFloat4(std::addressof(pbox.Orientation));

						const auto f = D3DBoundingOrientedBox::GetPoint(DirectX::g_XMIdentityR1.v, center, extents, orientation);
						const auto r = D3DBoundingOrientedBox::GetPoint(DirectX::g_XMIdentityR0.v, center, extents, orientation);
						const auto u = D3DBoundingOrientedBox::GetPoint(DirectX::g_XMIdentityR2.v, center, extents, orientation);

						const auto nf = XMVector3Normalize(f - center);
						const auto nr = XMVector3Normalize(r - center);
						const auto nu = XMVector3Normalize(u - center);

						const auto d = origin - center;

						const auto mf = nf * XMVector3Dot(nf, d);
						const auto mr = nr * XMVector3Dot(nr, d);
						const auto mu = nu * XMVector3Dot(nu, d);

						const auto l = XMVectorMin(XMVector3Length(d) + offset, maxLen);

						const auto wp = origin + XMVector3Normalize(mf + mr + mu * zScale) * l;

						const auto sp = VectorMath::WorldToScreenSpace(
							a_data.scene.GetViewport(),
							wp,
							a_data.scene.GetViewMatrix(),
							a_data.scene.GetProjectionMatrix());

						a_data.commonPopup.SetLineWorldOrigin(origin);
						a_data.commonPopup.SetPosition(sp, { 0.5f, 1.f });

						stl::snprintf(b, "###i3di_obj_tt_%p", this);

						a_data.commonPopup.Draw(b, a_data, [&] {
							DrawTooltip(a_data);
						});
					}
				}
			}

			if (WantDrawContents())
			{
				stl::snprintf(b, "i3di_obj_c_%p", this);

				ImGui::PushID(b);

				DrawContents(a_data);

				ImGui::PopID();
			}
		}

	}
}