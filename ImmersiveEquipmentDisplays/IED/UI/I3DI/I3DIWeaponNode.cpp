#include "pch.h"

#include "I3DIWeaponNode.h"

#include "I3DIActorContext.h"
#include "I3DIActorObject.h"
#include "I3DIBoundingSphere.h"
#include "I3DICommonData.h"

#include "Common/VectorMath.h"

#include "IED/Controller/Controller.h"

#include "IED/D3D/D3DModelData.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIWeaponNode::I3DIWeaponNode(
			ID3D11Device*                              a_device,
			const std::shared_ptr<D3DModelData>&       a_data,
			const stl::fixed_string&                   a_nodeName,
			const NodeOverrideData::weaponNodeEntry_t& a_nodeInfo,
			I3DIActorContext&                          a_actorContext) :
			I3DIModelObject(
				a_device,
				BoundingShape::kOrientedBox,
				a_data),
			I3DIDraggable(I3DIDraggableType::Dynamic),
			m_nodeName(a_nodeName),
			m_nodeInfo(a_nodeInfo),
			m_actorContext(a_actorContext)
		{
		}

		void I3DIWeaponNode::UpdateLocalMatrix(
			const NiTransform& a_transform)
		{
			m_localMatrix = VectorMath::NiTransformToMatrix4x4(a_transform);
		}

		I3DIBoundObject* I3DIWeaponNode::GetParentObject() const
		{
			return m_actorContext.GetActorObject().get();
		}

		bool I3DIWeaponNode::OnDragBegin(I3DICommonData& a_data, ImGuiMouseButton a_button)
		{
			if (a_button == ImGuiMouseButton_Left)
			{
				m_oldDiffuse.emplace(m_diffuseColor);
				SetDiffuseColor({ 1.0f, 0, 0, 1.0f });

				EnableDepth(false);

				return true;
			}
			else
			{
				return false;
			}
		}

		void I3DIWeaponNode::OnDragEnd(
			I3DIDragDropResult a_result,
			I3DIDropTarget*    a_target)
		{
			if (m_oldDiffuse)
			{
				SetDiffuseColor(*m_oldDiffuse);
				m_oldDiffuse.reset();
			}

			EnableDepth(true);
		}

		void I3DIWeaponNode::OnDragUpdate(I3DICommonData& a_data)
		{
			/*PerfTimer pt;
			pt.Start();*/

			for (auto& e : m_actorContext.GetMOVNodes())
			{
				const auto mov = e.second.get();

				if (!mov->AcceptsDraggable(*this))
				{
					continue;
				}

				const auto m = m_localMatrix * mov->GetOriginalWorldMatrix();

				mov->UpdateWorldMatrix(m);
				mov->SetOriginalPos(m.r[3]);
				mov->UpdateBound();
			}

			auto& data = m_actorContext.GetMOVNodes();

			for (auto ita = data.cbegin(); ita != data.cend(); ++ita)
			{
				const auto mova = ita->second.get();

				if (!mova->AcceptsDraggable(*this))
				{
					continue;
				}

				for (auto itb = ita; ++itb != data.cend();)
				{
					auto movb = itb->second.get();

					if (!movb->AcceptsDraggable(*this))
					{
						continue;
					}

					const auto& sphereA = mova->GetBoundingShape<I3DIBoundingSphere>()->GetBound();
					const auto& sphereB = movb->GetBoundingShape<I3DIBoundingSphere>()->GetBound();

					const auto centerA = XMLoadFloat3(std::addressof(sphereA.Center));
					const auto radiusA = XMVectorReplicatePtr(std::addressof(sphereA.Radius));

					const auto centerB = XMLoadFloat3(std::addressof(sphereB.Center));
					const auto radiusB = XMVectorReplicatePtr(std::addressof(sphereB.Radius));

					const auto diff = centerB - centerA;
					const auto len  = XMVector3Length(diff);

					const auto radSum = radiusA + radiusB;

					if (XMVector3Greater(len, radSum))
					{
						continue;
					}

					const auto penHalfDist = (len - radSum) * DirectX::g_XMOneHalf;

					const auto normal =
						XMVector3Greater(len, DirectX::g_XMEpsilon) ?
							diff / len :
                            DirectX::g_XMIdentityR0;

					const auto p1 = mova->GetWorldPosition();
					const auto p2 = movb->GetWorldPosition();

					const auto offset = normal * penHalfDist;

					mova->SetWorldPosition(p1 + offset);
					movb->SetWorldPosition(p2 - offset);

					mova->UpdateBound();
					movb->UpdateBound();
				}
			}

			for (auto& e : m_actorContext.GetMOVNodes())
			{
				const auto* const mov = e.second.get();

				if (!mov->AcceptsDraggable(*this))
				{
					continue;
				}

				const auto initial = mov->GetOriginalPos();
				const auto current = mov->GetWorldPosition();

				if (!XMVector3Equal(initial, current))
				{
					constexpr XMVECTOR colGreen = { 0, 1, 0, 1 };
					constexpr XMVECTOR colRed   = { 1, 0, 0, 1 };

					a_data.batchNoDepth.AddLine(initial, colRed, current, colGreen);
				}
			}

			//_DMESSAGE("%f", pt.Stop());
		}

		void I3DIWeaponNode::DrawTooltip(I3DICommonData& a_data)
		{
			ImGui::Text("%s [%s]", m_nodeInfo.desc, m_nodeName.c_str());
		}

		bool I3DIWeaponNode::WantDrawTooltip()
		{
			return (m_objectFlags & I3DIObjectFlags::kHSMask) == I3DIObjectFlags::kHovered;
		}

		bool I3DIWeaponNode::WantDrawBound()
		{
			return m_objectFlags.test_any(I3DIObjectFlags::kHSMask);
		}

		bool I3DIWeaponNode::WantDrawContents()
		{
			return IsSelected();
		}

		bool I3DIWeaponNode::IsSelectable()
		{
			return true;
		}

		void I3DIWeaponNode::DrawContents(I3DICommonData& a_data)
		{
		}

		bool I3DIWeaponNode::ShouldProcess(I3DICommonData& a_data)
		{
			return a_data.controller.GetSettings().data.ui.i3di.enableWeapons;
		}

	}
}