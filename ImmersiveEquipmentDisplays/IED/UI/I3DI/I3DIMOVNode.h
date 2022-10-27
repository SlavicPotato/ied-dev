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

		class I3DIMOVNode :
			public I3DIModelObject,
			public I3DIDropTarget
		{
		public:
			I3DIMOVNode(
				ID3D11Device*                                  a_device,
				ID3D11DeviceContext*                           a_context,
				const std::shared_ptr<D3DModelData>&           a_data,
				const stl::fixed_string&                       a_name,
				const NodeOverrideData::weaponNodeListEntry_t& a_nodeInfo,
				const NodeOverrideData::overrideNodeEntry_t*   a_cmeNodeInfo,
				I3DIWeaponNode&                                a_acceptedDraggable,
				I3DIActorContext&                              a_actorContext);

			virtual ~I3DIMOVNode() noexcept override = default;

			virtual I3DIMOVNode* GetAsMOVNode() override
			{
				return this;
			};

			virtual I3DIDropTarget* GetAsDropTarget() override
			{
				return static_cast<I3DIDropTarget*>(this);
			};

			void SetAdjustedWorldMatrix(const NiTransform& a_worldTransform);

			[[nodiscard]] inline constexpr auto& GetNodeName() const noexcept
			{
				return m_name;
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

			virtual I3DIObject* GetParentObject() const;

		private:
			virtual void DrawTooltip(I3DICommonData& a_data) override;

			virtual bool AcceptsDraggable(I3DIDraggable& a_item) override;
			virtual bool ProcessDropRequest(I3DIDraggable& a_item) override;

			virtual bool ShouldProcess(I3DICommonData& a_data) override;

			virtual bool WantDrawTooltip() override;

			const stl::fixed_string                        m_name;
			const NodeOverrideData::weaponNodeListEntry_t& m_nodeInfo;
			const NodeOverrideData::overrideNodeEntry_t*   m_cmeNodeInfo{ nullptr };

			bool m_weaponNodeAttached{ false };
			//bool m_hasVisibleGeometry{ false };

			I3DIWeaponNode& m_weaponNode;

			I3DIActorContext& m_actorContext;
		};
	}
}