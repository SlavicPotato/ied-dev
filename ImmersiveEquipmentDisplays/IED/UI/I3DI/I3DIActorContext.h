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

			[[nodiscard]] inline constexpr auto& GetController() const noexcept
			{
				return m_controller;
			}

			[[nodiscard]] inline constexpr auto& GetActorFormID() const noexcept
			{
				return m_actor;
			}

			[[nodiscard]] inline constexpr bool LastUpdateFailed() const noexcept
			{
				return m_lastUpdateFailed;
			}

			[[nodiscard]] inline constexpr auto& GetActorObject() const noexcept
			{
				return m_actorObject;
			}

			[[nodiscard]] inline constexpr auto& GetMOVNodes() const noexcept
			{
				return m_movNodes;
			}

			/*[[nodiscard]] inline constexpr auto& GetMOVPairs() const noexcept
			{
				return m_movPairs;
			}*/

			[[nodiscard]] inline constexpr auto& GetWeaponNodes() const noexcept
			{
				return m_weaponNodes;
			}

			inline void SetCamera(std::unique_ptr<I3DICamera>&& a_camera) noexcept
			{
				m_camera = std::move(a_camera);
			}

			[[nodiscard]] inline constexpr auto& GetCamera() const noexcept
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

			std::shared_ptr<I3DIActorObject> m_actorObject;
			std::unique_ptr<I3DICamera>      m_camera;

			Controller& m_controller;
		};
	}
}