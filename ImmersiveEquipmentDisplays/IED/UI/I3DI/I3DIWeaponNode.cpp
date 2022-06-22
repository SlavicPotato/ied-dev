#include "pch.h"

#include "I3DIActorContext.h"
#include "I3DIActorObject.h"
#include "I3DIWeaponNode.h"

#include "Common/VectorMath.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		I3DIWeaponNode::I3DIWeaponNode(
			ID3D11Device*                              a_device,
			ID3D11DeviceContext*                       a_context,
			const std::shared_ptr<D3DModelData>&       a_data,
			const stl::fixed_string&                   a_nodeName,
			const NodeOverrideData::weaponNodeEntry_t& a_nodeInfo,
			I3DIActorContext&                          a_actorContext) :
			I3DIModelObject(a_device, a_context, a_data),
			I3DIDraggable(DRAGGABLE_TYPE, *this),
			m_nodeName(a_nodeName),
			m_nodeInfo(a_nodeInfo),
			m_actorContext(a_actorContext)
		{
		}

		void I3DIWeaponNode::UpdateLocalMatrix(
			const NiTransform& a_transform)
		{
			m_localMatrix = VectorMath::NiTransformTo4x4Matrix(a_transform);
		}

		bool I3DIWeaponNode::OnDragBegin()
		{
			return true;
		}

		void I3DIWeaponNode::OnDragEnd(
			I3DIDragDropResult a_result,
			I3DIDropTarget*    a_target)
		{
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

		DirectX::XMVECTOR XM_CALLCONV I3DIWeaponNode::GetParentCenter() const
		{
			return DirectX::XMLoadFloat3(std::addressof(m_actorContext.GetActorObject()->GetAsActorObject()->GetActorBound().Center));
		}
	}
}