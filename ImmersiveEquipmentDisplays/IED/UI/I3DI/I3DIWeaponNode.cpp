#include "pch.h"

#include "I3DIWeaponNode.h"

#include "Common/VectorMath.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		I3DIWeaponNode::I3DIWeaponNode(
			ID3D11Device*                        a_device,
			const std::shared_ptr<D3DModelData>& a_data,
			const stl::fixed_string&             a_name,
			I3DIActorContext&                    a_actorContext) :
			I3DIObject(a_device, a_data),
			I3DIDraggable(DRAGGABLE_TYPE, *this),
			m_name(a_name),
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
	}
}