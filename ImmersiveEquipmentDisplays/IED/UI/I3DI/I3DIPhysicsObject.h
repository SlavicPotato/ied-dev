#pragma once

#include "IED/ConfigNodePhysicsValues.h"

//#include "I3DIBoundObject.h"
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
			public I3DIModelObject
		{
			static inline constexpr std::uint32_t NB_SPHERE_DETAIL_FACTOR = 2;

			static inline constexpr std::uint32_t NB_SECTORS_SPHERE = 9 * NB_SPHERE_DETAIL_FACTOR;
			static inline constexpr std::uint32_t NB_STACKS_SPHERE  = 5 * NB_SPHERE_DETAIL_FACTOR;

		public:
			I3DIPhysicsObject(
				I3DICommonData&        a_data,
				I3DIActorContext&      a_actorContext,
				const PHYSimComponent& a_sc) noexcept(false);

			void UpdateData(const PHYSimComponent& a_sc);

			//virtual void UpdateBound() override;

			[[nodiscard]] inline constexpr auto& GetTag() const noexcept
			{
				return m_tag;
			}

			virtual void OnObjectRegistered(I3DIObjectController& a_data) override;
			virtual void OnObjectUnregistered(I3DIObjectController& a_data) override;

		private:
			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;

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

			static void XM_CALLCONV DrawSphere(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_pos,
				float              a_radius,
				DirectX::XMVECTOR  a_color);

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
		};
	}
}
