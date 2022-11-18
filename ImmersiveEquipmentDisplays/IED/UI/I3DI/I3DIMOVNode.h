#pragma once

#include "I3DIDropTarget.h"
#include "I3DIModelObject.h"

#include "IED/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		class I3DIWeaponNode;
		class I3DIActorContext;
		class I3DIObjectController;

		class I3DIMOVNode :
			public I3DIModelObject,
			public I3DIDropTarget
		{
			friend class I3DIObjectController;

		public:
			I3DIMOVNode(
				ID3D11Device*                                  a_device,
				const std::shared_ptr<D3DModelData>&           a_data,
				const stl::fixed_string&                       a_name,
				const NodeOverrideData::weaponNodeListEntry_t& a_nodeInfo,
				const NodeOverrideData::overrideNodeEntry_t*   a_cmeNodeInfo,
				I3DIWeaponNode&                                a_acceptedDraggable,
				I3DIActorContext&                              a_actorContext);

			virtual ~I3DIMOVNode() noexcept override = default;

			virtual I3DIMOVNode* AsMOVNode() override
			{
				return this;
			};

			virtual I3DIDropTarget* AsDropTarget() override
			{
				return static_cast<I3DIDropTarget*>(this);
			};

			[[nodiscard]] inline constexpr auto& GetNodeName() const noexcept
			{
				return m_name;
			}

			[[nodiscard]] inline constexpr auto& GetWeaponNode() const noexcept
			{
				return m_weaponNode;
			}

			inline constexpr void SetWeaponNodeAttached(bool a_switch) noexcept
			{
				m_weaponNodeAttached = a_switch;
			}

			/*inline constexpr void SetHasVisibleGeometry(bool a_switch) noexcept
			{
				m_hasVisibleGeometry = a_switch;
			}

			[[nodiscard]] inline constexpr auto HasVisibleGeometry() const noexcept
			{
				return m_hasVisibleGeometry;
			}*/

			[[nodiscard]] inline constexpr auto GetCMENodeInfo() const noexcept
			{
				return m_cmeNodeInfo;
			}

			inline constexpr void XM_CALLCONV SetOriginalPos(DirectX::XMVECTOR a_pos) noexcept
			{
				m_originalPos = a_pos;
			}

			inline constexpr auto XM_CALLCONV GetOriginalPos() const noexcept
			{
				return m_originalPos;
			}

			virtual I3DIBoundObject* GetParentObject() const override;

			virtual bool AcceptsDraggable(I3DIDraggable& a_item) const override;

		private:
			virtual void DrawTooltip(I3DICommonData& a_data) override;

			virtual bool ProcessDropRequest(I3DIDraggable& a_item) override;
			void         OnDraggableMovingOver(I3DIDraggable& a_item) override;

			virtual bool ShouldProcess(I3DICommonData& a_data) override;

			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;

			virtual bool WantDrawTooltip() override;

			const stl::fixed_string                        m_name;
			const NodeOverrideData::weaponNodeListEntry_t& m_nodeInfo;
			const NodeOverrideData::overrideNodeEntry_t*   m_cmeNodeInfo{ nullptr };

			bool m_weaponNodeAttached{ false };
			//bool m_hasVisibleGeometry{ false };

			DirectX::XMVECTOR m_originalPos{ DirectX::g_XMZero.v };

			I3DIWeaponNode& m_weaponNode;

			I3DIActorContext& m_actorContext;
		};
	}
}