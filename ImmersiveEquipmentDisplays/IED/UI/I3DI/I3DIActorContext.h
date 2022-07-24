#pragma once

#include "I3DICamera.h"
#include "I3DIMOVNode.h"
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
				const std::shared_ptr<I3DIActorObject>& a_actorObject);

			void RegisterObjects(I3DIObjectController& a_objectController);
			void UnregisterObjects(I3DIObjectController& a_objectController);

			bool Update();
			void Draw(I3DICommonData& a_data);
			void Render(I3DICommonData& a_data);
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

			std::unordered_map<stl::fixed_string, std::shared_ptr<I3DIWeaponNode>> m_weaponNodes;

			bool m_ranFirstUpdate{ false };
			bool m_lastUpdateFailed{ false };

			std::shared_ptr<I3DIActorObject> m_actorObject;
			std::unique_ptr<I3DICamera>      m_camera;

			Controller& m_controller;
		};
	}
}