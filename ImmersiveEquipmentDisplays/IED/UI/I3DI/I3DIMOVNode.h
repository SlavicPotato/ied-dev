#pragma once

#include "I3DIDropTarget.h"
#include "I3DIModelObject.h"

namespace IED
{
	namespace UI
	{
		class I3DIWeaponNode;
		class I3DIActorContext;

		class ID3IMOVNode :
			public I3DIModelObject,
			public I3DIDropTarget
		{
		public:
			ID3IMOVNode(
				ID3D11Device*                        a_device,
				ID3D11DeviceContext*                 a_context,
				const std::shared_ptr<D3DModelData>& a_data,
				const stl::fixed_string&             a_name,
				I3DIWeaponNode&                      a_acceptedCandidate,
				I3DIActorContext&                    a_actorContext);

			virtual ~ID3IMOVNode() noexcept override = default;

			void SetAdjustedWorldMatrix(const NiTransform& a_worldTransform);

			[[nodiscard]] inline constexpr auto& GetNodeName() const noexcept
			{
				return m_name;
			}

		private:
			virtual bool AcceptsDraggable(I3DIDraggable& a_item) override;
			virtual bool ProcessDropRequest(I3DIDraggable& a_item) override;

			virtual DirectX::XMVECTOR XM_CALLCONV GetParentCenter() const override;

			stl::fixed_string m_name;

			I3DIWeaponNode& m_acceptedCandidate;

			I3DIActorContext& m_actorContext;
		};
	}
}