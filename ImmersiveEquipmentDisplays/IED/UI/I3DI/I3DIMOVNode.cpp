#include "pch.h"

#include "I3DIMOVNode.h"

#include "I3DIActorContext.h"
#include "I3DIActorObject.h"
#include "I3DIWeaponNode.h"

#include "Common/VectorMath.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		ID3IMOVNode::ID3IMOVNode(
			ID3D11Device*                        a_device,
			ID3D11DeviceContext*                 a_context,
			const std::shared_ptr<D3DModelData>& a_data,
			const stl::fixed_string&             a_name,
			I3DIWeaponNode&                      a_acceptedCandidate,
			I3DIActorContext&                    a_actorContext) :
			I3DIModelObject(a_device, a_context, a_data),
			I3DIDropTarget(*this),
			m_name(a_name),
			m_acceptedCandidate(a_acceptedCandidate),
			m_actorContext(a_actorContext)
		{
		}

		void ID3IMOVNode::SetAdjustedWorldMatrix(const NiTransform& a_worldTransform)
		{
			auto world = VectorMath::NiTransformTo4x4Matrix(a_worldTransform);

			UpdateWorldMatrix(world * m_acceptedCandidate.GetLocalMatrix());
		}

		bool ID3IMOVNode::AcceptsDraggable(I3DIDraggable& a_item)
		{
			return std::addressof(static_cast<I3DIDraggable&>(m_acceptedCandidate)) == std::addressof(a_item);
		}

		bool ID3IMOVNode::ProcessDropRequest(I3DIDraggable&)
		{
			auto& controller = m_actorContext.GetController();

			auto& actorid = m_actorContext.GetActorFormID();

			/*auto& data = controller.GetData();

			auto it = data.find(m_actorContext.GetActorFormID());
			if (it == data.end())
			{
				return false;
			}*/

			//controller.GetConfigStore().

			controller.RequestEvaluate(actorid, false, true, true);

			return false;
		}

		DirectX::XMVECTOR XM_CALLCONV ID3IMOVNode::GetParentCenter() const
		{
			return DirectX::XMLoadFloat3(std::addressof(m_actorContext.GetActorObject()->GetAsActorObject()->GetActorBound().Center));
		}
	}
}