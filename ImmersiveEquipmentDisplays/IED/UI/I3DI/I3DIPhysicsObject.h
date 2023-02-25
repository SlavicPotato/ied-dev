#pragma once

#include "IED/ConfigNodePhysicsValues.h"

//#include "I3DIBoundObject.h"
#include "I3DIDraggable.h"
#include "I3DIModelObject.h"

namespace IED
{
	class PHYSimComponent;
	class D3DPrimitiveBatch;

	namespace UI
	{
		class I3DIActorContext;
		class I3DIObjectController;
		class I3DISphereObject;

		class I3DIPhysicsObject :
			public I3DIModelObject,
			public I3DIDraggable
		{
			static constexpr std::uint32_t NB_SPHERE_DETAIL_FACTOR = 2;

			static constexpr std::uint32_t NB_SECTORS_SPHERE = 9 * NB_SPHERE_DETAIL_FACTOR;
			static constexpr std::uint32_t NB_STACKS_SPHERE  = 5 * NB_SPHERE_DETAIL_FACTOR;

		public:
			I3DIPhysicsObject(
				I3DICommonData&        a_data,
				I3DIActorContext&      a_actorContext,
				const PHYSimComponent& a_sc) noexcept(false);

			virtual I3DIDraggable* AsDraggable() override
			{
				return static_cast<I3DIDraggable*>(this);
			};

			virtual I3DIObject& GetDraggableObject() override
			{
				return static_cast<I3DIObject&>(*this);
			}

			void UpdateData(const PHYSimComponent& a_sc);

			//virtual void UpdateBound() override;

			[[nodiscard]] constexpr auto& GetLUID() const noexcept
			{
				return m_tag;
			}

			virtual void OnObjectRegistered(I3DIObjectController& a_data) override;
			virtual void OnObjectUnregistered(I3DIObjectController& a_data) override;

		private:
			virtual bool OnDragBegin(I3DICommonData& a_data, ImGuiMouseButton a_button) override;
			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) override;
			virtual void OnDragUpdate(I3DICommonData& a_data) override;

			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;
			//virtual void OnMouseDown(I3DICommonData& a_data, ImGuiMouseButton a_button) override;

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;

			virtual bool IsSelectable() override;
			virtual bool OnSelect(I3DICommonData& a_data) override;
			virtual void OnUnselect(I3DICommonData& a_data) override;

			void XM_CALLCONV DrawImpl(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color);

			void XM_CALLCONV DrawBoxConstraint(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color);

			/*void XM_CALLCONV DrawSphereConstraint(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color);*/

			void DrawOrientation(
				D3DPrimitiveBatch& a_batch);

			Bullet::btTransformEx                   m_parentTransform;
			Bullet::btTransformEx                   m_objectTransform;
			btVector3                               m_bbMin;
			btVector3                               m_bbMax;
			btVector3                               m_sphereOrigin;
			btVector3                               m_axis;
			float                                   m_sphereRadius;
			stl::flag<Data::ConfigNodePhysicsFlags> m_flags{ Data::ConfigNodePhysicsFlags::kNone };

			I3DIActorContext& m_actorContext;

			std::string m_name;
			luid_tag    m_tag;

			std::shared_ptr<I3DISphereObject> m_virtSphere;
			std::shared_ptr<I3DISphereObject> m_constraintSphere;
			std::shared_ptr<I3DISphereObject> m_cogSphere;

			std::optional<std::pair<float, DirectX::XMVECTOR>> m_afd;

			/*btVector3             m_virtld;
			btVector3             m_velocity;
			Bullet::btTransformEx m_initialTransform;*/
		};
	}
}
