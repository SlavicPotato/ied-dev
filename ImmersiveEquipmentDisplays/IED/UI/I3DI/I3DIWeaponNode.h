#pragma once

#include "I3DIDraggable.h"
#include "I3DIModelObject.h"

#include "IED/Controller/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		class I3DIActorContext;

		class I3DIWeaponNode :
			public I3DIModelObject,
			public I3DIDraggable
		{
		public:
			inline static constexpr std::uint32_t DRAGGABLE_TYPE = 1;

			I3DIWeaponNode(
				ID3D11Device*                              a_device,
				ID3D11DeviceContext*                       a_context,
				const std::shared_ptr<D3DModelData>&       a_data,
				const stl::fixed_string&                   a_nodeName,
				const NodeOverrideData::weaponNodeEntry_t& a_nodeInfo,
				I3DIActorContext&                          a_actorContext);

			virtual ~I3DIWeaponNode() noexcept override = default;

			void UpdateLocalMatrix(const NiTransform& a_transform);

			inline constexpr auto& GetLocalMatrix() const noexcept
			{
				return m_localMatrix;
			}

			inline constexpr auto& GetNodeName() const noexcept
			{
				return m_nodeName;
			}
			
			inline constexpr auto GetNodeDesc() const noexcept
			{
				return m_nodeInfo.desc;
			}

			virtual I3DIDraggable* GetAsDraggable() override
			{
				return static_cast<I3DIDraggable*>(this);
			};

		private:
			virtual bool OnDragBegin() override;
			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) override;
			virtual void DrawTooltip(I3DICommonData& a_data) override;

			virtual bool WantDrawTooltip();
			virtual bool WantDrawBound();

			virtual bool IsSelectable() override;

			virtual DirectX::XMVECTOR XM_CALLCONV GetParentCenter() const override;

			stl::fixed_string                          m_nodeName;
			const NodeOverrideData::weaponNodeEntry_t& m_nodeInfo;

			DirectX::XMMATRIX m_localMatrix{ DirectX::SimpleMath::Matrix::Identity };

			I3DIActorContext& m_actorContext;
		};
	}
}