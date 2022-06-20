#pragma once

#include "I3DIDropTarget.h"
#include "I3DIObject.h"

namespace IED
{
	namespace UI
	{
		class I3DIWeaponNode;
		class I3DIActorContext;

		class ID3IMOVNode :
			public I3DIObject,
			public I3DIDropTarget
		{
		public:
			ID3IMOVNode(
				ID3D11Device*                        a_device,
				const std::shared_ptr<D3DModelData>& a_data,
				const stl::fixed_string&             a_name,
				I3DIWeaponNode&                      a_acceptedCandidate,
				I3DIActorContext&                    a_actorContext);

			void SetAdjustedWorldMatrix(const NiTransform& a_worldTransform);

			[[nodiscard]] inline constexpr auto& GetNodeName() const noexcept
			{
				return m_name;
			}

		private:
			virtual bool AcceptsDraggable(I3DIDraggable& a_item) override;
			virtual bool ProcessDropRequest(I3DIDraggable& a_item) override;

			stl::fixed_string m_name;

			I3DIWeaponNode& m_acceptedCandidate;

			I3DIActorContext& m_actorContext;
		};
	}
}