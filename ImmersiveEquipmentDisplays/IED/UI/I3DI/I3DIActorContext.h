#pragma once

#include "I3DICamera.h"
#include "I3DIMOVNode.h"
#include "I3DIPhysicsObject.h"
#include "I3DIWeaponNode.h"

namespace IED
{
	class Controller;
	class ActorObjectHolder;

	namespace Handlers
	{
		struct MouseMoveEvent;
	}

	namespace UI
	{
		struct I3DICommonData;
		class I3DIObjectController;
		class I3DIActorObject;

		class I3DIActorContext
		{
		public:
			I3DIActorContext(
				I3DICommonData&                         a_data,
				Controller&                             a_controller,
				const ActorObjectHolder&                a_holder,
				const std::shared_ptr<I3DIActorObject>& a_actorObject) noexcept(false);

			void RegisterObjects(I3DIObjectController& a_objectController);
			void UnregisterObjects(I3DIObjectController& a_objectController);

			bool Update(I3DICommonData& a_data);
			void Draw(I3DICommonData& a_data);
			void UpdateCamera(NiCamera* a_camera);

			void OnMouseMoveEvent(
				I3DICommonData&                 a_data,
				const Handlers::MouseMoveEvent& a_evn);

			[[nodiscard]] constexpr auto& GetController() const noexcept
			{
				return m_controller;
			}

			[[nodiscard]] constexpr auto& GetActorFormID() const noexcept
			{
				return m_actor;
			}

			[[nodiscard]] constexpr bool LastUpdateFailed() const noexcept
			{
				return m_lastUpdateFailed;
			}

			[[nodiscard]] constexpr auto& GetActorObject() const noexcept
			{
				return m_actorObject;
			}

			[[nodiscard]] constexpr auto& GetMOVNodes() const noexcept
			{
				return m_movNodes;
			}

			/*[[nodiscard]] constexpr auto& GetMOVPairs() const noexcept
			{
				return m_movPairs;
			}*/

			[[nodiscard]] constexpr auto& GetWeaponNodes() const noexcept
			{
				return m_weaponNodes;
			}

			inline void SetCamera(std::unique_ptr<I3DICamera>&& a_camera) noexcept
			{
				m_camera = std::move(a_camera);
			}

			template <class T, class... Args>
			constexpr void CreateCamera(Args&&... a_args) noexcept
			{
				m_camera = std::make_unique<T>(std::forward<Args>(a_args)...);
			}

			constexpr void SetAnimEventSent() noexcept
			{
				m_animEventSent = true;
			}

			[[nodiscard]] constexpr auto GetAnimEventSent() noexcept
			{
				return m_animEventSent;
			}

			[[nodiscard]] constexpr auto& GetCamera() const noexcept
			{
				return m_camera;
			}

			[[nodiscard]] inline bool HasCamera() const noexcept
			{
				return static_cast<bool>(m_camera.get());
			}

		private:

			Game::FormID m_actor;

			stl::unordered_map<stl::fixed_string, std::shared_ptr<I3DIWeaponNode>> m_weaponNodes;
			stl::unordered_map<stl::fixed_string, std::shared_ptr<I3DIMOVNode>>    m_movNodes;
			stl::unordered_map<luid_tag, std::shared_ptr<I3DIPhysicsObject>>       m_physicsObjects;

			std::shared_ptr<D3DModelData> m_sphereModel;

			/*struct objpair_t
			{
				I3DIMOVNode* first;
				I3DIMOVNode* second;

				bool operator<(const objpair_t& other) const
				{
					return first < other.first || (first == other.first && second < other.second);
				}

				bool operator==(const objpair_t& other) const
				{
					return first == other.first && second == other.second;
				}
			};*/

			//stl::vector<std::pair<I3DIMOVNode*, I3DIMOVNode*>> m_movPairs;

			bool m_ranFirstUpdate{ false };
			bool m_lastUpdateFailed{ false };
			bool m_animEventSent{ false };

			std::shared_ptr<I3DIActorObject> m_actorObject;
			std::unique_ptr<I3DICamera>      m_camera;

			Controller& m_controller;
		};
	}
}