#pragma once

#include "I3DIDraggable.h"
#include "I3DIObject.h"

namespace IED
{
	namespace UI
	{
		class I3DIActorContext;

		class I3DIWeaponNode :
			public I3DIObject,
			public I3DIDraggable
		{
		public:
			inline static constexpr std::uint32_t DRAGGABLE_TYPE = 1;

			I3DIWeaponNode(
				ID3D11Device*                        a_device,
				const std::shared_ptr<D3DModelData>& a_data,
				const stl::fixed_string&             a_name,
				I3DIActorContext&                    a_actorContext);

			void UpdateLocalMatrix(const NiTransform& a_transform);

			inline constexpr auto& GetLocalMatrix() const noexcept
			{
				return m_localMatrix;
			}

			inline constexpr auto& GetNodeName() const noexcept
			{
				return m_name;
			}

			virtual I3DIDraggable* GetAsDraggable() override
			{
				return static_cast<I3DIDraggable*>(this);
			};

		private:
			virtual bool OnDragBegin() override;
			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) override;

			stl::fixed_string m_name;

			DirectX::XMMATRIX m_localMatrix{ DirectX::SimpleMath::Matrix::Identity };

			I3DIActorContext& m_actorContext;
		};
	}
}