#pragma once

#include "I3DIDraggable.h"
#include "I3DIModelObject.h"

#include "IED/NodeOverrideData.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		class I3DIActorContext;
		struct I3DICommonData;

		class I3DIWeaponNode :
			public I3DIModelObject,
			public I3DIDraggable
		{
		public:

			I3DIWeaponNode(
				ID3D11Device*                              a_device,
				const std::shared_ptr<D3DModelData>&       a_data,
				const stl::fixed_string&                   a_nodeName,
				const NodeOverrideData::weaponNodeEntry_t& a_nodeInfo,
				I3DIActorContext&                          a_actorContext);

			virtual ~I3DIWeaponNode() noexcept override = default;

			virtual I3DIWeaponNode* AsWeaponNode() override
			{
				return this;
			};

			virtual I3DIDraggable* AsDraggable() override
			{
				return static_cast<I3DIDraggable*>(this);
			};

			virtual I3DIObject& GetDraggableObject() override
			{
				return static_cast<I3DIObject&>(*this);
			}

			void UpdateLocalMatrix(const NiTransform& a_transform);

			[[nodiscard]] constexpr auto XM_CALLCONV GetLocalMatrix() const noexcept
			{
				return m_localMatrix;
			}

			[[nodiscard]] constexpr auto& GetNodeName() const noexcept
			{
				return m_nodeName;
			}

			[[nodiscard]] constexpr auto GetNodeDesc() const noexcept
			{
				return m_nodeInfo.desc;
			}

			virtual I3DIBoundObject* GetParentObject() const override;

		private:
			virtual bool OnDragBegin(I3DICommonData& a_data, ImGuiMouseButton a_button) override;
			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) override;
			virtual void OnDragUpdate(I3DICommonData& a_data) override;

			virtual void DrawTooltip(I3DICommonData& a_data) override;

			virtual bool WantDrawTooltip();
			virtual bool WantDrawBound();
			virtual bool WantDrawContents();

			virtual bool IsSelectable() override;

			virtual void DrawContents(I3DICommonData& a_data) override;

			virtual bool ShouldProcess(I3DICommonData& a_data) override;

			const stl::fixed_string                    m_nodeName;
			const NodeOverrideData::weaponNodeEntry_t& m_nodeInfo;

			DirectX::XMMATRIX m_localMatrix{ VectorMath::g_identity };

			std::optional<DirectX::XMVECTOR> m_oldDiffuse;

			I3DIActorContext& m_actorContext;
		};
	}
}